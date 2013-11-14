/* -----------------------------------------------------------------------------
** Copyright (c) 2012 Advanced Resources and Risk Technology, LLC
** All rights reserved.
**
** This file is part of Advanced Resources and Risk Technology, LLC (AR2TECH) 
** version of the open source software sgems.  It is a derivative work by 
** AR2TECH (THE LICENSOR) based on the x-free license granted in the original 
** version of the software (see notice below) and now sublicensed such that it 
** cannot be distributed or modified without the explicit and written permission 
** of AR2TECH.
**
** Only AR2TECH can modify, alter or revoke the licensing terms for this 
** file/software.
**
** This file cannot be modified or distributed without the explicit and written 
** consent of AR2TECH.
**
** Contact Dr. Alex Boucher (aboucher@ar2tech.com) for any questions regarding
** the licensing of this file/software
**
** The open-source version of sgems can be downloaded at 
** sourceforge.net/projects/sgems.
** ----------------------------------------------------------------------------*/



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#include <geostat/cosgsim.h>
#include <geostat/parameters_handler.h>
#include <utils/gstl_messages.h>
#include <utils/string_manipulation.h>
#include <utils/error_messages_handler.h>
#include <grid/geostat_grid.h>
#include <grid/strati_grid.h>
#include <grid/point_set.h>
#include <grid/grid_property.h>
#include <grid/grid_property_manager.h>
#include <grid/combined_neighborhood.h>
#include <grid/gval_iterator.h>
#include <grid/property_copier.h>
#include <grid/grid_path.h>
#include <utils/manager_repository.h>
#include <math/random_numbers.h>
#include <appli/utilities.h>

#include <GsTL/kriging/cokriging_weights.h>
#include <GsTL/kriging/LMC_covariance.h>
#include <GsTL/kriging/MM1_covariance.h>
#include <GsTL/kriging/MM2_covariance.h>
#include <GsTL/matrix_library/tnt/cmat.h>
#include <GsTL/cdf/gaussian_cdf.h>
#include <GsTL/sampler/monte_carlo_sampler.h>
#include <GsTL/cdf_estimator/gaussian_cdf_coKestimator.h>
#include <GsTL/simulation/sequential_cosimulation.h>
#include <GsTL/univariate_stats/cdf_transform.h>
#include <GsTL/univariate_stats/build_cdf.h>


 

int Cosgsim::execute( GsTL_project* ) {

  // Initialize the global random number generator
  Global_random_number_generator::instance()->seed( seed_ );
  
  
  // Set up a progress notifier	
  int total_steps = simul_grid_->size() * (nb_of_realizations_);
  int frequency = std::max( total_steps / 20, 1 );
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running CoSgsim", 
			    total_steps, frequency );

  // In sequential gaussian simulation, the marginal is a Gaussian cdf, 
  // with mean 0 and variance 1.
  Gaussian_cdf marginal( 0.0, 1.0 );
  Gaussian_cdf ccdf;

  // work on the fine grid
  if( dynamic_cast<Strati_grid*>( simul_grid_ ) ) {
    Strati_grid* sgrid = dynamic_cast<Strati_grid*>( simul_grid_ );
    sgrid->set_level( 1 );
  }

  // set up the cdf-estimator
  typedef Gaussian_cdf_coKestimator< CovarianceSet,
                                     KrigingConstraints,
                                     CoKrigingCombiner
                                    >    CoKriging_cdf_estimator;
  CoKriging_cdf_estimator cdf_estimator( *covar_,
		                          		       *kconstraints_,
				                                 *combiner_ );

  // set up the sampler
  Random_number_generator gen;
  Monte_carlo_sampler_t< Random_number_generator > sampler( gen );
  

  // loop on all realizations
  Grid_path path(simul_grid_,simul_grid_->selected_property(),target_grid_region_);
  for( int nreal = 0; nreal < nb_of_realizations_ ; nreal ++ ) {

    // update the progress notifier
    progress_notifier->message() << "working on realization " 
                                 << nreal+1 << gstlIO::end;
    if( !progress_notifier->notify() ) {
      clean(); 
      return 1;
    }

    bool clean_sec_variable_after_each_simulation = false;
    if( secondary_variable_group_ != 0 ) {
      secondary_variable_ = secondary_variable_group_->get_property( nreal%secondary_variable_group_->size() );
      if( !secondary_variable_ ) return false;
      secondary_variable_ = 
      distribution_utils::gaussian_transform_property( secondary_variable_, sec_distribution_.raw_ptr(), sec_harddata_grid_ );
      neighborhood_vector_.back().select_property(secondary_variable_->name());
      clean_sec_variable_after_each_simulation = true;
      clean_secondary_var_ = false; // will be clean automatically after each simulation

    }


    // Create a new property to hold the realization and tell the simulation 
    // grid to use it as the current property 
    appli_message( "Creating new realization" );
    Grid_continuous_property* prop = multireal_property_->new_realization();
    prop->set_parameters(parameters_);;
    simul_grid_->select_property( prop->name() );
    neighborhood_vector_[0].select_property( prop->name() );

    // initialize the new realization with the hard data, if that was requested 
    if( property_copier_ ) {
      property_copier_->copy( prim_harddata_grid_, primary_variable_,
                              simul_grid_, prop );
    }
     
    // compute the random path
    path.set_property( prop->name() );
    path.randomize();
    //simul_grid_->init_random_path();
   
    appli_message( "Doing simulation" );
    // do the simulation
    int status = 
      sequential_cosimulation( path.begin(),
			                         path.end(),
			                         neighborhood_vector_.begin(), 
                               neighborhood_vector_.end(),
                  			       ccdf, cdf_estimator,
                  			       marginal,
                   			       sampler, progress_notifier.raw_ptr()
			                       );
    if( status == -1 ) {
      clean( prop );
      return 1;
    }
      
    // back-transform if needed
    if( transform_primary_variable_ ) {
      cdf_transform( prop->begin(), prop->end(), 
        marginal, *original_cdf_.raw_ptr() );
    }

    if( clean_sec_variable_after_each_simulation ) {
      clean( secondary_variable_ );
    }

  }

  clean();
  return 0;
}


void Cosgsim::clean( Grid_continuous_property* prop ) {
  if( prop ) 
    simul_grid_->remove_property( prop->name() );

  // if we transformed the primary/secondary variable, 
  // primary_variable_/secondary_variable_ points to a temporary property 
  // (the transformed hard data property) that we need to remove

  if( transform_primary_variable_ && clean_primary_var_ &&
      primary_variable_ && prim_harddata_grid_ ) {
    appli_message( "removing property " << primary_variable_->name() );
    prim_harddata_grid_->remove_property( primary_variable_->name() );
    primary_variable_ = 0;
  }
  if( transform_secondary_variable_ && clean_secondary_var_ &&
      secondary_variable_ && sec_harddata_grid_ ) {
    appli_message( "removing property " << secondary_variable_->name() );
    sec_harddata_grid_->remove_property( secondary_variable_->name() );
    secondary_variable_ = 0;
  }
}



bool Cosgsim::initialize( const Parameters_handler* parameters, 
                         Error_messages_handler* errors ) {

  // Extract the parameters input by the user from the parameter handler
  
  //-------------
  // The "simulation" grid parameters
  
  std::string simul_grid_name = parameters->value( "Grid_Name.value" );
  errors->report( simul_grid_name.empty(), 
            		  "Grid_Name", "No grid selected" );
  std::string property_name = parameters->value( "Property_Name.value" );
  errors->report( property_name.empty(), 
            		  "Property_Name", "No property name specified" );

  // Get the simulation grid from the grid manager  
  if( simul_grid_name.empty() ) return false;

  bool ok = geostat_utils::create( simul_grid_, simul_grid_name,
                      				 "Grid_Name", errors );
  if( !ok ) return false;

  // create  a multi-realization property
  multireal_property_ =
    simul_grid_->add_multi_realization_property( property_name );



  //-------------
  // The hard data parameters

  std::string prim_harddata_grid_name = 
    parameters->value( "Primary_Harddata_Grid.value" );

  if( !prim_harddata_grid_name.empty() ) {
    std::string primary_variable_name =
      parameters->value( "Primary_Variable.value" );

    // Get the harddata grid from the grid manager
    bool ok1 = geostat_utils::create( prim_harddata_grid_, prim_harddata_grid_name, 
      	                           	 "Primary_Harddata_Grid", errors );
    if( !ok1 ) return false;

    primary_variable_ = prim_harddata_grid_->property( primary_variable_name );
  }


  std::string sec_harddata_grid_name = 
    parameters->value( "Secondary_Harddata_Grid.value" );
  errors->report( sec_harddata_grid_name.empty(),
                  "Secondary_Harddata_Grid", "No secondary variable specified" );

  if(!errors->empty()) return false;
    
  // Get the harddata grid from the grid manager
  bool ok2 = geostat_utils::create( sec_harddata_grid_, sec_harddata_grid_name, 
  	                           	    "Secondary_Harddata_Grid", errors );
  if( !ok2 ) return false;


  // Does the secondary attribute is a single property or a group of property
  bool is_sec_group = parameters->value( "is_secondary_group.value" )=="1";

  if( is_sec_group ) {
    std::string secondary_variable_group_name =
      parameters->value( "Secondary_Variable_Group.value" );
    errors->report( secondary_variable_group_name.empty(), 
                    "Secondary_Variable_Group", "No group property name specified" );

    secondary_variable_group_ = sec_harddata_grid_->get_group( secondary_variable_group_name );
    if(secondary_variable_group_ == 0 || secondary_variable_group_->empty()) {
        errors->report("Secondary_Variable_Group", "The group "+secondary_variable_group_name+" is empty" );
        return false;
    }
    secondary_variable_ = secondary_variable_group_->get_property(0);
  }
  else {
    std::string secondary_variable_name =
      parameters->value( "Secondary_Variable.value" );
    errors->report( secondary_variable_name.empty(), 
                    "Secondary_Variable", "No property name specified" );

    secondary_variable_ = sec_harddata_grid_->property( secondary_variable_name );
  }


  //Assign the coordinate mapper
  if( dynamic_cast<Point_set*>(prim_harddata_grid_) ) {
    prim_harddata_grid_->set_coordinate_mapper(simul_grid_->coordinate_mapper());
  }
  if( sec_harddata_grid_ != simul_grid_ && dynamic_cast<Point_set*>(sec_harddata_grid_) ) {
    sec_harddata_grid_->set_coordinate_mapper(simul_grid_->coordinate_mapper());
  }

  bool assign_harddata = 
    String_Op::to_number<bool>( parameters->value( "Assign_Hard_Data.value" ) );
  if( prim_harddata_grid_ == NULL ) assign_harddata=false; 
  else if( prim_harddata_grid_ == simul_grid_ ) assign_harddata=true;


 // Set up the regions
  std::string region_name = parameters->value( "Grid_Name.region" );
  std::string prim_region_name = parameters->value( "Primary_Harddata_Grid.region" );
  std::string sec_region_name = parameters->value( "Secondary_Harddata_Grid.region" );

  target_grid_region_ = simul_grid_->region(region_name);
  prim_hd_grid_region_ = prim_harddata_grid_->region( prim_region_name );
  sec_hd_grid_region_ = sec_harddata_grid_->region( sec_region_name );

  /*
  if (!region_name.empty() && simul_grid_->region( region_name ) == NULL ) {
    errors->report("Grid_Name","Region "+region_name+" does not exist");
  }
  else grid_region_.set_temporary_region( region_name, simul_grid_);

  
  if( prim_harddata_grid_ )  prim_region = prim_harddata_grid_->region( prim_region_name );
  if(prim_harddata_grid_ && !assign_harddata && prim_harddata_grid_ != simul_grid_) {
    
    if (!prim_region_name.empty() && prim_harddata_grid_->region( prim_region_name ) == NULL ) {
      errors->report("Primary_Harddata_Grid","Region "+prim_region_name+" does not exist");
    }
    else  prim_hd_grid_region_.set_temporary_region( prim_region_name,prim_harddata_grid_ );
  }

  std::string sec_region_name = parameters->value( "Secondary_Harddata_Grid.region" );
  if( sec_harddata_grid_ )  sec_region = sec_harddata_grid_->region( sec_region_name );
  if(sec_harddata_grid_ && sec_harddata_grid_ != simul_grid_ 
     && sec_harddata_grid_ != prim_harddata_grid_) {
    
    if (!sec_region_name.empty() && sec_harddata_grid_->region( sec_region_name ) == NULL ) {
      errors->report("Secondary_Harddata_Grid","Region "+sec_region_name+" does not exist");
    }
    else  sec_hd_grid_region_.set_temporary_region( sec_region_name,sec_harddata_grid_ );
  }
  */
  //--------------
  // transform the hard data into standard gaussian variables if required

  transform_primary_variable_ = 
    String_Op::to_number<bool>( parameters->value( "Transform_Primary_Variable.value" ) );
  
  if( transform_primary_variable_ ) {
    bool ok = distribution_utils::get_continuous_cdf(original_cdf_,parameters, errors,"nonParamCdf_primary");
    if(!ok) {
      return false;
    }
	  if( primary_variable_ ) {
		  primary_variable_ = 
        distribution_utils::gaussian_transform_property( primary_variable_ , original_cdf_.raw_ptr(), prim_harddata_grid_, prim_hd_grid_region_ );
		  if( !primary_variable_ ) return false;

      clean_primary_var_ = true;
		  prim_harddata_grid_->select_property( primary_variable_ ->name() );
	  }
  }

 
  transform_secondary_variable_ = 
    String_Op::to_number<bool>( parameters->value( "Transform_Secondary_Variable.value" ) );

  if( transform_secondary_variable_ ) {
    if( !secondary_variable_ ) {
      errors->report( std::string( "Secondary_Variable" ), 
                      "Normal score transform requested, "
                      "but no property was selected" );
      return false;
    }

    //SmartPtr<Continuous_distribution> sec_distribution;
    bool ok = distribution_utils::get_continuous_cdf(sec_distribution_, parameters, errors, "nonParamCdf_secondary");
    if(!ok) {
      return false;
    }
    if( secondary_variable_group_ == 0 ) {
      secondary_variable_ = 
        distribution_utils::gaussian_transform_property( secondary_variable_, sec_distribution_.raw_ptr(), sec_harddata_grid_, sec_hd_grid_region_ );
      if( !secondary_variable_ ) return false;
      clean_secondary_var_ = true;
    }

  }
 
 
  //-------------
  // Assign primary variable to the simulation grid ?


  if( assign_harddata  ) {
    property_copier_ = 
      Property_copier_factory::get_copier( prim_harddata_grid_, simul_grid_ );
    if( !property_copier_ ) {
      std::ostringstream message;
      message << "It is currently not possible to copy a property from a "
              << prim_harddata_grid_->classname() << " to a " 
              << simul_grid_->classname() ;
      errors->report( !property_copier_, "Assign_Hard_Data", message.str() );
      return false;
    }
  }

  //-------------
  // Number of realizations and random number seed
  nb_of_realizations_ = 
    String_Op::to_number<int>( parameters->value( "Nb_Realizations.value" ) );
  
  seed_ = String_Op::to_number<int>( parameters->value( "Seed.value" ) );



  //-------------
  // Primary variable variogram (covariance), C11(h), initialization 

  Covariance<Location> C11;
  bool init_cov_ok =
    geostat_utils::initialize_covariance( &C11, "Variogram_C11",
                        			            parameters, errors );
  if( !init_cov_ok ) return false;

  
  //-------------
  // The primary variable search neighborhood parameters
  
  int max_neigh_1 = 
    String_Op::to_number<int>(parameters->value( "Max_Conditioning_Data_1.value" ));
  
  GsTLTriplet ranges_1;
  GsTLTriplet angles_1;
  bool extract_ok = 
    geostat_utils::extract_ellipsoid_definition( ranges_1, angles_1, 
                                    					   "Search_Ellipsoid_1.value",
				      	                                  parameters, errors );
  if( !extract_ok ) return false;
  
  extract_ok = geostat_utils::is_valid_range_triplet( ranges_1 );
  errors->report( !extract_ok,
                  "Search_Ellipsoid_1",
                  "Ranges must verify: major range >= " 
                  "medium range >= minor range >= 0" );
  if( !extract_ok ) return false;


  if( !prim_harddata_grid_ || assign_harddata ) {
    SmartPtr<Neighborhood> prim_neighborhood = 
      SmartPtr<Neighborhood>( simul_grid_->neighborhood( ranges_1, angles_1, &C11 ) );
    neighborhood_vector_.push_back( NeighborhoodHandle(prim_neighborhood) );
  }
  else {

    Neighborhood* simul_neigh = 
      simul_grid_->neighborhood( ranges_1, angles_1, &C11 );
    simul_neigh->max_size( max_neigh_1 );


    Neighborhood* harddata_neigh;
    if( dynamic_cast<Point_set*>(prim_harddata_grid_) ) {
      harddata_neigh = 
        prim_harddata_grid_->neighborhood( ranges_1, angles_1, &C11, true, prim_hd_grid_region_);
    } 
    else {
      harddata_neigh = 
        prim_harddata_grid_->neighborhood( ranges_1, angles_1, &C11, false, prim_hd_grid_region_ );
    }


    harddata_neigh->max_size( 3*max_neigh_1 /4 );
    harddata_neigh->select_property( primary_variable_->name() );

    SmartPtr<Neighborhood> prim_neighborhood = 
      SmartPtr<Neighborhood>( new Combined_neighborhood( harddata_neigh,
							 simul_neigh, &C11 ) );
    prim_neighborhood->max_size( max_neigh_1 );

    neighborhood_vector_.push_back( NeighborhoodHandle(prim_neighborhood) );
  }
  
  neighborhood_vector_[0].max_size( max_neigh_1 ); 
  geostat_utils::set_advanced_search(neighborhood_vector_[0], 
       "AdvancedSearch_1", parameters, errors);


  //-------------
  // depending on the cokriging type, initialize the secondary
  // variable neighborhood and the covariance set

  geostat_utils::Cokriging_type cok_type = 
    geostat_utils::cokriging_type( "Cokriging_Type.value", parameters, errors );

  geostat_utils::CokrigTagMap cokrig_tagmap;
  cokrig_tagmap[ geostat_utils::FULL ] = "Variogram_C12 Variogram_C22";
  cokrig_tagmap[ geostat_utils::MM1 ] = "Correl_Z1Z2.value Var_Z2.value";
  cokrig_tagmap[ geostat_utils::MM2 ] = "MM2_Correl_Z1Z2.value MM2_Variogram_C22";

  geostat_utils::CokrigDefaultsMap defaults;
  defaults[ geostat_utils::MM1 ] = "1.0";
  covar_ = 
    geostat_utils::init_covariance_set( cok_type, C11, cokrig_tagmap,
                                        parameters, errors, defaults );
  if( !covar_ ) return false;

  NeighborhoodHandle sec_neighborhood = 
    geostat_utils::init_secondary_neighborhood( cok_type, sec_harddata_grid_, 
                                                secondary_variable_,
                                                parameters, errors, 
                                                "Max_Conditioning_Data_2.value",
                                                "Search_Ellipsoid_2.value",
                                                "Variogram_C22", 
                                                sec_hd_grid_region_ );
  
  neighborhood_vector_.push_back( sec_neighborhood );
  geostat_utils::set_advanced_search(neighborhood_vector_[1], 
       "AdvancedSearch_2", parameters, errors);
//  init_covset_and_neighborhood( C11, cok_type, parameters, errors );
  

  //-----------------
  // The kriging constraints and combiner
  
  geostat_utils::Kriging_type ktype = 
    geostat_utils::kriging_type( "Kriging_Type.value", parameters, errors );

  
  //-----------------
  // parameter SK_Means will only be used if ktype is SK
  std::string skmean = "0.0";
  geostat_utils::initialize( combiner_, kconstraints_,
			                       ktype, "SK_Means.value",
			                       parameters, errors,
                             skmean );


  //-----------------
  // Additional checks

  if( cok_type == geostat_utils::MM1 || 
      cok_type == geostat_utils::MM2 ) {
    errors->report( sec_harddata_grid_ != simul_grid_,
                    "Secondary_Harddata_Grid", 
                    "For MM1 and MM2 options, the secondary variable must "
                    "be defined on the simulation grid" );

    errors->report( ktype == geostat_utils::OK,
                    "Kriging_Type", 
                    "Colocated cokriging (eg MM1 or MM2) is not compatible with "
                    "ordinary kriging: it amounts to ignoring the secondary "
                    "variable" );
  }



  if( !errors->empty() )
    return false;

  this->extract_parameters(parameters);
  return true;

}


Cosgsim::Cosgsim() {
  simul_grid_ = 0;
  multireal_property_ = 0; 
   
  prim_harddata_grid_ = 0;
  sec_harddata_grid_ = 0;
  primary_variable_ = 0;
  secondary_variable_ = 0;
  secondary_variable_group_ = 0;
  
  covar_ = 0; 
  combiner_ = 0; 
  kconstraints_ = 0; 

  variance_record_neighborhood_ = 0;

  clean_primary_var_ = false;
  clean_secondary_var_= false;
}



Cosgsim::~Cosgsim() {
  delete covar_;
  delete kconstraints_;
  delete combiner_;

  if( prim_harddata_grid_!=0 && dynamic_cast<Point_set*>(prim_harddata_grid_) ) {
      prim_harddata_grid_->set_coordinate_mapper(0);
  }

  if( sec_harddata_grid_!=simul_grid_ && dynamic_cast<Point_set*>(sec_harddata_grid_) ) {
      sec_harddata_grid_->set_coordinate_mapper(0);
  }

  clean();  
}



Named_interface* Cosgsim::create_new_interface( std::string& ) {
  return new Cosgsim;
}

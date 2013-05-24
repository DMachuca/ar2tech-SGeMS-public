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
** This file is part of the "grid" module of the Geostatistical Earth
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

#include <grid/rgrid_neighborhood.h>
#include <grid/rgrid.h>
#include <math/random_numbers.h>

#include <GsTL/math/math_functions.h>

#include <QIcon>

Named_interface* create_Rgrid( std::string& ) {
  return new RGrid;
}

//=======================================================


//TL modified
bool RGrid::reNameProperty(std::string oldName, std::string newName)
{
	return property_manager_.reNameProperty(oldName, newName);
}

RGrid::RGrid( )
  :  Strati_grid(),
     geom_(0),
     cell_connection_(0),
     connection_is_updated_(false), property_manager_(),
     accessor_(0),
     grid_cursor_(0){
	 region_manager_.set_parent_item(this);
	 property_manager_.set_parent_item(this);
}

RGrid::RGrid( std::string name )
:  Strati_grid(name),
   geom_(0),
   cell_connection_(0),
   connection_is_updated_(false), property_manager_(),
   accessor_(0),
   grid_cursor_(0){
	 region_manager_.set_parent_item(this);
	 property_manager_.set_parent_item(this);
	 group_manager_.set_parent_item(this);

}


RGrid::~RGrid() {
    delete geom_;
    //delete cell_connection_;
    delete accessor_;
    delete grid_cursor_;
}


void RGrid::set_geometry(RGrid_geometry* geom) {
  if( geom_ != geom ) {
    delete geom_;
    geom_ = geom->clone();
    connection_is_updated_ = false;
  }
  grid_cursor_ = new SGrid_cursor(nx(), ny(), nz(), 1);
  property_manager_.set_prop_size( geom->size() );
  region_manager_.set_region_size( geom->size() );
}


void RGrid::set_accessor(RGrid_gval_accessor* accessor) {
  if(accessor_ != accessor) {
    delete accessor_;
    accessor_ = accessor;
  }
}


Grid_continuous_property* RGrid::add_property( const std::string& name ) {
				
  return property_manager_.add_property( name );
}

Grid_continuous_property* RGrid::add_property_from_disk( const std::string& name, const std::string& filename ) {

  return property_manager_.add_property_from_disk( name, filename );
}


Grid_weight_property* RGrid::add_weight_property( const std::string& name ) {
				
  return property_manager_.add_weight_property( name );
}

Grid_weight_property* RGrid::add_weight_property_from_disk( const std::string& name, const std::string& filename ) {

  return property_manager_.add_weight_property_from_disk( name, filename );
}


Grid_categorical_property* RGrid::add_categorical_property(
		const std::string& name,
		const std::string& definition_name){
	return property_manager_.add_categorical_property( name, definition_name );
}


Grid_categorical_property* RGrid::add_categorical_property_from_disk(
		const std::string& name,
		const std::string& filename,
		const std::string& definition_name){
	return property_manager_.add_categorical_property_from_disk( name, filename, definition_name );
}

bool RGrid::remove_property( const std::string& name ) {
 // return property_manager_.remove_property( name );

  std::string name_group;

  Grid_continuous_property* prop = property(name );
  if(!prop) return false;
  std::vector< GsTLGridPropertyGroup*> groups = prop->groups();
  bool ok = property_manager_.remove_property( name);
  if(!ok) return false;
  for(int i=0; i<groups.size(); i++) {
    if(groups[i]->size() == 0) {
      remove_group(groups[i]->name());
    }
  }
  return true;

}

Grid_continuous_property* RGrid::select_property(const std::string& prop_name) {
  Grid_continuous_property* prop = property_manager_.select_property( prop_name );

  if (accessor_) delete accessor_;

  if( prop )
    accessor_ = new RGrid_gval_accessor(this, prop);
  else
    accessor_ = 0;

  return prop;
}



std::list<std::string> RGrid::property_list() const {

  std::list<std::string> result;

  Grid_property_manager::Property_name_iterator it = 
    property_manager_.names_begin();
  Grid_property_manager::Property_name_iterator end = 
    property_manager_.names_end();
  for( ; it != end ; ++it )
    result.push_back( *it );

  return result;
}

std::list<std::string> RGrid::categorical_property_list() const {
  std::list<std::string> result;

  Grid_property_manager::Property_name_iterator it =
    property_manager_.names_begin();
  Grid_property_manager::Property_name_iterator end =
    property_manager_.names_end();
  for( ; it != end ; ++it ) {
    const Grid_categorical_property* prop = categorical_property(*it);
    if(prop) result.push_back( *it );
  }

  return result;
}


std::list<std::string> RGrid::weight_property_list() const {
  std::list<std::string> result;

  Grid_property_manager::Property_name_iterator it =
    property_manager_.names_begin();
  Grid_property_manager::Property_name_iterator end =
    property_manager_.names_end();
  for( ; it != end ; ++it ) {
    const Grid_weight_property* prop = weight_property(*it);
    if(prop) result.push_back( *it );
  }

  return result;
}


MultiRealization_property* 
RGrid::add_multi_realization_property( const std::string& name ) {
  MultiRealization_property* mprops = property_manager_.new_multireal_property( name );
  GsTLGridPropertyGroup* group = this->add_group( mprops->name(), "Simulation" );
  if(group) {
    mprops->set_group(group);
  }
  return mprops;
}

 
std::list<std::string> RGrid::region_list() const {

  std::list<std::string> result;

  Grid_region_manager::Region_name_iterator it = 
    region_manager_.names_begin();
  Grid_region_manager::Region_name_iterator end = 
    region_manager_.names_end();
  for( ; it != end ; ++it )
    result.push_back( *it );

  return result;
}

Neighborhood* RGrid::neighborhood( double x, double y, double z,
				   double ang1, double ang2, double ang3,
				   const Covariance<location_type>* cov,
				   bool only_harddata, const Grid_region* region,
           Coordinate_mapper* coord_mapper) {

  // The constructor of Rgrid_ellips_neighborhood expects the dimensions
  // of the search ellipsoid to be in "number of cells", and the covariance
  // ranges to be expressed in term of "number of cells".

  int nx = GsTL::round( x /geom_->cell_dims()[0] );
  int ny = GsTL::round( y /geom_->cell_dims()[1] );
  int nz = GsTL::round( z /geom_->cell_dims()[2] );

  // The ranges of the covariance of a Neighborhood must be expressed
  // in "number of cells", while they are supplied to the rgrid in 
  // "actual" unit. So do the convertion. 
  Covariance<location_type>* cov_copy = 0;

  if( cov ) {
    cov_copy = new Covariance<location_type>( *cov );
    for( int i=0; i < cov_copy->structures_count() ; i++ ) {
      double R1,R2,R3;
      cov_copy->get_ranges( i, R1,R2,R3 );
      cov_copy->set_ranges( i,
		       R1/geom_->cell_dims()[0],
		       R2/geom_->cell_dims()[1],
		       R3/geom_->cell_dims()[2] );
    }
  }

  if( only_harddata )
    return new Rgrid_ellips_neighborhood_hd( this, 
					     property_manager_.selected_property(),
					     nx,ny,nz, ang1,ang2,ang3,
					     nx*ny*nz, cov_copy, region );
  else
    return new Rgrid_ellips_neighborhood( this, 
					  property_manager_.selected_property(),
					  nx,ny,nz, ang1,ang2,ang3,
					  nx*ny*nz, cov_copy, region  );

  delete cov_copy;
}



Neighborhood* RGrid::neighborhood( const GsTLTripletTmpl<double>& dim,
				   const GsTLTripletTmpl<double>& angles,
				   const Covariance<location_type>* cov,
				   bool only_harddata, const Grid_region* region,
           Coordinate_mapper* coord_mapper) {
  int nx = GsTL::round( dim[0] /geom_->cell_dims()[0] );
  int ny = GsTL::round( dim[1] /geom_->cell_dims()[1] );
  int nz = GsTL::round( dim[2] /geom_->cell_dims()[2] );

  Covariance<location_type>* cov_copy = 0;

  if( cov ) {
    cov_copy = new Covariance<location_type>( *cov );
    for( int i=0; i < cov_copy->structures_count() ; i++ ) {
      double R1,R2,R3;
      cov_copy->get_ranges( i, R1,R2,R3 );
      cov_copy->set_ranges( i,
		       R1/geom_->cell_dims()[0],
		       R2/geom_->cell_dims()[1],
		       R3/geom_->cell_dims()[2] );
    }
  }

  if( only_harddata )
    return new Rgrid_ellips_neighborhood_hd( this, 
					     property_manager_.selected_property(),
					     nx,ny,nz,
					     angles[0], angles[1], angles[2],
					     nx*ny*nz, cov_copy, region );
  else
    return new Rgrid_ellips_neighborhood( this, 
					  property_manager_.selected_property(),
					  nx,ny,nz,
					  angles[0], angles[1], angles[2],
					  nx*ny*nz, cov_copy, region );
}

Window_neighborhood* RGrid::window_neighborhood( const Grid_template& templ) {
  return new Rgrid_window_neighborhood( templ, this,
					property_manager_.selected_property() );
}
  

void RGrid::init_random_path( bool from_scratch ) {
  if( int( grid_path_.size() ) !=  grid_cursor_->max_index() ) {
    grid_path_.resize( grid_cursor_->max_index() );
    for( int i=0; i < int( grid_path_.size() ); i++ ) 
      grid_path_[i] = i;
  }
  else {
    if( from_scratch ) {
      for( int i=0; i < int( grid_path_.size() ); i++ ) 
      grid_path_[i] = i;
    }
  }
  
  STL_generator gen;
  std::random_shuffle( grid_path_.begin(), grid_path_.end(), gen );

}


void RGrid::clear_selected_region_from_property(){
  Grid_property_manager::Property_name_iterator it = property_manager_.names_begin();
  for(; it != property_manager_.names_end(); ++it) {
    Grid_continuous_property* prop = property_manager_.get_property( *it );
    prop->set_region(NULL);
  }
}


/*
 *
 */




QString RGrid::item_type() const{
	return QString(this->classname().c_str());
}

GsTL_object_item *RGrid::child(int row){
	if(row == 0) {
		return &property_manager_;
	}
	else if(row < group_manager_.size() +1) {
		std::map<std::string, GsTLGridPropertyGroup*>::iterator  it = group_manager_.begin_group();
		for(int i=1; i<row; ++i, ++it){}
		return it->second;
	}
	else return &region_manager_;

}


const GsTL_object_item *RGrid::child(int row) const{
	if(row == 0) {
		return &property_manager_;
	}
	else if(row < group_manager_.size() +1) {
		std::map<std::string, GsTLGridPropertyGroup*>::const_iterator  it = group_manager_.begin_group();
		for(int i=1; i<row; ++i, ++it){}
		return it->second;
	}
	else return &region_manager_;

}


int RGrid::childCount() const{
	return group_manager_.size()+2;

}

int RGrid::columnCount() const{
	return 2;

}

QVariant RGrid::item_data(int column) const{
	if(column ==0)
		return QString(name_.c_str());
	if(column ==1)
		return this->item_type();

	return QVariant();

}


/*
QModelIndex	RGrid::mapToSource ( const QModelIndex & proxyIndex ) const{
	return proxyIndex;
}
QModelIndex	RGrid::mapFromSource ( const QModelIndex & sourceIndex ) const{
	return sourceIndex;
}
*/

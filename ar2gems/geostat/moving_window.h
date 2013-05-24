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



#ifndef __GSTLAPPLI_GEOSTAT_PLUGIN_MOVING_WINDOW_H__
#define __GSTLAPPLI_GEOSTAT_PLUGIN_MOVING_WINDOW_H__


#include <geostat/utilities.h>
#include <geostat/geostat_algo.h>
#include <GsTL/utils/smartptr.h>
#include <grid/point_set.h>
#include <grid/rgrid.h>
#include <grid/grid_categorical_property.h>
#include <grid/neighborhood.h>
#include <math/gstlpoint.h>
#include <GsTL/matrix_library/tnt/cmat.h>
#include "GsTL_filters.h"
#include "Filtersim_filters.h"

//typedef std::vector< std::vector< double > > vec_vec_double;



typedef std::pair< Grid_continuous_property*, Grid_continuous_property* > pair_propT;


class Geostat_grid;
class Point_set;

class  Moving_window : public Geostat_algo {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:

	Moving_window();
	~Moving_window();

	virtual bool initialize( const Parameters_handler* parameters,
			   Error_messages_handler* errors );

  /** Runs the algorithm. 
   * @return 0 if the run was successful
   */  

	virtual int execute(GsTL_project *);
  /** Tells the name of the algorithm
   */

	virtual std::string name() const { return "moving_window"; }  

  private:
  typedef Neighborhood Smart_Neighborhood;
  //typedef SmartPtr<Neighborhood> Smart_Neighborhood;

	Geostat_grid* grid_;
	GsTL_filter<Smart_Neighborhood>* filters_;
  Smart_Neighborhood* neigh_;
  
  int nCategory_;
  const CategoricalPropertyDefinition* catdef_;

  std::vector< Grid_continuous_property* > props_;

  Grid_template create_neigh_template( int nx, int ny, int nz );
};





#endif


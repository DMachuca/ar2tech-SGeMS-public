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

#ifndef __GSTLAPPLI_POINTSET_H__ 
#define __GSTLAPPLI_POINTSET_H__ 
 
 
 
#include <grid/common.h>
#include <grid/grid_model/gval_iterator.h> 
#include <math/gstlpoint.h> 
#include <grid/grid_model/geostat_grid.h> 
#include <grid/grid_model/grid_property_manager.h> 
#include <grid/grid_model/grid_region_manager.h>  
#include <grid/grid_model/grid_property_set.h> 
#include <string> 
#include <vector> 
 


class Grid_continuous_property;
class Grid_categorical_property;
class Point_set_neighborhood; 
 
 
 
class GRID_DECL Point_set : public Geostat_grid { 

 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  //typedef Gval_iterator<LinearMapIndex> iterator; 
  Point_set():Geostat_grid(){}
  Point_set(std::string name, int size);
   
  virtual ~Point_set(); 
   
  virtual void point_locations( const std::vector<location_type>& locations ); 
  const std::vector<location_type>& point_locations() const { return point_loc_;}
  const std::vector<location_type>& xyz_point_locations() const { return xyz_point_loc_;}

  virtual void set_coordinate_mapper(Coordinate_mapper* coord_mapper);
 
  virtual std::pair<Geostat_grid::location_type, Geostat_grid::location_type> bounding_box() const {return bbox_;}

  // Returns the most specific name of the current class 
  virtual std::string classname() const { return "Point_set"; } 

  //----------------------------
  // Properties management 
  virtual Grid_continuous_property* add_property( const std::string& name );

  virtual Grid_continuous_property* add_property_from_disk( const std::string& name,
													const std::string& filename );
 
 
  virtual Grid_weight_property* add_weight_property( const std::string& name );

  virtual Grid_weight_property* add_weight_property_from_disk( 
          const std::string& name,const std::string& filename );

  virtual Grid_categorical_property* add_categorical_property(
			  const std::string& name,
			  const std::string& definition_name = "Default");

  virtual Grid_categorical_property* add_categorical_property_from_disk(
			  const std::string& name,const std::string& filename,
			  const std::string& definition_name = "Default");

  virtual bool remove_property( const std::string& name ); 
  
 
  /** Selects one of the properties of the grid. That property becomes 
   * the property the grid operates on by default: iterators returned 
   * by begin(), end() will iterate on the default property. 
   */ 
  virtual Grid_continuous_property* select_property( const std::string& name ); 
  virtual const Grid_continuous_property* selected_property() const; 
  virtual Grid_continuous_property* selected_property(); 
 
  virtual const Grid_continuous_property* property( const std::string& name ) const; 
  virtual Grid_continuous_property* property( const std::string& name ); 
 
  virtual const Grid_categorical_property* categorical_property( const std::string& name ) const;
  virtual Grid_categorical_property* categorical_property( const std::string& name );

  virtual const Grid_weight_property* weight_property( const std::string& name ) const;
  virtual Grid_weight_property* weight_property( const std::string& name );

  virtual std::list<std::string> property_list() const ; 
  virtual std::list<std::string> categorical_property_list() const ; 
  virtual std::list<std::string> weight_property_list() const;
 
  virtual MultiRealization_property*  
    add_multi_realization_property( const std::string& name ); 
 

  //--------------------------- 
  // PropertyGroup management 
  virtual GsTLGridPropertyGroup* add_group( const std::string& name, const std::string& type  );   
  virtual std::list<std::string> get_group_names(const std::string& type = "") const;   
  virtual unsigned int group_size() const;   
  virtual GsTLGridPropertyGroup* get_group( const std::string& name );   
  virtual const GsTLGridPropertyGroup* get_group( const std::string& name ) const;  
  virtual void remove_group( const std::string& name );


 //--------------------------- 
  // Region management 
 
  virtual Grid_region* region(const std::string& region_name);
  virtual const Grid_region* region(const std::string& region_name) const;

  virtual Grid_region* add_region(const std::string& name); 
  virtual bool remove_region(const std::string& name); 

    /** Selects a region. After calling this function, any operation 
   * that can be restricted to a region will only operate on the 
   * selected region. For example, an iterator returned by begin() 
   * will only iterate on the selected region. 
   */ 
  virtual bool select_region(const std::string& region_name); 

  virtual void unselect_region();
  //virtual bool select_region(const std::vector<std::string>& region_names);

  virtual const Grid_region* selected_region() const; 
  virtual Grid_region* selected_region();

  virtual std::list<std::string> region_list() const;

  virtual bool is_inside_selected_region(int node_id) const;

  
  //-------------------------------- 
  // neighborhood definition 
  virtual Neighborhood* neighborhood( double x, double y, double z, 
                                      double ang1, double ang2, double ang3, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false, const Grid_region* region = 0,
              Coordinate_mapper* coord_mapper=0); 
 
  virtual Neighborhood* neighborhood( const GsTLTripletTmpl<double>& dim, 
                                      const GsTLTripletTmpl<double>& angles, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false, const Grid_region* region = 0,
              Coordinate_mapper* coord_mapper=0); 
 
 
   
 
  //---------------------------  
  // iterators 
 
  /** returns an iterator to the begining of the current region, 
   * iterating on the current selected property. 
   */ 
  virtual iterator begin( Grid_continuous_property* prop = 0 ); 
  virtual iterator end( Grid_continuous_property* prop = 0 ); 
  virtual const_iterator begin( const Grid_continuous_property* prop = 0 ) const; 
  virtual const_iterator end( const Grid_continuous_property* prop = 0 ) const ; 
 
  virtual void init_random_path( bool from_scratch = true );
  virtual random_path_iterator random_path_begin( Grid_continuous_property* prop = 0 );
  virtual random_path_iterator random_path_end( Grid_continuous_property* prop = 0 );
 
 
  /** Computes the location of a node, given its node_id. 
   */ 
  virtual location_type location( int node_id ) const; 
  virtual location_type xyz_location( int node_id ) const; 
 
  virtual GsTLInt node_id( GsTLInt index ) const { return index; } 
   
  virtual GsTLInt size() const {return xyz_point_loc_.size();} 
 
  Geovalue geovalue(int node_id); 
 
  virtual GsTLInt closest_node( const location_type& P ) const { 
    appli_warning( "NOT YET IMPLEMENTED"); 
    return -1; 
  } 
 
   
  //TL modified
  virtual bool reNameProperty(std::string, std::string);

// GsTL_object_item implementation
  virtual QString item_type() const;
  virtual GsTL_object_item *child(int row);
  virtual const GsTL_object_item *child(int row) const;
  virtual int childCount() const;
  virtual int columnCount() const;
  virtual QVariant item_data(int column) const;

  virtual GsTL_object_item* regions_item(){
	  return &region_manager_;
  }


protected:
  std::vector<location_type> xyz_point_loc_; 
  std::vector<location_type> point_loc_;
  Grid_property_manager point_prop_;

  Grid_region_manager region_manager_;
  Grid_property_group_manager group_manager_;
  
  std::vector<GsTLInt> grid_path_; 

  std::pair<location_type,location_type> bbox_;
 
 private: 
  Point_set( const Point_set& ); 
  Point_set& operator = ( const Point_set& ); 
};  
 
 
 
 
//========================= 
 
inline 
Geovalue Point_set:: geovalue(int node_id) 
{ 
      Geovalue temp(this,point_prop_.selected_property(),node_id);
      return temp; 
} 
 
inline 
Grid_continuous_property* Point_set::select_property( const std::string& name ) 
{ 
   Grid_continuous_property* prop = point_prop_.select_property( name );
   return prop; 
} 
 
 
inline const Grid_continuous_property* Point_set::selected_property() const { 
  return point_prop_.selected_property();
} 
 
inline Grid_continuous_property* Point_set::selected_property() { 
  return point_prop_.selected_property();
} 
 
 
inline 
Point_set::location_type Point_set::location( int node_id ) const { 
     appli_assert( node_id >= 0 && node_id <(signed int)( point_loc_.size()) ); 
     return point_loc_[node_id]; 
  } 
 
inline 
Point_set::location_type Point_set::xyz_location( int node_id ) const { 
     appli_assert( node_id >= 0 && node_id <(signed int)( xyz_point_loc_.size()) ); 
     return xyz_point_loc_[node_id]; 
  }  
 
 
inline 
const Grid_continuous_property* Point_set::property( const std::string& name ) const 
{ 
    return point_prop_.get_property( name );
} 
 
 
inline 
Grid_continuous_property* Point_set::property( const std::string& name )  
{ 
    return point_prop_.get_property( name );
} 
 
inline 
const Grid_weight_property* Point_set::weight_property( const std::string& name ) const 
{ 
    return point_prop_.get_weight_property( name );
} 
 
 
inline 
Grid_weight_property* Point_set::weight_property( const std::string& name )  
{ 
    return point_prop_.get_weight_property( name );
} 

inline
const Grid_categorical_property* Point_set::categorical_property( const std::string& name ) const{
	 return point_prop_.get_categorical_property( name );
}

inline
Grid_categorical_property* Point_set::categorical_property( const std::string& name ){
	return point_prop_.get_categorical_property( name );
}
 
inline 
Grid_region* Point_set::add_region(const std::string& name) { 
  return region_manager_.add_region(name); 
} 
 
inline 
bool Point_set::remove_region(const std::string& name) { 
  return region_manager_.remove_region(name); 
} 
 

inline 
Grid_region* Point_set::region(const std::string& name) { 
  return region_manager_.get_region(name);
} 

inline 
const Grid_region* Point_set::region(const std::string& name) const { 
  return region_manager_.get_region(name);
} 

inline const Grid_region* Point_set::selected_region() const { 
  return region_manager_.selected_region(); 
} 
 
inline Grid_region* Point_set::selected_region() { 
  return region_manager_.selected_region(); 
}

inline bool Point_set::select_region(const std::string& region_name) {
  return region_manager_.select_region( region_name ) != NULL;
} 

inline void Point_set::unselect_region() {
  region_manager_.unselect_region();
}

inline bool Point_set::is_inside_selected_region(int node_id) const {
  const Grid_region* region = region_manager_.selected_region();
  // When no region is selected, use the full grid
  if( !region ) return true;  
  return region->is_inside_region(node_id);
}

 
inline 
Point_set::iterator Point_set::begin( Grid_continuous_property* prop ){ 
  Grid_continuous_property* property = prop;
  if( !prop )
    property = point_prop_.selected_property();

  return iterator( this, property, 
		               0,xyz_point_loc_.size(), LinearMapIndex() ); 
} 
inline 
Point_set::iterator Point_set::end( Grid_continuous_property* prop ){ 
  Grid_continuous_property* property = prop;
  if( !prop )
    property = point_prop_.selected_property();

  return iterator( this, property, 
              		 xyz_point_loc_.size(), xyz_point_loc_.size(), LinearMapIndex() ); 
} 
 

inline 
Point_set::const_iterator Point_set::begin( const Grid_continuous_property* prop ) const { 
  const Grid_continuous_property* property = prop;
  if( !prop )
    property = point_prop_.selected_property();

  return const_iterator( this, property, 
		  0,xyz_point_loc_.size(), 
                   LinearMapIndex() ); 
} 
inline 
Point_set::const_iterator Point_set::end( const Grid_continuous_property* prop ) const { 
  const Grid_continuous_property* property = prop;
  if( !prop )
    property = point_prop_.selected_property();

  return const_iterator( this, property, 
	                    	 xyz_point_loc_.size(),xyz_point_loc_.size(), 
                         LinearMapIndex() ); 
} 


inline GsTLGridPropertyGroup* 
Point_set::add_group( const std::string& name, const std::string& type ) {
  return group_manager_.add_group(name,type);
}

inline std::list<std::string> 
Point_set::get_group_names(const std::string& type) const {
  return group_manager_.group_names(type);
}

inline unsigned int Point_set::group_size() const {
  return group_manager_.size();
}

inline GsTLGridPropertyGroup* 
Point_set::get_group( const std::string& name ){
  return group_manager_.get_group(name);
}
inline const GsTLGridPropertyGroup* 
Point_set::get_group( const std::string& name ) const{
  return group_manager_.get_group(name);
}

inline void
Point_set::remove_group( const std::string& name ) {
	group_manager_.remove_group(name);
}

#endif 

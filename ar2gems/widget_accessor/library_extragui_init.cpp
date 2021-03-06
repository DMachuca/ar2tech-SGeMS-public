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
** This file is part of the "extra" module of the Geostatistical Earth
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

#include <widget_accessor/library_extragui_init.h>
#include <utils/manager_repository.h>
#include <utils/gstl_messages.h>
#include <widget_accessor/qt_accessors.h>
#include <gui/QWidget_value_accessors/qtable_accessor.h>

#include <qtplugins/filechooser.h>
#include <qtplugins/selectors.h>
#include <qtplugins/weight_property_selector.h>
#include <qtplugins/categorical_selectors.h>
#include <qtplugins/variogram_input.h>
#include <qtplugins/kriging_type_selector.h>
#include <qtplugins/gstl_table.h>
#include <qtplugins/ellipsoid_input.h>
#include <qtplugins/non_param_cdf_input.h>
#include <qtplugins/neighborhood_filter_input.h>
#include <qtplugins/group_selectors.h>

int library_extragui_init::references_ = 0;


int library_extragui_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "extragui library already registered" << "\n";
    return 2;
  }
     
  GsTLlog << "\n\n registering extragui library" << "\n"; 
  
  //---------------------
  // Create the qt_accessors manager

  GsTLlog << "Getting QT_wrappers manager" << "\n";
  SmartPtr<Named_interface> ni_wrapper = 
    Root::instance()->interface( xmlGeostatParamUtils_manager );
  Manager* dir = dynamic_cast<Manager*>( ni_wrapper.raw_ptr() );

  if( !dir ) {
    GsTLlog << "Directory " << xmlGeostatParamUtils_manager << " does not exist" << "\n";
    return 1;
  }

  bind_qt_wrappers_factories( dir );


  GsTLlog << "Registration done" << "\n\n";

  Root::instance()->list_all( GsTLlog );
  return 0;
}



int library_extragui_init::release_lib() {
  references_--;

  return 0;
}



bool library_extragui_init::bind_qt_wrappers_factories(Manager* dir) {
  // GridSelector
  GridSelectorBasic selector;
  std::string classname( selector.metaObject()->className() );
  dir->factory(classname, GridSelector_accessor::create_new_interface);
  
  // PropertySelector
  PropertySelectorNoRegion prop_noregion;
  classname = std::string( prop_noregion.metaObject()->className() );
  dir->factory(classname, PropertySelectorNoRegion_accessor::create_new_interface);

  // PropertySelector
  PropertySelector prop;
  classname = std::string( prop.metaObject()->className() );
  dir->factory(classname, PropertySelector_accessor::create_new_interface);

  // SinglePropertySelector
  SinglePropertySelector single_prop_select;
  classname = std::string( single_prop_select.metaObject()->className() );
  dir->factory(classname, SinglePropertySelector_accessor::create_new_interface);

  // MultiPropertySelector
  MultiPropertySelector multi_prop_select;
  classname = std::string( multi_prop_select.metaObject()->className() );
  dir->factory(classname, MultiPropertySelector_accessor::create_new_interface);

  // MultiPropertySelector
  OrderedPropertySelector order_prop_select;
  classname = std::string( order_prop_select.metaObject()->className() );
  dir->factory(classname, OrderedPropertySelector_accessor::create_new_interface);

//MultiRegionSelector_accessor
  MultiRegionSelector multi_region_select;
  classname = std::string( multi_region_select.metaObject()->className() );
  dir->factory(classname, MultiRegionSelector_accessor::create_new_interface);

  // FileChooser
  FileChooser chooser;
  classname = std::string( chooser.metaObject()->className() );
  dir->factory(classname, FileChooser_accessor::create_new_interface);

  // VariogramInput;
  VariogramInput varg_input;
  classname = std::string( varg_input.metaObject()->className() );
  dir->factory( classname, VariogramInput_accessor::create_new_interface);

  // KrigingTypeSelector
  KrigingTypeSelector ktype_selector;
  classname = std::string( ktype_selector.metaObject()->className() );
  dir->factory( classname, KrigingTypeSelector_accessor::create_new_interface);

  // GsTLTable
  GsTLTable gstl_table;
  classname = std::string( gstl_table.metaObject()->className() );
  dir->factory( classname, QTable_accessor::create_new_interface);

  // KrigingTypeSelector
  TrendComponents trend_components;
  classname = std::string( trend_components.metaObject()->className() );
  dir->factory( classname, TrendComponents_accessor::create_new_interface);

  // GsTLTable
  GsTLTable gstltable;
  classname = gstltable.metaObject()->className();
  dir->factory(classname, QTable_accessor::create_new_interface);

  // GsTLTable
  EllipsoidInput ellipsoid_input;
  classname = ellipsoid_input.metaObject()->className();
  dir->factory(classname, EllipsoidInput_accessor::create_new_interface);

  // LowerTailCdfInput
  LowerTailCdfInput lowerTailCdf_input;
  classname = lowerTailCdf_input.metaObject()->className();
  dir->factory(classname, lowerTailCdfInput_accessor::create_new_interface);

  // UpperTailCdfInput
  UpperTailCdfInput upperTailCdf_input;
  classname = upperTailCdf_input.metaObject()->className();
  dir->factory(classname, upperTailCdfInput_accessor::create_new_interface);

  // NonParamCdfInput
  NonParamCdfInput nonParamCdf_input;
  classname = nonParamCdf_input.metaObject()->className();
  dir->factory(classname, nonParamCdfInput_accessor::create_new_interface);

  // OctantSearchInput
  OctantSearchInput octant_input;
  classname = octant_input.metaObject()->className();
  dir->factory(classname, OctantSearch_accessor::create_new_interface);

  // AdvancedNeighborhoodInput
  AdvancedNeighborhoodInput advanced_search_input;
  classname = advanced_search_input.metaObject()->className();
  dir->factory(classname, AdvancedSearch_accessor::create_new_interface);

  // GridRegionSelector
  GridSelector grid_region_selector;
  classname = grid_region_selector.metaObject()->className();
  dir->factory(classname, GridRegionSelector_accessor::create_new_interface);

  // PropertyRegionSelector
  PropertySelector prop_region_selector;
  classname = prop_region_selector.metaObject()->className();
  dir->factory(classname, PropertyRegionSelector_accessor::create_new_interface);


/*  --  Categorical selector 
*/

  // MultiPropertySelector
  OrderedCategoricalPropertySelector cat_order_prop_select;
  classname = std::string( cat_order_prop_select.metaObject()->className() );
  dir->factory(classname, OrderedCategoricalPropertySelector_accessor::create_new_interface);

  // SingleWeightPropertySelector
  SingleWeightPropertySelector weight_prop_select;
  classname = std::string( weight_prop_select.metaObject()->className() );
  dir->factory(classname, SingleWeightPropertySelector_accessor::create_new_interface);


  // SinglePropertyGroupSelector
  SinglePropertyGroupSelector single_prop_group_select;
  classname = std::string( single_prop_group_select.metaObject()->className() );
  dir->factory(classname, SinglePropertyGroupSelector_accessor::create_new_interface);

  // MultiPropertySelector
  MultiPropertyGroupSelector multi_prop_group_select;
  classname = std::string( multi_prop_group_select.metaObject()->className() );
  dir->factory(classname, MultiPropertyGroupSelector_accessor::create_new_interface);

  return true;

}


extern "C" {
  int libGsTLAppli_extragui_init() {
    return library_extragui_init::init_lib();
  }
  int libGsTLAppli_extragui_release(){
    return library_extragui_init::release_lib();
  }
}

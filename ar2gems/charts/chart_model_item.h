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



#ifndef CHART_MODEL_ITEM_H
#define CHART_MODEL_ITEM_H

#include <utils/manager.h>
#include <appli/root_model.h>
#include <charts/common.h>
#include <grid/geostat_grid.h>
#include <grid/grid_property.h>
#include <grid/grid_weight_property.h>
#include <grid/grid_property_set.h>

#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QAbstractProxyModel>
#include <QWidget>
#include <QHash>
#include <QPair>

class CHARTS_DECL Chart_item {

public :
  Chart_item(){}
  virtual ~Chart_item(){}

  virtual int children_count() const =0;
//  virtual Chart_item* children(int row) =0;  
//  virtual Chart_item* parent() =0;  

  virtual QString type() const =0;

  virtual Geostat_grid* grid() const =0;

};


#endif 

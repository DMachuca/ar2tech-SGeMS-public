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



#ifndef __SCALE_PROPERTY_DIALOG_H__ 
#define __SCALE_PROPERTY_DIALOG_H__ 

#include <gui/common.h>

#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QtGui/qlineedit.h>

class GridSelectorBasic;
class SinglePropertySelector;
class MultipleCategorySelector;
class QGroupBox;
class QDoubleSpinBox;

class GsTL_project;



class Scale_property_dialog : public QDialog {
  Q_OBJECT

public:
  Scale_property_dialog( GsTL_project* proj,
                        QWidget* parent = 0, const char* name = 0 );

protected slots:

  bool execute();
  void execute_and_close();

protected:
  GsTL_project* project_;
  GridSelectorBasic* gridSelector_;
  SinglePropertySelector* propSelector_;


  QGroupBox* min_max_box_;
  QDoubleSpinBox *min_value_;
  QDoubleSpinBox *max_value_;
  QLineEdit* new_prop_name_;


};

 
#endif 

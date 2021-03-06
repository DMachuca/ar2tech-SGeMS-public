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
** This file is part of the "filters" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_FILTERS_CSV_QT_DIALOGS_H__ 
#define __GSTLAPPLI_FILTERS_CSV_QT_DIALOGS_H__ 

#include <filters/common.h>
#include <filters/ui_importwizard.h> // "qt-designer" generated file 
#include <filters/ui_csv_grid_import.h> // "qt-designer" generated file 
#include <filters/ui_csv_pointset_import.h> // "qt-designer" generated file 
#include <filters/ui_csv_masked_import.h> // "qt-designer" generated file 
 
#include <QTextEdit>
#include <qcombobox.h> 
#include <qstring.h> 
#include <qwidget.h> 
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>


#include <fstream> 
 
 
class Csv_specialized_infilter; 

 
/** Csv_infilter_dialog is the GUI part of the import filter. 
 * This wizard first determines which specialized filter to use (eg a grid 
 * import filter, or a pointset import filter), and then prompts for 
 * additional information, depending on the selected specialized filter. 
 */ 
class FILTERS_DECL Csv_infilter_dialog : public QDialog, public Ui::ImportWizard { 
   
  Q_OBJECT 
 
 public: 
  Csv_infilter_dialog( QWidget* parent , const char* name = 0 ); 
  virtual ~Csv_infilter_dialog(); 
 
  void set_file( std::ifstream& infile ); 
  Csv_specialized_infilter* filter() { return filter_; } 
 
 public slots: 
  void create_specialized_filter( const QString& ); 
 
 protected slots:
   virtual void accept();
   void nextClicked();
   void backClicked();

 protected: 
  Csv_specialized_infilter* filter_; 
  std::ifstream* file_stream_; 
}; 
 

class FILTERS_DECL Csv_input_mgrid_dialog : public QWidget, public Ui::CsvMaskedImport
{
	Q_OBJECT
public:
  Csv_input_mgrid_dialog( QWidget* parent , const char* name = 0 ); 
  Csv_input_mgrid_dialog( std::ifstream& infile, 
			       QWidget* parent , const char* name = 0 ); 
//	bool is_ijk() { return is_ijk_coords_->isChecked(); }
    bool is_xyz_file() { return is_xyz_locations_->isChecked(); }

	/// Cell dimensions 
	float x_size() const; 
	float y_size() const; 
	float z_size() const; 

	/// Coordinates of the origin 
	float Ox() const; 
	float Oy() const; 
	float Oz() const; 

	/// Number of pixels
	float nx() const; 
	float ny() const; 
	float nz() const; 

  float rotation_z() const;

	/// name of the grid 
	QString name() const; 

	QString X_column_name() const; 
	QString Y_column_name() const; 
	QString Z_column_name() const; 
  QString mask_column_name() const;


	int X_column_index() const; 
	int Y_column_index() const; 
	int Z_column_index() const; 
  int mask_column_index() const;

	// no data value
	bool use_no_data_value() const;
	float no_data_value() const;
};

 
/** This class is the GUI part of the grid import filter. It prompts for 
 * information concerning the grid to import: 
 * what name should the imported grid have, what are its dimensions, etc.  
 */ 
class FILTERS_DECL Csv_input_grid_dialog : public QWidget, public Ui::CsvGridImport { 
 
  Q_OBJECT 
 
 public: 
  Csv_input_grid_dialog( QWidget* parent , const char* name = 0 ); 
 
  /// Grid dimensions 
  int nx() const; 
  int ny() const; 
  int nz() const; 
 
  /// Cell dimensions 
  float x_size() const; 
  float y_size() const; 
  float z_size() const; 
 
  /// Coordinates of the origin 
  float Ox() const; 
  float Oy() const; 
  float Oz() const; 

  float rotation_z() const;

   
  /// name of the grid 
  QString name() const; 

  // no data value
  bool use_no_data_value() const;
  float no_data_value() const;
}; 
 
 
 
 
/** This class is the GUI part of the pointset import filter. It prompts for 
 * information concerning the point-set to import: 
 * what name the imported pointset should have, etc.  
 */ 
class FILTERS_DECL Csv_input_pointset_dialog : public QWidget, public Ui::CsvPointsetImport { 
Q_OBJECT

 public: 
   
  Csv_input_pointset_dialog( QWidget* parent , const char* name = 0 ); 
  Csv_input_pointset_dialog( std::ifstream& infile, 
			       QWidget* parent , const char* name = 0 ); 
   
  QString name() const; 
	QString X_column_name() const; 
	QString Y_column_name() const; 
	QString Z_column_name() const; 

	int X_column_index() const; 
	int Y_column_index() const; 
	int Z_column_index() const; 


  // no data value
  bool use_no_data_value() const;
  float no_data_value() const;

}; 
 
 
#endif 

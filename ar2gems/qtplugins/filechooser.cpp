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

#include <qtplugins/filechooser.h>
#include <utils/manager_repository.h>
#include <appli/project.h>

#include <qlineedit.h>
#include <qpushbutton.h>
#include <QFileDialog>
#include <qlayout.h>
#include <QHBoxLayout>

FileChooser::FileChooser( QWidget *parent, const char *name )
    : QWidget( parent ), md( File )
{
    if (name)
      setObjectName(name);

    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setMargin( 0 );
    layout->setSpacing( 4 ); 

    lineEdit = new QLineEdit( this);
    layout->addWidget( lineEdit );

    connect( lineEdit, SIGNAL( textChanged( const QString & ) ),
	     this, SIGNAL( fileNameChanged( const QString & ) ) );

    button = new QPushButton( "...", this);
    button->setFixedWidth( button->fontMetrics().width( "  ...  " ) );
    layout->addWidget( button );

    connect( button, SIGNAL( clicked() ),
	     this, SLOT( chooseFile() ) );

    setFocusProxy( lineEdit );
}

void FileChooser::setMode( Mode m )
{
    md = m;
}

FileChooser::Mode FileChooser::mode() const
{
    return md;
}

void FileChooser::setFileName( const QString &fn )
{
    lineEdit->setText( fn );
}

QString FileChooser::fileName() const
{
    return lineEdit->text();
}

void FileChooser::chooseFile()
{
  QString fn;
  QString path;
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( projects_manager + "/" + "project" );
  GsTL_project* project = dynamic_cast<GsTL_project*>( ni.raw_ptr() );

  switch( mode() ) {
  case File:
  	if ( lineEdit->text().isEmpty() && project)  {
  		path = QString(project->last_input_path().c_str() );
  	}
  	else {
  		path = lineEdit->text();
  	}
    /*
    QFileDialog dialog(this,"", path, QString::null);
    dialog.setFileMode(QFileDialog::AnyFile);
     QStringList fileNames;
     if (dialog.exec())
         fileNames = dialog.selectedFiles();

     if( !fileNames.isEmpty() )
       fn = fileNames.at(0);
  */ 
    
    fn = QFileDialog::getOpenFileName( this, "", path, QString::null);
    if(!fn.isEmpty())  project->last_input_path(path.toStdString());
    break;
  case Any:
  	if ( lineEdit->text().isEmpty() && project)  {
  		path = QString(project->last_output_path().c_str() );
  	}
  	else {
  		path = lineEdit->text();
  	}
    fn = QFileDialog::getSaveFileName( this, "", lineEdit->text(), QString::null );
    if(!fn.isEmpty())  project->last_output_path(path.toStdString());
    break;
  case Directory:
    fn = QFileDialog::getExistingDirectory( this, lineEdit->text() );
    break;
  }

  if ( !fn.isEmpty() ) {
	  lineEdit->setText( fn );
	  emit fileNameChanged( fn );
  }
}


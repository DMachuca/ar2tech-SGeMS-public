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
** Author: Ting Li
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "actions" module of the Geostatistical Earth
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

//$Log: lineeditkey.h,v $
//Revision 1.1.1.1  2007/10/30 18:14:22  lting2001
//sgems based on qt4
//
//Revision 1.1.1.1  2007-09-06 16:51:42  lting200
//
//
//Revision 1.1.1.1  2007-07-24 11:05:41  lting200
//qt4 port of sgems
//
//
//Revision 1.1.1.1  2007-07-24 11:03:38  lting200
//qt4 port of sgems
//
//Revision 3.0  2007/06/25 01:12:21  nico97492
//NEW RELEASE
//
//Revision 2.3  2007/06/04 06:15:27  nico97492
//Fixes to compile on Ubuntu 7 - g++ 4.1.2
//
//Revision 2.2  2007/04/25 17:19:35  lting2001
//merged with branch testing_030707
//
//Revision 2.1.2.1  2007/04/09 21:10:14  lting2001
//*** empty log message ***
//
//Revision 2.1  2007/02/27 19:36:36  lting2001
//*** empty log message ***
//
//Revision 1.1  2006/12/07 01:15:52  lting2001
//*** empty log message ***
//

#ifndef LINEEDITKEY_H
#define LINEEDITKEY_H

#include <utils/manager.h>
#include <utils/manager_repository.h>
#include <utils/gstl_messages_private.h>

#include <qevent.h>
#include <qlineedit.h>
#include <qnamespace.h>  // for key defs
//Added by qt3to4:
#include <QKeyEvent>
#include <vector>

// We want to capture certain key combos in the line edit
class LineEditKey : public QLineEdit
{
public:
	LineEditKey(QWidget * parent, const char * name = 0) : QLineEdit(parent) {
     	        if (name)
		  setObjectName(name);
		_v = NULL;
		_curCommand = "";
		_needSave = true;
	}
	void setHistory(const std::vector<std::string> * p) { _v = p; }
	void reset() {  _itr = _v->end(); _needSave = true;}

protected:
	void keyPressEvent ( QKeyEvent * e ) {
		QString command;

		// auto finish
		if (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_Tab) {
			std::vector<QString> match;
			Manager* mng;
			SmartPtr<Named_interface> ni;
			std::vector<std::string>::iterator itr;

			ni = Root::instance()->interface( actions_manager );
			mng = dynamic_cast<Manager*>( ni.raw_ptr() );
			if( !mng ) {
				appli_message("Cannot cast to manager type");
				return;
			}

			command = text();
			QByteArray tmp = command.toLatin1();
			std::string sCommand = tmp.constData();
			sCommand = mng->finish(sCommand);
			command = sCommand.c_str();
			
			for (Manager::type_iterator titr = mng->begin(); titr != mng->end(); ++titr) {
				QString temp = (*titr).c_str();
				if (temp.startsWith(command, Qt::CaseInsensitive))
					match.push_back(temp);
			}

			if (match.size() == 1) {
				setText(match[0]);
			}
			else if (match.size() == 0) {
				GsTLcout << "***No match found***" << gstlIO::end;
			}
			else {
				int count = 1;
				setText(command);
				GsTLcout << "Possible completions:" << gstlIO::end;				
				for (std::vector<QString>::iterator itr = match.begin();
				     itr != match.end(); ++itr,++count) {
				        QByteArray tmp = itr->toLatin1();
					GsTLcout << "**" << count << " - " << tmp.constData() << gstlIO::end;
				}
			}
			return;
		}
		
		switch (e->key()) {
		case Qt::Key_Up :  // shows command history
			if (_v->empty())
				break;
			if (_needSave) {
				_curCommand = text();
				_needSave = false;
			}
			if (_itr != _v->begin()) {
				--_itr;
				command = (*_itr).c_str();
				setText(command);
			}
			break;
		case Qt::Key_Down :
			if (_itr == _v->end())
				break;
			else if (_itr == (_v->end()-1)) {
				command = _curCommand;
				++_itr;
			}
			else {
				++_itr;
				command = (*_itr).c_str();
			}
			setText(command);
			break;
		case Qt::Key_Tab:
			break;
		default :
			QLineEdit::keyPressEvent(e); 
			break;
		}
	}

	const std::vector<std::string> * _v;
	QString _curCommand;
	std::vector<std::string>::const_iterator _itr;
	bool _needSave;
};

#endif

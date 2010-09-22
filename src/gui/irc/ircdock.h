//------------------------------------------------------------------------------
// ircdock.h
//------------------------------------------------------------------------------
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//
//------------------------------------------------------------------------------
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __IRCDOCK_H_
#define __IRCDOCK_H_

#include "ui_ircdock.h"
#include <QDockWidget>

class IRCAdapterBase;
class IRCDockTabContents;

/**
 *	@brief Dockable widget designed for IRC communication.
 */
class IRCDock : public QDockWidget, private Ui::IRCDock
{
	Q_OBJECT;

	public:
		IRCDock(QWidget* parent = NULL);

		void				addIRCAdapter(IRCAdapterBase* pIRCAdapter);
		
	protected:
		QAction*			toolBarConnect;

		void				setupToolbar();

	protected slots:
		void				chatWindowCloseRequestSlot(IRCDockTabContents* pCaller);
		void				tabCloseRequestedSlot(int index);
		void				titleChange(IRCDockTabContents* pCaller);
		void				toolBarAction(QAction* pAction);
		

};

#endif

//------------------------------------------------------------------------------
// ircdocktabcontents.h
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
#ifndef __IRCDOCK_TAB_CONTENTS_H_
#define __IRCDOCK_TAB_CONTENTS_H_

#include "irc/ircadapterbase.h"

#include "ui_ircdocktabcontents.h"
#include <QWidget>

class IRCChatAdapter;
class IRCDock;

/**
 *	@brief Dockable widget designed for IRC communication.
 */
class IRCDockTabContents : public QWidget, private Ui::IRCDockTabContents
{
	Q_OBJECT;

	public:
		IRCDockTabContents(IRCDock* pParentIRCDock);

		IRCAdapterBase*		ircAdapter() const { return pIrcAdapter; }
		
		/**
		 *	@brief Calling this multiple times on the same object will cause
		 *	memory leaks.
		 */
		void				setIRCAdapter(IRCAdapterBase* pAdapter);

	signals:
		/**
		 *	@brief Called when the variable returned by IRCAdapterBase::title()
		 *	might have changed and the application should be notified of this
		 *	fact.
		 */
		void				titleChange(IRCDockTabContents* pCaller);

	protected slots:
		void				adapterTitleChange()
		{
			emit titleChange(this);
		}

		void				nameAdded(const QString& nickname);
		void				nameListUpdated(const QStringList& names);
		void				nameRemoved(const QString& nickname);
		void				newChatWindowIsOpened(IRCChatAdapter* pAdapter);
		void				receiveError(const QString& error);
		void				receiveMessage(const QString& message);
		void				receiveMessageColored(const QString& message, const QString& htmlColor);
		void				sendMessage();

	protected:
		IRCAdapterBase*		pIrcAdapter;
		IRCDock*			pParentIRCDock;
};

#endif

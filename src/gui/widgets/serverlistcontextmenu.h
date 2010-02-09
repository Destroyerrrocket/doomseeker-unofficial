//------------------------------------------------------------------------------
// serverlistcontextmenu.h
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
#ifndef __SERVER_LIST_CONTEXT_MENU_H_
#define __SERVER_LIST_CONTEXT_MENU_H_

#include <QMenu>
#include <QObject>

class Server;

class ServerListContextMenu : public QObject
{
	public:
		enum Result
		{
			/// This is returned when something was copied to clipboard.
			DataCopied,
			Join,
			OpenRemoteConsole,
			OpenURL,
			Refresh,
			ShowJoinCommandLine,

			/// This is returned upon cancel.
			NothingHappened
		};

		ServerListContextMenu(Server* server);

		Result					exec(const QPoint& point);

	protected:
		QMenu*					createCopyMenu(QWidget* parent);
		void					createMembers();
		Result					translateQMenuResult(QAction* resultAction);
		void					initializeMembers();

		QAction*				copyAddress;
		QAction* 				copyEmail;
		QAction*				copyName;
		QAction* 				copyUrl;
		QAction* 				join;

		QMenu*					menu;

		QAction* 				openUrlInDefaultBrowser;

		Server*					pServer;

		QAction*				rcon;
		QAction* 				refresh;
		QAction* 				showJoinCommandLine;
};

#endif

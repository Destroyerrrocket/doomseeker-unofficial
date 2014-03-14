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
#include <QModelIndex>
#include <QObject>
#include "serverapi/serverptr.h"

class Server;
class ServerFilterBuilderMenu;
class ServerListFilterInfo;

class ServerListContextMenu : public QObject
{
	Q_OBJECT

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
			SortAdditionallyAscending,
			SortAdditionallyDescending,
			ClearAdditionalSorting,
			RemoveAdditionalSortingForColumn,

			/// This is returned upon cancel.
			NothingHappened
		};

		ServerListContextMenu(ServerPtr server, const ServerListFilterInfo& filter,
			const QModelIndex &modelIndex, QObject *parent);
		~ServerListContextMenu();

		const QModelIndex &modelIndex() const;
		void popup(const QPoint& point);
		ServerPtr server() const;
		const ServerListFilterInfo& serverFilter() const;
		Result translateQMenuResult(QAction* resultAction);

	signals:
		void aboutToHide();
		void triggered(QAction* action);

	private:
		class PrivData;

		PrivData* d;

		QMenu*					createCopyMenu(QWidget* parent);
		void					createMembers();
		void					initializeMembers();

		QAction*				copyAddress;
		QAction* 				copyEmail;
		QAction*				copyName;
		QAction* 				copyUrl;
		ServerFilterBuilderMenu* filterBuilder;
		QAction* 				join;

		QMenu*					menu;

		QAction* 				openUrlInDefaultBrowser;

		ServerPtr pServer;

		QAction*				rcon;
		QAction* 				refresh;
		QAction* 				showJoinCommandLine;
};

#endif

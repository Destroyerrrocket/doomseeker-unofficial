//------------------------------------------------------------------------------
// serverlistmodel.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __SERVER_LIST_MODEL_H_
#define __SERVER_LIST_MODEL_H_

#include "serverapi/serverptr.h"
#include <QStandardItemModel>

class Server;
class ServerListSortFilterProxyModel;
class ServerListHandler;

class ServerListModel : public QStandardItemModel
{
	Q_OBJECT

	friend class ServerListHandler;
	friend class ServerListProxyModel;

	public:
		/**
		 *	Servers from the same group will be always kept together
		 *  and sorted only inside this group. Group order is always descending:
		 *  SG_NORMAL servers will be always on the top of the list, after them
		 *  will be SG_WAIT servers, etc.
		 *
		 *  @b WARNING:
		 *	Exception: custom servers will always be on top of the list
		 *	and will be sorted inside their own group independentedly.
		 */
		enum ServerGroup
		{
			SG_NORMAL = 200,
			SG_WAIT = 175,
			SG_BANNED = 150,
			SG_TIMEOUT = 125,
			SG_BAD = 100,
			SG_FIRST_QUERY = 50,
		};

		enum ServerListDataTypes
		{
			// Pointer to the server structure is always stored in the first column
			SLDT_POINTER_TO_SERVER_STRUCTURE = Qt::UserRole+1,
			SLDT_SORT = Qt::UserRole+2
		};

		ServerListModel(ServerListHandler* parent);

		/**
		 * @return New row index.
		 */
		int addServer(ServerPtr server, int response);

		void destroyRows();

		/**
		 *	@brief Finds index of the row where server is contained.
		 *
		 *	@return -1 in case of a failure or index of the row otherwise.
		 */
		int findServerOnTheList(const Server* server);

		ServerListHandler* handler() { return parentHandler; }

		/**
		 *	Enforces update of a given row. No modificiation is done
		 *	to the server info itself. Can be used to redraw things like
		 *	background.
		 */
		void redraw(int row);
		void redrawAll();

		void removeCustomServers();
		void removeServer(const ServerPtr &server);

		/**
		 *  Updates flag on given row.
		 *  @param row - index of row to update
		 */
		void updateFlag(int row, bool force = true);

		/**
		 *	Returns row index.
		 */
		int updateServer(int row, ServerPtr server, int response);

		ServerPtr serverFromList(int rowIndex);
		ServerPtr serverFromList(const QModelIndex&);

		void setRefreshing(ServerPtr server);

	signals:
		void allRowsContentChanged();
		void modelCleared();
		void rowContentChanged(int row);

	protected:
		void prepareHeaders();
		ServerGroup serverGroup(int row);

		QVariant columnSortData(int row, int column);

		ServerListHandler* parentHandler;
};

#endif

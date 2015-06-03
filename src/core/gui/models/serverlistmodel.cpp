//------------------------------------------------------------------------------
// serverlistmodel.cpp
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
#include "gui/models/serverlistmodel.h"
#include "gui/models/serverlistcolumn.h"
#include "gui/models/serverlistrowhandler.h"
#include "gui/helpers/playersdiagram.h"
#include "gui/widgets/serverlistview.h"
#include "gui/serverlist.h"
#include "configuration/doomseekerconfig.h"
#include "serverapi/server.h"
#include "log.h"
#include <QItemDelegate>
#include <QTime>

using namespace ServerListColumnId;

//////////////////////////////////////////////////////////////

ServerListModel::ServerListModel(ServerListHandler* parent)
: QStandardItemModel(parent),
  parentHandler(parent)
{
	setSortRole(SLDT_SORT);
}

int ServerListModel::addServer(ServerPtr server, int response)
{
	QList<QStandardItem*> columns = ServerListColumns::generateListOfCells();
	appendRow(columns);

	// Country flag is set only once. Set it here and avoid setting it in
	// updateServer() method.

	QModelIndex index = indexFromItem(columns[0]);
	ServerListRowHandler rowHandler(this, index.row(), server);
	if (parentHandler->getMainWindow()->isActiveWindow())
	{
		rowHandler.setCountryFlag();
	}

	return rowHandler.updateServer(response);
}

void ServerListModel::destroyRows()
{
	int rows = rowCount();
	removeRows(0, rows);
	emit modelCleared();
}

int ServerListModel::findServerOnTheList(const Server* server)
{
	if (server != NULL)
	{
		for (int i = 0; i < rowCount(); ++i)
		{
			ServerCPtr savedServ = serverFromList(i);
			if (server == savedServ)
			{
				return i;
			}
		}
	}
	return -1;
}

void ServerListModel::redraw(int row)
{
	ServerPtr server = serverFromList(row);
	ServerListRowHandler rowHandler(this, row, server);
	rowHandler.redraw();
}

void ServerListModel::redrawAll()
{
	int slotstyle = gConfig.doomseeker.slotStyle;
	PlayersDiagram::loadImages(slotstyle);

	for (int i = 0; i < rowCount(); ++i)
	{
		redraw(i);
	}
}

void ServerListModel::removeCustomServers()
{
	QList<ServerPtr> serversToRemove;
	for (int i = 0; i < rowCount(); ++i)
	{
		ServerPtr server = serverFromList(i);
		if (server->isCustom())
		{
			serversToRemove.append(server);
		}
	}

	foreach (const ServerPtr &server, serversToRemove)
	{
		removeServer(server);
	}
}

void ServerListModel::removeServer(const ServerPtr &server)
{
	int index = findServerOnTheList(server.data());
	if (index >= 0)
	{
		removeRow(index);
	}
}

ServerPtr ServerListModel::serverFromList(int rowIndex)
{
	return ServerListRowHandler::serverFromList(this, rowIndex);
}

ServerPtr ServerListModel::serverFromList(const QModelIndex& index)
{
	return ServerListRowHandler::serverFromList(this, index.row());
}

ServerListModel::ServerGroup ServerListModel::serverGroup(int row)
{
	QStandardItem* qstdItem = item(row, IDHiddenGroup);
	return static_cast<ServerListModel::ServerGroup>(qstdItem->data(SLDT_SORT).toInt());
}

void ServerListModel::setRefreshing(ServerPtr server)
{
	int rowIndex = findServerOnTheList(server.data());
	if (rowIndex >= 0)
	{
		ServerListRowHandler rowHandler(this, rowIndex, server);
		rowHandler.setRefreshing();
	}
}

void ServerListModel::prepareHeaders()
{
	setHorizontalHeaderLabels(ServerListColumns::generateColumnHeaderLabels());
}

int ServerListModel::updateServer(int row, ServerPtr server, int response)
{
	ServerListRowHandler rowHandler(this, row, server);
	rowHandler.updateServer(response);

	return row;
}

QVariant ServerListModel::columnSortData(int row, int column)
{
	QStandardItem* it = item(row, column);
	return it->data(SLDT_SORT);
}

void ServerListModel::updateFlag(int row, bool force)
{
	ServerPtr server = serverFromList(row);
	ServerListRowHandler rowHandler(this, row, server);
	QStandardItem* itm = item(row, IDServerName);

	if (force || itm->icon().isNull())
	{
		rowHandler.setCountryFlag();
	}
}

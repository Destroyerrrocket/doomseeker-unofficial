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
#include "log.h"
#include "main.h"
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

int ServerListModel::addServer(Server* server, int response)
{
	QList<QStandardItem*> columns;
	ServerListColumns::generateListOfCells(columns);

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
			const Server* savedServ = serverFromList(i);
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
	Server* server = serverFromList(row);
	ServerListRowHandler rowHandler(this, row, server);
	rowHandler.redraw();
}

void ServerListModel::redrawAll()
{
	int slotstyle = *parentHandler->configurationObject()->setting("SlotStyle");
	PlayersDiagram::loadImages(slotstyle);

	for (int i = 0; i < rowCount(); ++i)
	{
		redraw(i);
	}
}

void ServerListModel::removeCustomServers()
{
	QList<Server*> serversToRemove;
	for (int i = 0; i < rowCount(); ++i)
	{
		Server* server = serverFromList(i);
		if (server->isCustom())
		{
			serversToRemove.append(server);
		}
	}

	QList<Server*>::iterator it;
	for (it = serversToRemove.begin(); it != serversToRemove.end(); ++it)
	{
		int index = findServerOnTheList(*it);
		if (index >= 0)
		{
			removeRow(index);
		}
	}
}

Server* ServerListModel::serverFromList(int rowIndex)
{
	ServerListRowHandler rowHandler(this, rowIndex);
    return rowHandler.getServer();
}

Server* ServerListModel::serverFromList(const QModelIndex& index)
{
	ServerListRowHandler rowHandler(this, index.row());
    return rowHandler.getServer();
}

ServerListModel::ServerGroup ServerListModel::serverGroup(int row)
{
	QStandardItem* qstdItem = item(row, IDHiddenGroup);
	return static_cast<ServerListModel::ServerGroup>(qstdItem->data(SLDT_SORT).toInt());
}

void ServerListModel::setRefreshing(Server* server)
{
	int rowIndex = findServerOnTheList(server);
	if (rowIndex >= 0)
	{
		ServerListRowHandler rowHandler(this, rowIndex, server);
		rowHandler.setRefreshing();
	}
}

void ServerListModel::prepareHeaders()
{
	QStringList labels;
	ServerListColumns::generateColumnHeaderLabels(labels);
	setHorizontalHeaderLabels(labels);
}

int ServerListModel::updateServer(int row, Server* server, int response)
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
    Server* server = serverFromList(row);
    ServerListRowHandler rowHandler(this, row, server);
    QStandardItem* itm = item(row, IDServerName);

    if (force || itm->icon().isNull())
    {
		rowHandler.setCountryFlag();
    }
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
bool ServerListSortFilterProxyModel::compareColumnSortData(QVariant& var1, QVariant& var2, int column) const
{
	if ( !(var1.isValid() && var2.isValid()) )
		return false;

	switch(column)
	{
		case IDAddress:
			return var1.toUInt() < var2.toUInt();

		case IDPing:
		case IDPlayers:
			return var1.toInt() < var2.toInt();

		case IDPort:
		case IDGametype:
		case IDIwad:
		case IDMap:
		case IDServerName:
		case IDWads:
			return var1.toString() < var2.toString();

		default:
			return false;
	}
}

bool ServerListSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	if (!parentHandler->getMainWindow()->isActiveWindow())
	{
		return false;
	}

	ServerListModel* model = static_cast<ServerListModel*>(sourceModel());

	Server* s1 = serverFromList(left);
	Server* s2 = serverFromList(right);

	if (s1 != NULL && s2 != NULL)
	{
		if (s1->isCustom() && !s2->isCustom())
		{
			return sortOrder == Qt::AscendingOrder;
		}
		else if (!s1->isCustom() && s2->isCustom())
		{
			return sortOrder == Qt::DescendingOrder;
		}
	}

	ServerListModel::ServerGroup sg1 = model->serverGroup(left.row());
	ServerListModel::ServerGroup sg2 = model->serverGroup(right.row());

	if (sg1 != sg2 && sg1 != 0 && sg2 != 0)
	{
		if (sg1 > sg2)
		{
			return sortOrder == Qt::AscendingOrder;
		}
		else
		{
			return sortOrder == Qt::DescendingOrder;
		}
	}
/*
	if (sg1 != sg2 && sg1 != 0 && sg2 != 0)
	{
		if (sortOrder == Qt::AscendingOrder)
		{
			if (sg1 > sg2)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (sg1 < sg2)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	*/

	QVariant leftVar = sourceModel()->data(left, sortRole());
	QVariant rightVar = sourceModel()->data(right, sortRole());

	return compareColumnSortData(leftVar, rightVar, left.column());
}

Server* ServerListSortFilterProxyModel::serverFromList(const QModelIndex& index) const
{
	ServerListModel* model = static_cast<ServerListModel*>(sourceModel());

    return model->serverFromList(index.row());
}

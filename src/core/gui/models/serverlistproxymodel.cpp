//------------------------------------------------------------------------------
// serverlistproxymodel.cpp
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
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "serverlistproxymodel.h"

#include "gui/entity/serverlistfilterinfo.h"
#include "gui/models/serverlistcolumn.h"
#include "gui/serverlist.h"
#include "serverapi/server.h"

class ServerListProxyModel::PrivData
{
	public:
		ServerListHandler* parentHandler;
		ServerListFilterInfo filterInfo;
		Qt::SortOrder sortOrder;
};

ServerListProxyModel::ServerListProxyModel(ServerListHandler* serverListHandler)
: QSortFilterProxyModel(serverListHandler)
{
	d = new PrivData();
	d->parentHandler = serverListHandler;
}

ServerListProxyModel::~ServerListProxyModel()
{
	delete d;
}

bool ServerListProxyModel::compareColumnSortData(QVariant& var1, QVariant& var2, int column) const
{
	using namespace ServerListColumnId;

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

bool ServerListProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	Server* s = serverFromList(sourceRow);
	if (s == NULL)
	{
		return false;
	}
	if (!d->filterInfo.bEnabled)
	{
		return true;
	}

	if (!s->isKnown())
	{
		if (d->filterInfo.bShowOnlyValid)
		{
			return false;
		}
	}
	else
	{
		// To make sure we perform an 'AND' query here all operations
		// must be negative. This means that if their test succeeds,
		// false must be returned.
		//
		// The ServerListFilterInfo copy constructor and operator= make
		// sure that all strings are trimmed.
		if (!d->filterInfo.bShowEmpty && s->isEmpty())
		{
			return false;
		}

		if (!d->filterInfo.bShowFull && s->isFull())
		{
			return false;
		}

		if (d->filterInfo.maxPing > 0 && d->filterInfo.maxPing < s->ping())
		{
			return false;
		}

		const QString& nameFilter = d->filterInfo.serverName;
		if (!nameFilter.isEmpty())
		{
			if (!s->name().contains(nameFilter, Qt::CaseInsensitive))
			{
				return false;
			}
		}

		if (!d->filterInfo.gameModes.isEmpty())
		{
			if (!d->filterInfo.gameModes.contains(s->gameMode().name(), Qt::CaseInsensitive))
			{
				return false;
			}
		}

		if (d->filterInfo.gameModesExcluded.contains(s->gameMode().name(), Qt::CaseInsensitive))
		{
			return false;
		}

		if (!d->filterInfo.wads.isEmpty())
		{
			bool bWadFound = false;

			// TODO
			// This may cause performance drops. Testing is required
			foreach (const QString& filteredWad, d->filterInfo.wads)
			{
				if (s->anyWadnameContains(filteredWad))
				{
					bWadFound = true;
					break;
				}
			}

			if (!bWadFound)
			{
				return false;
			}
		}

		if (!d->filterInfo.wadsExcluded.isEmpty())
		{
			bool bWadFound = false;
			foreach (const QString& filteredWad, d->filterInfo.wadsExcluded)
			{
				if (s->anyWadnameContains(filteredWad))
				{
					return false;
				}
			}
		}
	}

	return true;
}

const ServerListFilterInfo& ServerListProxyModel::filterInfo() const
{
	return d->filterInfo;
}

bool ServerListProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	if (!d->parentHandler->getMainWindow()->isActiveWindow())
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
			return d->sortOrder == Qt::AscendingOrder;
		}
		else if (!s1->isCustom() && s2->isCustom())
		{
			return d->sortOrder == Qt::DescendingOrder;
		}
	}

	ServerListModel::ServerGroup sg1 = model->serverGroup(left.row());
	ServerListModel::ServerGroup sg2 = model->serverGroup(right.row());

	if (sg1 != sg2 && sg1 != 0 && sg2 != 0)
	{
		if (sg1 > sg2)
		{
			return d->sortOrder == Qt::AscendingOrder;
		}
		else
		{
			return d->sortOrder == Qt::DescendingOrder;
		}
	}

	QVariant leftVar = sourceModel()->data(left, sortRole());
	QVariant rightVar = sourceModel()->data(right, sortRole());

	return compareColumnSortData(leftVar, rightVar, left.column());
}

void ServerListProxyModel::setFilterInfo(const ServerListFilterInfo& filterInfo)
{
	d->filterInfo = filterInfo;
	invalidate();
}

Server* ServerListProxyModel::serverFromList(const QModelIndex& index) const
{
	return serverFromList(index.row());
}

Server* ServerListProxyModel::serverFromList(int row) const
{
	ServerListModel* model = static_cast<ServerListModel*>(sourceModel());
	return model->serverFromList(row);
}

void ServerListProxyModel::sortServers(int column, Qt::SortOrder order)
{
	d->sortOrder = order;
	sort(column, order);
}


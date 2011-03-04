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

ServerListProxyModel::ServerListProxyModel(ServerListHandler* serverListHandler)
: QSortFilterProxyModel(serverListHandler)
{
	parentHandler = serverListHandler;

	pFilterInfo = new ServerListFilterInfo();
}

ServerListProxyModel::~ServerListProxyModel()
{
	delete pFilterInfo;
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
	
	if (!s->isKnown())
	{
		if (pFilterInfo->bShowOnlyValid)
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
		if (!pFilterInfo->bShowEmpty && s->isEmpty())
		{
			return false;
		}
		
		if (!pFilterInfo->bShowFull && s->isFull())
		{
			return false;
		}
		
		if (pFilterInfo->maxPing > 0 && pFilterInfo->maxPing < s->ping())
		{
			return false;
		}
		
		const QString& nameFilter = pFilterInfo->serverName;
		if (!nameFilter.isEmpty())
		{
			if (!s->name().contains(nameFilter, Qt::CaseInsensitive))
			{
				return false;
			}
		}
		
		if (!pFilterInfo->gameMode.isEmpty())
		{
			if (s->gameMode().name().compare(pFilterInfo->gameMode, Qt::CaseInsensitive) != 0)
			{
				return false;
			}
		}
		
		if (!pFilterInfo->wads.isEmpty())
		{
			bool bWadFound = false;
			
			// TODO
			// This may cause performance drops. Testing is required
			for (int i = 0; i < pFilterInfo->wads.count(); ++i)
			{	
				const QString& wad = pFilterInfo->wads[i];
			
				for (int j = 0; j < s->numWads(); ++j)
				{
					const PWad& pwad = s->wad(j);
					if (pwad.name.contains(wad, Qt::CaseInsensitive))
					{
						bWadFound = true;
						break;
					}
				}
				
				if (bWadFound)
				{
					// Exit loop, wad was found.
					break;
				}
			}
			
			if (!bWadFound)
			{
				return false;
			}
		}
	}
		
	return true;
}


bool ServerListProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
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

	QVariant leftVar = sourceModel()->data(left, sortRole());
	QVariant rightVar = sourceModel()->data(right, sortRole());

	return compareColumnSortData(leftVar, rightVar, left.column());
}

void ServerListProxyModel::setFilterInfo(const ServerListFilterInfo& filterInfo)
{
	*pFilterInfo = filterInfo;
	
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


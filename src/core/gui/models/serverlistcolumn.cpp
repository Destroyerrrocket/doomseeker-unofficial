//------------------------------------------------------------------------------
// serverlistcolumn.cpp
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
#include "serverlistcolumn.h"

using namespace ServerListColumnId;

#define HIDDEN true
#define RESIZEABLE true

ServerListColumn ServerListColumns::columns[] =
{
	{ IDPort, 					tr("Port"),			24,		!HIDDEN, !RESIZEABLE, Qt::AscendingOrder },
	{ IDPlayers, 				tr("Players"),		60,		!HIDDEN,  RESIZEABLE, Qt::DescendingOrder },
	{ IDPing, 					tr("Ping"),			50,		!HIDDEN,  RESIZEABLE, Qt::AscendingOrder },
	{ IDServerName, 			tr("Servername"),	200,	!HIDDEN,  RESIZEABLE, Qt::AscendingOrder },
	{ IDAddress, 				tr("Address"),		120,	!HIDDEN,  RESIZEABLE, Qt::AscendingOrder },
	{ IDIwad, 					tr("IWAD"),			90,		!HIDDEN,  RESIZEABLE, Qt::AscendingOrder },
	{ IDMap, 					tr("MAP"),			70,		!HIDDEN,  RESIZEABLE, Qt::AscendingOrder },
	{ IDWads, 					tr("Wads"),			120,	!HIDDEN,  RESIZEABLE, Qt::AscendingOrder },
	{ IDGametype, 				tr("Gametype"),		150,	!HIDDEN,  RESIZEABLE, Qt::AscendingOrder },
	{ IDHiddenGroup, 			"SORT_GROUP",		0,		 HIDDEN, !RESIZEABLE, Qt::DescendingOrder },
	{ IDHiddenServerPointer, 	"SERVER_POINTER",	0,		 HIDDEN, !RESIZEABLE, Qt::AscendingOrder }
};

void ServerListColumns::generateColumnHeaderLabels(QStringList& outputLabels)
{
	outputLabels.clear();
	for (int i = 0; i < NUM_SERVERLIST_COLUMNS; ++i)
	{
		outputLabels << columns[i].name;
	}
}

void ServerListColumns::generateListOfCells(QList<QStandardItem*>& outputList)
{
	outputList.clear();
	for (int x = 0; x < NUM_SERVERLIST_COLUMNS; ++x)
	{
		outputList.append(new QStandardItem());
	}
}

bool ServerListColumns::isColumnVital(int columnId)
{
	// We assume that columnIndex == columnId.
	return columns[columnId].bHidden || columnId == IDAddress || columnId == IDPort;
}

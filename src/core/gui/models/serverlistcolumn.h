//------------------------------------------------------------------------------
// serverlistcolumn.h
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
#ifndef __SERVER_LIST_COLUMN_H_
#define __SERVER_LIST_COLUMN_H_

#include <QList>
#include <QObject>
#include <QStandardItem>
#include <QString>

class ServerListColumn
{
	public:
		int columnId;
		int width;
		bool bHidden;
		bool bResizable;
		Qt::SortOrder defaultSortOrder;
};

namespace ServerListColumnId
{
	enum ColumnId
	{
		IDPort,
		IDPlayers,
		IDPing,
		IDServerName,
		IDAddress,
		IDIwad,
		IDMap,
		IDWads,
		IDGametype,
		IDHiddenGroup,
		IDHiddenServerPointer,

		NUM_SERVERLIST_COLUMNS
	};
}

class ServerListColumns : public QObject
{
	Q_OBJECT

	public:
		static ServerListColumn columns[];

		static QString columnLabel(int columnId);
		static void generateColumnHeaderLabels(QStringList& outputLabels);
		static void generateListOfCells(QList<QStandardItem*>& outputList);

		static bool isColumnVital(int columnId);

		ServerListColumns() {}
};

#endif

//------------------------------------------------------------------------------
// serverlistcolumn.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __SERVER_LIST_COLUMN_H_
#define __SERVER_LIST_COLUMN_H_

#include <QtContainerFwd>
#include <QObject>

class QStandardItem;

struct ServerListColumn
{
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
		static QStringList generateColumnHeaderLabels();
		static QList<QStandardItem*> generateListOfCells();

		static bool isColumnVital(int columnId);

		ServerListColumns() {}
};

#endif

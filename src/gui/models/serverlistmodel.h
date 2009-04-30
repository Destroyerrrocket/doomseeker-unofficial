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

#define HOW_MANY_SERVERLIST_COLUMNS 11

#include <QHostAddress>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QPixmap>
#include "server.h"

struct ServerListColumn
{
	QString		name;
	int			width;
	bool		bHidden;
	bool		bResizable;
};

class ServerListModel : public QStandardItemModel
{
	Q_OBJECT

	friend class SLHandler;

	public:
		enum ColumnId
		{
			SLCID_PORT = 0,
			SLCID_PLAYERS = 1,
			SLCID_PING = 2,
			SLCID_SERVERNAME = 3,
			SLCID_ADDRESS = 4,
			SLCID_IWAD = 5,
			SLCID_MAP = 6,
			SLCID_WADS = 7,
			SLCID_GAMETYPE = 8,
			SLCID_HIDDEN_GROUP = 9,
			SLCID_HIDDEN_SERVER_POINTER = 10
		};

		/**
		 *	Servers from the same group will be always kept together
		 *  and sorted only inside this group. Group order is always descending:
		 *  SG_CUSTOM servers will be always on the top of the list, after them
		 *  will be SG_NORMAL servers, etc.
		 */
		enum ServerGroup
		{
			SG_CUSTOM 	= 300,
			SG_NORMAL 	= 200,
			SG_WAIT 	= 175,
			SG_BANNED	= 150,
			SG_TIMEOUT	= 125,
			SG_BAD		= 100
		};

		enum ServerListDataTypes
		{
			// Pointer to the server structure is always stored in the first column
			SLDT_POINTER_TO_SERVER_STRUCTURE = Qt::UserRole+1,
			SLDT_SORT						 = Qt::UserRole+2
		};

		static ServerListColumn columns[];

		ServerListModel(QObject* parent = NULL);

		void destroyRows();

		/**
		 * Removes content from fields that aren't
		 * hidden and don't belong to SLCID_ADDRESS column.
		 */
		void clearNonVitalFields(int row);
		void emptyItem(QStandardItem*);
		void fillItem(QStandardItem*, const QString&);
		void fillItem(QStandardItem*, int);
		void fillItem(QStandardItem*, const QHostAddress&, const QString& actualDisplay = QString());
		void fillItem(QStandardItem*, const QString&, const QPixmap&);

		/**
		 *	Returns row number
		 */
		int addServer(Server* server, int response);

		/**
		 *	Returns row number
		 */
		int updateServer(int row, Server* server, int response);

		void setBad(int row, Server* server);
		void setBanned(int row, Server* server);
		void setGood(int row, Server* server);
		void setTimeout(int row, Server* server);
		void setWait(int row, Server* server);
		void setRefreshing(int row);

		QModelIndex findServerOnTheList(const Server* server);
		Server* serverFromList(int rowNum);
        Server* serverFromList(const QModelIndex&);
        Server* serverFromList(const QStandardItem*);

		void sort( int column, Qt::SortOrder order = Qt::AscendingOrder);

	signals:
		void allRowsContentChanged();
		void modelCleared();
		void rowContentChanged(int row);

	protected:
		void 				clearRows();
		void 				prepareHeaders();
		ServerGroup 		serverGroup(int row);

		/**
		 * Returns:
		 *		0 if rows are equal
		 *	   <0 if row1 is less than row2
		 *	   >0 if row1 is more than row2
		 */
		int					compareColumnSortData(QVariant& var1, QVariant& var2, int column);
		int					compareColumnSortData(int row1, int row2, int column);

		QVariant			columnSortData(int row, int column);
		void 				swapRows(unsigned int row1, unsigned int row2);
};

#endif

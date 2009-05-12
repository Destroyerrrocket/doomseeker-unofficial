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

#include <QHostAddress>
#include <QSortFilterProxyModel>
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

class ServerListSortFilterProxyModel;

class ServerListModel : public QStandardItemModel
{
	Q_OBJECT

	friend class SLHandler;
	friend class ServerListSortFilterProxyModel;

	public:
		enum ColumnId
		{
			SLCID_PORT,
			SLCID_PLAYERS,
			SLCID_PING,
			SLCID_SERVERNAME,
			SLCID_ADDRESS,
			SLCID_IWAD,
			SLCID_MAP,
			SLCID_WADS,
			SLCID_GAMETYPE,
			SLCID_HIDDEN_GROUP,
			SLCID_HIDDEN_SERVER_POINTER,

			HOW_MANY_SERVERLIST_COLUMNS
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
		 * hidden and don't belong to SLCID_ADDRESS and SLCID_PORT columns.
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

		QModelIndex findServerOnTheList(const Server* server);
		Server* serverFromList(int rowNum);
		Server* serverFromList(const QModelIndex&);
		Server* serverFromList(const QStandardItem*);

	signals:
		void allRowsContentChanged();
		void modelCleared();
		void rowContentChanged(int row);

	protected:
		void 					clearRows();
		void 					prepareHeaders();
		ServerGroup 			serverGroup(int row);

		void 					setBad(int row, Server* server);
		void 					setBanned(int row, Server* server);
		void 					setGood(int row, Server* server);
		void					setTimeout(int row, Server* server);
		void 					setWait(int row, Server* server);
		void 					setRefreshing(int row);

		QVariant				columnSortData(int row, int column);
};

class ServerListSortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	friend class SLHandler;

	public:
		ServerListSortFilterProxyModel(QObject* parent = 0) : QSortFilterProxyModel(parent) {}

		void	sortServers(int column, Qt::SortOrder order = Qt::AscendingOrder)
		{
			qDebug() << "Sort!";
			sortOrder = order;
			sort(column, order);
		}

	protected:
		Qt::SortOrder sortOrder;

		/**
		 * @return true if var1 is less than var2, false if otherwise.
		 */
		bool	compareColumnSortData(QVariant& var1, QVariant& var2, int column) const;
		bool	lessThan(const QModelIndex& left, const QModelIndex& right) const;
};

#endif

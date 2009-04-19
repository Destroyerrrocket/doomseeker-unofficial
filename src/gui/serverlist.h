//------------------------------------------------------------------------------
// serverlist.h
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

#ifndef __SERVERLIST_H_
#define __SERVERLIST_H_

#include "server.h"

#include <QObject>
#include <QString>
#include <QTableView>
#include <QStandardItem>
#include "masterclient.h"

#define HOW_MANY_SERVERLIST_COLUMNS 8
#define TAB_WIDTH					8

struct ServerListColumn
{
	QString		name;
	int			width;
};

class SLHandler : public QObject
{
	Q_OBJECT

	public:
		SLHandler(QTableView*);
		~SLHandler();

		void clearTable();

		static ServerListColumn columns[];

		enum ColumnId
		{
			SLCID_PLAYERS = 0,
			SLCID_PING = 1,
			SLCID_SERVERNAME = 2,
			SLCID_ADDRESS = 3,
			SLCID_IWAD = 4,
			SLCID_MAP = 5,
			SLCID_WADS = 6,
			SLCID_GAMETYPE = 7,
		};

		enum ServerListDataTypes
		{
			// Pointer to the server structure is always stored in the first column
			SLDT_POINTER_TO_SERVER_STRUCTURE = Qt::UserRole+1,
			SLDT_SORT						 = Qt::UserRole+2
		};

		void 			setMaster(MasterClient*);
		QList<Server*>*	serverList()
		{
			if (master == NULL)
			{
				return NULL;
			}

			return &master->serverList();
		}

	public slots:
		void serverUpdated(Server *server, int response);
		void refreshAll();
		void tableRightClicked(const QModelIndex&);

	protected slots:
		// Handles column sorting.
		void columnHeaderClicked(int);
		void mouseEntered(const QModelIndex&);
		void doubleClicked(const QModelIndex&);

	signals:
		void serverDoubleClicked(const Server*);

	protected:
		QTableView* 	table;
		MasterClient*	master;

		Qt::SortOrder 	sortOrder;
		int				sortIndex;

		void fillItem(QStandardItem*, const QString&);
		void fillItem(QStandardItem*, int);
		void fillItem(QStandardItem*, const QHostAddress&, const QString& actualDisplay = QString());

		void prepareServerTable();
		QModelIndex findServerOnTheList(const Server* server);

		void addServer(Server* server, int response);
		void updateServer(int row, Server* server, int response);

		void setBad(int row, Server* server);
		void setBanned(int row, Server* server);
		void setGood(int row, Server* server);
		void setTimeout(int row, Server* server);
		void setRefreshing(int row);


		QString createPlayersToolTip(const Server* server);
		QString createServerNameToolTip(const Server* server);
		QString createPwadsToolTip(const Server* server);

		Server* serverFromList(int rowNum);
        Server* serverFromList(const QModelIndex&);
        Server* serverFromList(const QStandardItem*);

		QString spawnGeneralInfoTable(const Server* server);
		QString spawnPlayerTable(const Server* server);
		/**
		 * This will return absolutely nothing if the list in the first argument is empty.
		 */
        QString spawnPartOfPlayerTable(QList<const Player*>, QString status, int colspan, bool isTeamgame, bool bAppendEmptyRowAtBeginning);
};

#endif

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
#include "gui/widgets/serverlistview.h"
#include "gui/models/serverlistmodel.h"

class SLHandler : public QObject
{
	Q_OBJECT



	public:
		SLHandler(ServerListView*);
		~SLHandler();

		void clearTable();



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
		ServerListView*	table;
		MasterClient*	master;

		Qt::SortOrder 	sortOrder;
		int				sortIndex;

		void prepareServerTable();

		QString createPlayersToolTip(const Server* server);
		QString createServerNameToolTip(const Server* server);
		QString createPwadsToolTip(const Server* server);


		QString spawnGeneralInfoTable(const Server* server);
		QString spawnPlayerTable(const Server* server);
		/**
		 * This will return absolutely nothing if the list in the first argument is empty.
		 */
        QString spawnPartOfPlayerTable(QList<const Player*>, QString status, int colspan, bool isTeamgame, bool bAppendEmptyRowAtBeginning);
};

#endif

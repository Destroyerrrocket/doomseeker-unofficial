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
#include <QStandardItem>
#include <QString>
#include <QTableView>
#include <QTimer>

#include "gui/widgets/serverlistview.h"
#include "gui/models/serverlistmodel.h"

class SLHandler : public QObject
{
	Q_OBJECT

	public:
		SLHandler(ServerListView*);
		~SLHandler();

		void 				clearTable();

		QList<Server*>		selectedServers();

		Server*				serverFromIndex(const QModelIndex&);

		ServerListModel*	serverModel() { return model; }
		ServerListView*		serverTable() { return table; }


	public slots:
		void cleanUp();
		void redraw();
		void refreshAll();
		void serverBegunRefreshing(Server* server);
		void serverUpdated(Server *server, int response);
		void tableRightClicked(const QModelIndex&, const QPoint&);
		void updateCountryFlags(bool onlyIfServerHasNoFlagYet = false);
		void updateSearch(const QString& search);

	protected slots:
		// Handles column sorting.
		void columnHeaderClicked(int);
		void doubleClicked(const QModelIndex&);
		void itemSelected(const QModelIndex&);
		void modelCleared();
		void mouseEntered(const QModelIndex&);
		void resizeChangedRows(const QModelIndex&, int, int);

	signals:
		void serverDoubleClicked(const Server*);
		void serversSelected(QList<Server*>&);

	protected:
		QTimer			cleaner;
		bool 			needsCleaning;

		ServerListView*			table;
		ServerListModel* 		model;
		QSortFilterProxyModel*	sortingModel;

		Qt::SortOrder 	sortOrder;
		int				sortIndex;


		void prepareServerTable();

		QString createPlayersToolTip(const Server* server);
		QString createServerNameToolTip(const Server* server);
		QString createPwadsToolTip(const Server* server);
};

#endif

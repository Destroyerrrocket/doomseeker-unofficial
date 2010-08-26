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

#include "serverapi/server.h"

#include <QObject>
#include <QStandardItem>
#include <QString>
#include <QTableView>
#include <QTimer>

#include "gui/widgets/serverlistview.h"
#include "gui/models/serverlistmodel.h"

class IniSection;

class ServerListHandler : public QObject
{
	Q_OBJECT

	public:
		ServerListHandler(ServerListView* serverTable, IniSection& config, QWidget* pMainWindow);
		~ServerListHandler();

		void 				clearTable();
		IniSection&			configurationObject() { return configuration; }

		QWidget*			getMainWindow() { return mainWindow; }

		bool				isSortingByColumn(int columnIndex);

		QList<Server*>		selectedServers();

		Server*				serverFromIndex(const QModelIndex&);

		ServerListModel*	serverModel() { return model; }
		ServerListView*		serverTable() { return table; }

	public slots:
		void 				cleanUp();
		void 				redraw();
		void 				refreshAll();
		void				refreshSelected();
		void 				serverBegunRefreshing(Server* server);
		void 				serverUpdated(Server *server, int response);

		/**
		 *	@brief Sets country flags for servers that don't have flags
		 *		present yet.
		 */
		void				setCountryFlagsIfNotPresent();

		void				tableMiddleClicked(const QModelIndex& index, const QPoint& cursorPosition);
		void 				tableRightClicked(const QModelIndex& index, const QPoint& cursorPosition);
		void 				updateCountryFlags();
		void 				updateSearch(const QString& search);

	protected slots:
		/// Handles column sorting.
		void 				columnHeaderClicked(int);
		void 				doubleClicked(const QModelIndex&);
		void 				itemSelected(const QModelIndex&);
		void 				modelCleared();
		void 				mouseEntered(const QModelIndex&);
		void 				resizeChangedRows(const QModelIndex&, int, int);

	signals:
		/**
		 * Emitted when a request for join command line show is called.
		 */
		void 					displayServerJoinCommandLine(const Server*);
		void 					serverDoubleClicked(const Server*);
		void 					serversSelected(QList<Server*>&);

	protected:
		// TODO: These need to be set by appearance configuration.
		static const QString	FONT_COLOR_MISSING;
		static const QString	FONT_COLOR_OPTIONAL;
		static const QString	FONT_COLOR_FOUND;
	
		QTimer					cleanerTimer;
		IniSection&				configuration;

		QWidget*				mainWindow;
		ServerListModel* 		model;
		bool 					needsCleaning;
		QSortFilterProxyModel*	sortingProxy;

		Qt::SortOrder 			sortOrder;
		int						sortIndex;
		ServerListView*			table;

		QString					createIwadToolTip(const Server* server);
		QString 				createPlayersToolTip(const Server* server);
		QString 				createPwadsToolTip(const Server* server);
		QString					createPwadToolTipInfo(const PWad& pwad);
		QString 				createServerNameToolTip(const Server* server);

		bool					areColumnsWidthsSettingsChanged();

		void					connectTableModelProxySlots();

		ServerListModel*		createModel();
		QSortFilterProxyModel*	createSortingProxy(ServerListModel* serverListModel);

		Qt::SortOrder			getColumnDefaultSortOrder(int columnId);

		void					initCleanerTimer();

		/**
		 *	@brief Creates default setting entries in configuration.
		 *
		 *	Will not override any existing configuration.
		 */
		void					initDefaultColumnsWidthsSettings();

		void					loadColumnsWidthsSettings();

		void 					prepareServerTable();

		void					saveColumnsWidthsSettings();

		void					setupTableColumnWidths();
		void					setupTableProperties(QSortFilterProxyModel* tableModel);

		Qt::SortOrder			swapCurrentSortOrder();

		void 					updateCountryFlags(bool force);
};

#endif

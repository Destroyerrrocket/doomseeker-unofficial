//------------------------------------------------------------------------------
// mainwindow.h
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

#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include "main.h"
#include "masterclient.h"
#include "sdeapi/config.hpp"
#include "sdeapi/pluginloader.hpp"
#include "gui/dockBuddiesList.h"
#include "gui/dockserverinfo.h"
#include "gui/serverlist.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QString>
#include <QStandardItem>

class MainWindow : public QMainWindow, private Ui::MainWindowWnd
{
	Q_OBJECT

	public:
		MainWindow(int argc, char** argv);
		~MainWindow();

	public slots:
		void checkRefreshFinished();
		void btnGetServers_Click();
		void enablePort();
		void menuBuddies();
		void menuCreateServer();
		void menuHelpAbout();
		void menuOptionsConfigure();
		void menuServerInfo();
		void menuWadSeeker();
		void runGame(const Server*);
		void updateServerInfo(QList<Server*>&);

	protected:
		QTimer	autoRefreshTimer;

		/**
		 *	This will either enable or disable the auto refresh timer
		 *	depending on the settings. This method also takes care of every
		 *	checks. It will make sure the delay between the refreshes is
		 *	inside gives boundaries (30 - 3600 seconds).
		 */
		void	initAutoRefreshTimer();

		/**
		 *	@param onlyCustom - if true, the refreshing buttons aren't blocked
		 *		and servers that aren't custom aren't contacted in any way.
		 */
		void 	refreshServers(bool onlyCustom);

	protected slots:
		void	autoRefreshTimer_timeout();

	private:
		DockBuddiesList*	buddiesList;
		DockServerInfo*		serverInfo;
		SLHandler*			serverTableHandler;

		MasterManager*		mc;
		const QAction**		queryMenuPorts;
};

#endif

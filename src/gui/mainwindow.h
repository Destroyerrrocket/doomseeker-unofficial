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
#include <QSystemTrayIcon>

/**
 *	@brief Menu action for Query Menu
 *
 *	Replaces the original QAction to make toggling of master clients easier.
 *	The constructor automatically connects the passed MasterClient's
 *	setEnabled() slot to this QQueryMenuAction toggled() signal.
 */
class QQueryMenuAction : public QAction
{
	public:
		QQueryMenuAction(MasterClient* mClient, QObject* parent = NULL)
		:QAction(parent)
		{
			if (mClient != NULL)
			{
				connect(this, SIGNAL( toggled(bool) ), mClient, SLOT( setEnabled(bool) ) );
			}
		}
};

class MainWindow : public QMainWindow, private Ui::MainWindowWnd
{
	Q_OBJECT

	public:
		MainWindow(int argc, char** argv);
		~MainWindow();

		/**
		 *	This will either enable or disable the auto refresh timer
		 *	depending on the settings. This method also takes care of every
		 *	checks. It will make sure the delay between the refreshes is
		 *	inside gives boundaries (30 - 3600 seconds).
		 */
		void	initAutoRefreshTimer();

		void	stopAutoRefreshTimer() { autoRefreshTimer.stop(); }

	protected:
		QTimer	autoRefreshTimer;

		/**
		 *	This is required so tray icon knows how to bring the window back.
		 */
		bool	bWasMaximized;

		/**
		 *	If set to true the closeEvent() method will ignore tray icon
		 *	settings and proceed to close the MainWindow. This is set by
		 *	quitProgram() slot.
		 */
		bool	bWantToQuit;

		void	changeEvent(QEvent* event);

		/**
		 *	Connects signals from objects and controls of the main window
		 *	to their slots.
		 */
		void	connectEntities();

		void	closeEvent(QCloseEvent* event);

		/**
		 *	Fills query menu with engines that have master server.
		 *	@param masterManager - instantiated MainWindow::mc is passed here.
		 *		Since this method is called from the constructor it's important
		 *		to pay attention to not call it before mc is instantiated.
		 *		In other words: this argument exists solely to avoid "random"
		 *		crashes.
		 */
		void	fillQueryMenu(MasterManager* masterManager);

		/**
		 *	Checks whether the program will use the tray icon and
		 *	deletes or instantiates a QSystemTrayIcon object.
		 */
		void	initTrayIcon();

		void	refreshCustomServers();

		/**
		 *	Excludes custom servers.
		 */
		void 	refreshServers(MasterClient* master);

		void	updateTrayIconTooltip();

	protected slots:
		void	autoRefreshTimer_timeout();
		void	blockRefreshButtons();
		void 	btnGetServers_Click();
		void	btnRefreshAll_Click();
		void 	finishedQueryingMaster(MasterClient* master);
		void	masterManagerMessages(const QString& title, const QString& content, bool isError);
		void	menuBuddies();
		void 	menuCreateServer();
		void	menuHelpAbout();
		void 	menuOptionsConfigure();
		void	menuServerInfo();
		void	menuWadSeeker();
		void	quitProgram();
		void 	refreshThreadBeginsWork();
		void	refreshThreadEndsWork();
		void 	runGame(const Server*);
		void	trayIcon_activated(QSystemTrayIcon::ActivationReason reason);
		void 	updateServerInfo(QList<Server*>&);

	private:
		DockBuddiesList*	buddiesList;
		DockServerInfo*		serverInfo;
		SLHandler*			serverTableHandler;

		MasterManager*		mc;
		QList<QAction*>		queryMenuPorts;
		QSystemTrayIcon*	trayIcon;
		QMenu*				trayIconMenu;
};

#endif

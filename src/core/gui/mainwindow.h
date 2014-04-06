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

#include "ui_mainwindow.h"

#include "gui/irc/ircdock.h"
#include "gui/dockBuddiesList.h"
#include "gui/logdock.h"
#include "gui/serverlist.h"
#include "gui/widgets/serversstatuswidget.h"
#include "serverapi/mastermanager.h"
#include "plugins/pluginloader.h"
#include "apprunner.h"

#include <QAction>
#include <QProgressBar>
#include <QString>
#include <QStandardItem>
#include <QSystemTrayIcon>

class AutoUpdater;
class ConnectionHandler;
class DoomseekerConfigurationDialog;
class IP2CLoader;
class ServerFilterDock;
class ServersStatusWidget;
class UpdateChannel;

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
		QQueryMenuAction(MasterClient* mClient, ServersStatusWidget *statusWidget, QObject* parent = NULL)
		:QAction(parent)
		{
			this->pClient = mClient;

			if (mClient != NULL)
			{
				connect(this, SIGNAL( toggled(bool) ), mClient, SLOT( setEnabled(bool) ) );
				connect(this, SIGNAL( toggled(bool) ), statusWidget, SLOT( setMasterEnabledStatus(bool) ) );
			}
		}

		MasterClient* masterClient()
		{
			return pClient;
		}

	private:
		MasterClient* pClient;
};

class MainWindow : public QMainWindow, private Ui::MainWindowWnd
{
	Q_OBJECT

	public:
		MainWindow(QApplication* application, int argc, char** argv);
		~MainWindow();

		/**
		 *	This will either enable or disable the auto refresh timer
		 *	depending on the settings. This method also takes care of every
		 *	checks. It will make sure the delay between the refreshes is
		 *	inside gives boundaries (30 - 3600 seconds).
		 */
		void initAutoRefreshTimer();

		void notifyFirstRun();

		/**
		 * @brief This should be set if program was started with
		 *        "--update-failed" argument.
		 *
		 * Display is done in a way that allows the full initialization process
		 * to complete first before displaying the actual window.
		 *
		 * @b NOTE: This is different than setDisplayUpdateInstallerError()
		 * and mutually exclusive.
		 */
		void setDisplayUpdaterProcessFailure(int errorCode);
		/**
		 * @brief If set then MainWindow will display the reason for
		 *        updater failure.
		 *
		 * Display is done in a way that allows the full initialization process
		 * to complete first before displaying the actual window.
		 *
		 * @b NOTE: This is different than setDisplayUpdaterProcessFailure()
		 * and mutually exclusive.
		 */
		void setDisplayUpdateInstallerError(int errorCode);
		/**
		 * @brief Sets query for selected MasterClient object to enabled
		 * or disabled.
		 */
		void setQueryMasterServerEnabled(MasterClient* pClient, bool bEnabled);

		void stopAutoRefreshTimer() { autoRefreshTimer.stop(); }

		void updateMasterAddresses();

	public slots:
		void runGame(const ServerPtr&);

	protected:
		friend class DoomseekerConfigurationDialog;

		static const QString HELP_SITE_URL;

		QApplication* application;
		QTimer autoRefreshTimer;

		AutoUpdater* autoUpdater;
		QWidget* autoUpdaterStatusBarWidget;
		QPushButton* autoUpdaterAbortButton;
		QLabel* autoUpdaterLabel;
		QProgressBar* autoUpdaterFileProgressBar;
		QProgressBar* autoUpdaterOverallProgressBar;

		/**
		 *	Set to true by btnGetServers_click() process and to false
		 *	when refreshing thread enters sleep mode.
		 */
		bool bTotalRefreshInProcess;

		DockBuddiesList* buddiesList;

		/**
		 *	This is required so tray icon knows how to bring the window back.
		 */
		bool bWasMaximized;

		/**
		 *	If set to true the closeEvent() method will ignore tray icon
		 *	settings and proceed to close the MainWindow. This is set by
		 *	quitProgram() slot.
		 */
		bool bWantToQuit;

		IP2CLoader* ip2cLoader;
		QProgressBar* ip2cUpdateProgressBar;
		IRCDock* ircDock;
		LogDock* logDock;
		ServerFilterDock* serverFilterDock;
		ServerListHandler* serverTableHandler;

		MasterManager* masterManager;
		QHash<MasterClient*, QQueryMenuAction*> queryMenuPorts;
		QHash<MasterClient*, ServersStatusWidget*> serversStatusesWidgets;
		QAction* toolBarGetServers;
		QAction* toolBarRefreshAll;
		QSystemTrayIcon* trayIcon;
		QMenu* trayIconMenu;
		/// Update should be discarded if this changes.
		UpdateChannel* updateChannelOnUpdateStart;
		int updaterInstallerErrorCode;

		ConnectionHandler *connectionHandler;

		void changeEvent(QEvent* event);
		void checkForUpdates(bool bUserTriggered);

		/**
		 *	Connects signals from objects and controls of the main window
		 *	to their slots.
		 */
		void connectEntities();

		void closeEvent(QCloseEvent* event);

		/**
		 *	Fills query menu with engines that have master server.
		 *	@param masterManager - instantiated MainWindow::masterManager
		 *		is passed here.
		 *		Since this method is called from the constructor it's important
		 *		to pay attention to not call it before masterManager is
		 *		instantiated.
		 *		In other words: this argument exists solely to avoid "random"
		 *		crashes.
		 */
		void fillQueryMenu(MasterManager* masterManager);

		/**
		 * Called by the configuration dialog to sync config changes.
		 */
		void finishConfiguration(DoomseekerConfigurationDialog &, bool);

		bool hasCustomServers() const;

		QProgressBar* mkStdProgressBarForStatusBar();

		void initAutoUpdaterWidgets();
		void initIP2CUpdater();
		void initIRCDock();
		void initLogDock();
		void initMainDock();
		void initServerFilterDock();

		/**
		 *	Checks whether the program will use the tray icon and
		 *	deletes or instantiates a QSystemTrayIcon object.
		 */
		void initTrayIcon();

		/**
		 *	@brief Will check if refresh operation has any sense.
		 *
		 *	@return True if there is anything to refresh (be it master or
		 *		even a single custom server). False if pressing "Get Servers"
		 *		button will produce no results whatsoever.
		 */
		bool isAnythingToRefresh() const;
		bool isAnyMasterEnabled() const;

		QQueryMenuAction* queryMenuActionForMasterClient(MasterClient* pClient);

		void refreshCustomServers();
		void setupIcons();
		void setupToolBar();

		/**
		 *	Functionality and name of this function might not be perfect but
		 *	it saves some copy&pasting in the end. The end justifies the means.
		 */
		void updateTrayIconTooltipAndLogTotalRefresh();

	protected slots:
		void abortAutoUpdater();
		void autoRefreshTimer_timeout();
		void blockRefreshButtons();
		/**
		 * @brief Auto triggered updates will display
		 *        install confirmation only if configured to.
		 */
		void checkForUpdatesAuto();
		/**
		 * @brief User triggered updates will always display
		 *        install confirmation.
		 */
		void checkForUpdatesUserTriggered();
		void confirmUpdateInstallation();
		void discardUpdates();
		void finishedQueryingMaster(MasterClient* master);
		void getServers();
		void masterManagerMessages(MasterClient* pSender, const QString& title, const QString& content, bool isError);
		void masterManagerMessagesImportant(MasterClient* pSender, const Message& objMessage);
		void menuBuddies();
		void menuCreateServer();
		void menuHelpAbout();
		void menuHelpHelp();
		void menuIRCOptions();
		void menuLog();
		void menuManageDemos();
		void menuOptionsConfigure();
		void menuRecordDemo();
		void menuUpdateIP2C();
		void menuViewIRC();
		void menuWadSeeker();

		void onAutoUpdaterDownloadAndInstallConfirmationRequest();
		void onAutoUpdaterFileProgress(qint64 bytesReceived, qint64 bytesTotal);
		void onAutoUpdaterFinish();
		void onAutoUpdaterOverallProgress(int current, int total, const QString& msg);

		void quitProgram();
		void postInitAppStartup();
		void refreshThreadBeginsWork();
		void refreshThreadEndsWork();
		void restartAndInstallUpdatesNow();
		void serverAddedToList(const ServerPtr&);
		void showServerJoinCommandLine(const ServerPtr&);
		void showUpdaterProcessErrorDialog();
		void showUpdateInstallErrorDialog();

		/**
		 * @brief Toggles specified MasterClient object enabled or disabled.
		 *
		 * This affects the query menu, servers status widgets and the master
		 * client itself.
		 */
		void toggleMasterClientEnabled(MasterClient* pClient);
		void toolBarAction(QAction* pAction);
		void trayIcon_activated(QSystemTrayIcon::ActivationReason reason);
		void updateServerFilter(const ServerListFilterInfo& filterInfo);

	private:
		QDockWidget *mainDock;

		void connectIP2CLoader(IP2CLoader* loader);

	private slots:
		void ip2cDownloadProgress(qint64 current, qint64 max);
		void ip2cJobsFinished();
		void ip2cStartUpdate();
};

#endif

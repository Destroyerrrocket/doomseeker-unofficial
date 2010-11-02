//------------------------------------------------------------------------------
// mainwindow.cpp
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
#include "configuration/doomseekerconfig.h"
#include "gui/configuration/irc/ircconfigurationdialog.h"
#include "gui/configuration/doomseekerconfigurationdialog.h"
#include "gui/helpers/playersdiagram.h"
#include "gui/widgets/serversstatuswidget.h"
#include "gui/aboutDlg.h"
#include "gui/copytextdlg.h"
#include "gui/createserver.h"
#include "gui/dockBuddiesList.h"
#include "gui/ip2cupdatebox.h"
#include "gui/mainwindow.h"
#include "gui/passwordDlg.h"
#include "gui/wadseekerinterface.h"
#include "irc/configuration/ircconfig.h"
#include "serverapi/gamerunner.h"
#include "serverapi/messages.h"
#include "customservers.h"
#include "doomseekerfilepaths.h"
#include "log.h"
#include "pathfinder.h"
#include "main.h"
#include "strings.h"
#include <QAction>
#include <QApplication>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileInfo>
#include <QIcon>
#include <QHeaderView>
#include <QMessageBox>
#include <QToolBar>
#include <QSizePolicy>

const QString MainWindow::HELP_SITE_URL = "http://skulltag.net/wiki/Doomseeker";

MainWindow::MainWindow(int argc, char** argv)
: bTotalRefreshInProcess(false), buddiesList(NULL), bWasMaximized(false),
  bWantToQuit(false), logDock(NULL), masterManager(NULL),
  trayIcon(NULL), trayIconMenu(NULL)
{
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	setupUi(this);
	setupIcons();
	
	if (Main::enginePlugins->numPlugins() == 0)
	{
		QString message = tr("\
Warning: \n\
Doomseeker failed to detect any plugins.\n\
While the core application will still work its functionality is going to be limited.\n\
One of the proper locations for plugin modules is the engines/ directory.\n\
");
		QMessageBox::warning(NULL, tr("Doomseeker - plugin load failure"), message);
	}
	
	ip2cParser = NULL;
	
	initIP2CUpdater();

	// The buddies list must always be available so we can perform certain operations on it
	buddiesList = new DockBuddiesList(this);
	menuView->addAction(buddiesList->toggleViewAction());
	connect(buddiesList, SIGNAL( joinServer(const Server*) ), this, SLOT( runGame(const Server*) ));
	buddiesList->hide();
	this->addDockWidget(Qt::LeftDockWidgetArea, buddiesList);
	initLogDock();
	initIRCDock();
	initMainDock();

	serverTableHandler = new ServerListHandler(tableServers, this);
	connectEntities();

	// Calculate screen center.
	int screenWidth = QApplication::desktop()->width();
	int screenHeight = QApplication::desktop()->height();

	// Window geometry settings
	if (!gConfig.doomseeker.areMainWindowSizeSettingsValid(screenWidth, screenHeight))
	{
		gConfig.doomseeker.mainWindowX = (screenWidth - width()) / 2;
		gConfig.doomseeker.mainWindowY = (screenHeight - height()) / 2;
		gConfig.doomseeker.mainWindowWidth = width();
		gConfig.doomseeker.mainWindowHeight = height();
	}
	else if (gConfig.doomseeker.mainWindowY < 0)
	{
		// Do not allow y values less than zero anyway.
		// We do not want to loose the titlebar.
		gConfig.doomseeker.mainWindowY = 0;
	}

	move(gConfig.doomseeker.mainWindowX, gConfig.doomseeker.mainWindowY);
	resize(gConfig.doomseeker.mainWindowWidth, gConfig.doomseeker.mainWindowHeight);

	restoreState(QByteArray::fromBase64(gConfig.doomseeker.mainWindowState.toAscii()));

	// Get the master
	masterManager = new MasterManager();
	buddiesList->scan(masterManager);
	connect(masterManager, SIGNAL( masterMessage(MasterClient*, const QString&, const QString&, bool) ), this, SLOT( masterManagerMessages(MasterClient*, const QString&, const QString&, bool) ) );

	// Allow us to enable and disable ports.
	fillQueryMenu(masterManager);

	// Init custom servers
	masterManager->customServs()->readConfig(serverTableHandler, SLOT(serverUpdated(Server *, int)), SLOT(serverBegunRefreshing(Server *)) );

	setWindowIcon(QIcon(":/icon.png"));

	// Auto refresh timer
	initAutoRefreshTimer();
	connect(&autoRefreshTimer, SIGNAL( timeout() ), this, SLOT( autoRefreshTimer_timeout() ));

	// Tray icon
	initTrayIcon();
	
	setupToolBar();

	// Player diagram styles
	int slotStyle = gConfig.doomseeker.slotStyle;
	PlayersDiagram::loadImages(slotStyle);
	
	// IP2C
	bool bParseIP2CDatabase = true;
	bool bPerformAutomaticIP2CUpdates = gConfig.doomseeker.bIP2CountryAutoUpdate;
	
	if (bPerformAutomaticIP2CUpdates)
	{
		int maxAge = gConfig.doomseeker.ip2CountryDatabaseMaximumAge;
	
		QString databasePath = DoomseekerFilePaths::ip2cDatabase();
		if (IP2CUpdater::needsUpdate(databasePath, maxAge))
		{
			ip2cStartUpdate();
			bParseIP2CDatabase = false;
		}
	}
	
	if (bParseIP2CDatabase)
	{
		ip2cParseDatabase();
	}	

	// check query on statup
	bool queryOnStartup = gConfig.doomseeker.bQueryOnStartup;
	if (queryOnStartup)
	{
		getServers();
	}
	else
	{
		// Custom servers should be refreshed no matter what.
		// They will not block the app in any way, there is no reason
		// not to refresh them.
		refreshCustomServers();
	}
}

MainWindow::~MainWindow()
{
	// Window geometry settings
	gConfig.doomseeker.bMainWindowMaximized = isMaximized();
	if (!isMaximized() && !isMinimized())
	{
		gConfig.doomseeker.mainWindowX = x();
		gConfig.doomseeker.mainWindowY = y();
		gConfig.doomseeker.mainWindowWidth = width();
		gConfig.doomseeker.mainWindowHeight = height();
	}

	gConfig.doomseeker.mainWindowState = saveState().toBase64();

	QList<QAction*> menuQueryActions = menuQuery->actions();
	QList<QAction*>::iterator it;
	for (it = menuQueryActions.begin(); it != menuQueryActions.end(); ++it)
	{
	    QAction* action = *it;
	    QString pluginName = action->text();

	    if (!pluginName.isEmpty())
	    {
			IniSection& pluginConfig = gConfig.iniSectionForPlugin(pluginName);
	        pluginConfig["Query"] = action->isChecked();
	    }
	}

	if (trayIcon != NULL)
	{
		trayIcon->setVisible(false);
		delete trayIcon;
		trayIcon = NULL;
	}

	if (trayIconMenu != NULL)
	{
		delete trayIconMenu;
		trayIconMenu = NULL;
	}

	delete serverTableHandler;

	if(masterManager != NULL)
	{
		delete masterManager;
	}
	
	if (ip2cParser != NULL)
	{
		while (ip2cParser->isParsing());
		
		delete ip2cParser;
	}
	
	delete ip2cUpdateProgressBar;
}

void MainWindow::autoRefreshTimer_timeout()
{
	if (gConfig.doomseeker.bQueryAutoRefreshDontIfActive && !isMinimized())
	{
		if (QApplication::activeWindow() != 0)
		{
			return;
		}
	}

	getServers();
}

void MainWindow::blockRefreshButtons()
{
	toolBarGetServers->setEnabled(false);
	toolBarRefreshAll->setEnabled(false);
}

void MainWindow::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::ActivationChange && isActiveWindow() && !isMinimized() && !isHidden())
	{
		serverTableHandler->cleanUp();
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

bool MainWindow::checkWadseekerValidity()
{
	QString targetDirPath = gConfig.wadseeker.targetDirectory;
	QDir targetDir(targetDirPath);
	QFileInfo targetDirFileInfo(targetDirPath);
	
	
	if (targetDirPath.isEmpty() || !targetDir.exists() || !targetDirFileInfo.isWritable())
	{
		QString message = tr("Wadseeker will not work correctly: \n\
Target directory is either not set, is invalid or cannot be written to.\n\
Please review your Configuration and/or refer to online help available from \
the Help menu.");
	
		QMessageBox::warning(this, tr("Doomseeker - Wadseeker error"), message);
		return false;
	}
	
	return true;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	// Check if tray icon is available and if we want to minimize to tray icon
	// when 'X' button is pressed. Real quit requests are handled by
	// quitProgram() method. This method sets bWantToQuit to true.
	if (trayIcon != NULL && gConfig.doomseeker.bCloseToTrayIcon && !bWantToQuit)
	{
		bWasMaximized = isMaximized();
		event->ignore();
		hide();
	}
	else
	{
		event->accept();
	}
}

void MainWindow::connectEntities()
{
	// Connect refreshing thread.
	connect(Main::refreshingThread, SIGNAL( block() ), this, SLOT( blockRefreshButtons() ) );
	connect(Main::refreshingThread, SIGNAL( finishedQueryingMaster(MasterClient*) ), this, SLOT( finishedQueryingMaster(MasterClient*) ) );
	connect(Main::refreshingThread, SIGNAL( sleepingModeEnter() ), this, SLOT( refreshThreadEndsWork() ) );
	connect(Main::refreshingThread, SIGNAL( sleepingModeExit() ), this, SLOT( refreshThreadBeginsWork() ) );

	// Controls
	connect(menuActionAbout, SIGNAL( triggered() ), this, SLOT( menuHelpAbout() ));
	connect(menuActionBuddies, SIGNAL( triggered() ), this, SLOT( menuBuddies() ));
	connect(menuActionConfigure, SIGNAL( triggered() ), this, SLOT( menuOptionsConfigure() ));
	connect(menuActionCreateServer, SIGNAL( triggered() ), this, SLOT( menuCreateServer() ));
	connect(menuActionHelp, SIGNAL( triggered() ), this, SLOT ( menuHelpHelp() ) );
	connect(menuActionIRCOptions, SIGNAL( triggered() ), this, SLOT( menuIRCOptions() ) );
	connect(menuActionLog, SIGNAL( triggered() ), this, SLOT( menuLog() ));
	connect(menuActionUpdateIP2C, SIGNAL( triggered() ), this, SLOT( menuUpdateIP2C() ) );
	connect(menuActionQuit, SIGNAL( triggered() ), this, SLOT( quitProgram() ));
	connect(menuActionViewIRC, SIGNAL( triggered() ) , this, SLOT( menuViewIRC() ));
	connect(menuActionWadseeker, SIGNAL( triggered() ), this, SLOT( menuWadSeeker() ));
	connect(serverTableHandler, SIGNAL( serverDoubleClicked(const Server*) ), this, SLOT( runGame(const Server*) ) );
	connect(serverTableHandler, SIGNAL( displayServerJoinCommandLine(const Server*) ), this, SLOT( showServerJoinCommandLine(const Server*) ) );
}

void MainWindow::fillQueryMenu(MasterManager* masterManager)
{
	// This is called only once from the constructor. No clears to
	// queryMenuPorts are ever performed. Not even in the destructor.
	for(unsigned i = 0; i < Main::enginePlugins->numPlugins(); ++i)
	{
		const EnginePlugin* plugin = (*Main::enginePlugins)[i]->info->pInterface;
		if(!plugin->hasMasterServer())
		{
			continue;
		}

		MasterClient* pMasterClient = plugin->masterClient();
		pMasterClient->updateAddress();
		masterManager->addMaster(pMasterClient);

		// Now is a good time to also populate the status bar widgets
		ServersStatusWidget *statusWidget = new ServersStatusWidget(plugin->icon(), pMasterClient);
		statusBar()->addPermanentWidget(statusWidget);

		QString name = (*Main::enginePlugins)[i]->info->name;
		QQueryMenuAction* query = new QQueryMenuAction(pMasterClient, statusWidget, menuQuery);
		menuQuery->addAction(query);

		query->setCheckable(true);
		query->setIcon(plugin->icon());
		query->setText(name);
		
		IniSection& pluginConfig = gConfig.iniSectionForPlugin(name);
		
		if (!pluginConfig.retrieveSetting("Query").isNull())
		{
			bool enabled = pluginConfig["Query"];
			if(pMasterClient != NULL)
			{
				pMasterClient->setEnabled(enabled);
			}
			
			query->setChecked(enabled);
			statusWidget->setEnabled(enabled);
		}
		else
		{
			// if no setting is found for this engine
			// set default as follows:
			if(pMasterClient != NULL)
			{
				pMasterClient->setEnabled(true);
			}
			
			query->setChecked(true);
			statusWidget->setEnabled(true);
		}
		queryMenuPorts.append(query);
	}
}

void MainWindow::finishedQueryingMaster(MasterClient* master)
{
	if (master == NULL)
	{
		return;
	}

	for(int i = 0;i < master->numServers();i++)
	{
		connect((*master)[i], SIGNAL(updated(Server *, int)), serverTableHandler, SLOT(serverUpdated(Server *, int)) );
		connect((*master)[i], SIGNAL(begunRefreshing(Server *)), serverTableHandler, SLOT(serverBegunRefreshing(Server *)) );
	}
}

void MainWindow::getServers()
{
	// Check if this operation has any sense.
	if (!isAnythingToRefresh())
	{
		gLog << tr("Senseless refresh operation attempted.");

		QString message = tr("\
Doomseeker is unable to proceed with the refresh \
operation because one or more of the following problems occured:\n\
- Plugins are missing from the engines/ directory\n\
- All master servers have been disabled from the query menu\n\
- There are no custom servers present.\n");
		QMessageBox::warning(this, tr("Doomseeker - senseless operation"), message);
		return;
	}
	
	bTotalRefreshInProcess = true;
	autoRefreshTimer.stop();
	gLog << tr("Total refresh process initialized!");
	serverTableHandler->clearTable();
	refreshCustomServers();
	
	bool bAtLeastOneEnabled = false;

	if (!isAnyMasterEnabled())
	{
		gLog << tr("Warning: No master servers were enabled for this refresh. Check your query menu or engines/ directory. Custom servers will still refresh.");
	}
	
	masterManager->clearServersList();
	for (int i = 0; i < masterManager->numMasters(); ++i)
	{
		MasterClient* pMaster = (*masterManager)[i];
		
		if (pMaster->isEnabled())
		{
			Main::refreshingThread->registerMaster(pMaster);
		}
	}
}

bool MainWindow::hasCustomServers() const
{
	CustomServers* customServers = masterManager->customServs();
	return customServers->numServers() > 0;
}

void MainWindow::initAutoRefreshTimer()
{
	const unsigned MIN_DELAY = 30;
	const unsigned MAX_DELAY = 3600;

	bool bEnabled = gConfig.doomseeker.bQueryAutoRefreshEnabled;

	if (!bEnabled)
	{
		autoRefreshTimer.stop();
	}
	else
	{
		// If delay value is out of bounds we should adjust
		// config value as well.
		unsigned& delay = gConfig.doomseeker.queryAutoRefreshEverySeconds;

		// Make sure delay is in given limit.
		if (delay < MIN_DELAY)
		{
			delay = MIN_DELAY;
		}
		else if (delay > MAX_DELAY)
		{
			delay = MAX_DELAY;
		}

		unsigned delayMs = delay * 1000;

		autoRefreshTimer.setSingleShot(false);
		autoRefreshTimer.start(delayMs);
	}
}

void MainWindow::initIP2CUpdater()
{
	static const int PROGRESSBAR_WIDTH = 250;

	ip2cUpdater = NULL;
	ip2cUpdateProgressBar = new QProgressBar();
	ip2cUpdateProgressBar->setFormat(tr("IP2C Update"));
	ip2cUpdateProgressBar->setTextVisible(true);
	ip2cUpdateProgressBar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	
	ip2cUpdateProgressBar->setMaximumWidth(PROGRESSBAR_WIDTH);
	ip2cUpdateProgressBar->setMinimumWidth(PROGRESSBAR_WIDTH);
}

void MainWindow::initIRCDock()
{
	ircDock = new IRCDock(this);
	menuView->addAction(ircDock->toggleViewAction());
	ircDock->hide();
	this->addDockWidget(Qt::BottomDockWidgetArea, ircDock);
	
	if (gIRCConfig.isAutojoinNetworksEnabled())
	{
		this->ircDock->setVisible(true);
		this->ircDock->performNetworkAutojoins();
	}
}

void MainWindow::initLogDock()
{
	logDock = new LogDock(this);
	menuView->addAction(logDock->toggleViewAction());
	logDock->hide();
	this->addDockWidget(Qt::BottomDockWidgetArea, logDock);

	connect(&gLog, SIGNAL( newEntry(const QString&) ), logDock, SLOT( appendLogEntry(const QString&) ) );

	// Also add anything that already might be in the log to the box.
	logDock->appendLogEntry(gLog.content());
}

void MainWindow::initMainDock()
{
	setDockNestingEnabled(true); // This line allows us to essentually treat a dock as a central widget.

	// Make a dock out of the server table and then get rid of the central
	// widget.
	QDockWidget *mainDock = new QDockWidget(tr("Servers"));
	mainDock->setObjectName("ServerList");
	mainDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	mainDock->setWidget(tableServers);
	setCentralWidget(0);
	addDockWidget(Qt::RightDockWidgetArea, mainDock);
}

void MainWindow::initTrayIcon()
{
	bool isEnabled = gConfig.doomseeker.bUseTrayIcon;
	if (!isEnabled || !QSystemTrayIcon::isSystemTrayAvailable())
	{
		if (trayIcon != NULL)
		{
			delete trayIcon;
			trayIcon = NULL;
		}

		if (trayIconMenu != NULL)
		{
			delete trayIconMenu;
			trayIconMenu = NULL;
		}
	}
	else if (trayIcon == NULL)
	{
		QAction* trayAction;
		trayIconMenu = new QMenu(this);
		trayAction = trayIconMenu->addAction("Exit");
		connect(trayAction, SIGNAL( triggered() ), this, SLOT( quitProgram() ) );

		// This should be automatically deleted when main window closes
		trayIcon = new QSystemTrayIcon(this);
		connect(trayIcon, SIGNAL( activated(QSystemTrayIcon::ActivationReason) ), this, SLOT( trayIcon_activated(QSystemTrayIcon::ActivationReason) ) );

		updateTrayIconTooltipAndLogTotalRefresh();

		trayIcon->setContextMenu(trayIconMenu);
		trayIcon->setIcon(QIcon(":/icon.png"));
		trayIcon->setVisible(true);
	}
}

void MainWindow::ip2cDownloadProgress(int current, int max)
{
	ip2cUpdateProgressBar->setMaximum(max);
	ip2cUpdateProgressBar->setValue(current);
}

void MainWindow::ip2cFinishUpdate(const QByteArray& downloadedData)
{
	if (downloadedData.isEmpty())
	{
		QString message = tr("IP2C download has failed.");
		gLog << message;
		statusBar()->showMessage(message);
		
		ip2cJobsFinished();
	}
	else
	{
		gLog << tr("IP2C database finished downloading.");
		
		QString filePath = DoomseekerFilePaths::ip2cDatabase();
				
		ip2cUpdater->getRollbackData();
		
		if (!ip2cUpdater->saveDownloadedData())
		{
			gLog << tr("Unable to save IP2C database at path: %1").arg(filePath);
			
			ip2cJobsFinished();
			return;
		}
	
		ip2cParseDatabase();
	}
}

void MainWindow::ip2cFinishedParsing(bool bSuccess)
{
	QString filePath = DoomseekerFilePaths::ip2cDatabase();

	if (!bSuccess)
	{
		QString message = tr("Failed to read IP2C database. Reverting...");
		gLog << message;
		statusBar()->showMessage(message);
		
		if (ip2cUpdater == NULL || !ip2cUpdater->hasRollbackData())
		{
			gLog << "IP2C revert attempt failed. Nothing to go back to.";

			// Delete file in this case.
			QFile file(filePath);			
			file.remove();
			
			gLog << "Using precompiled IP2C database.";
			ip2cParser->readDatabaseThreaded(DoomseekerFilePaths::IP2C_QT_SEARCH_PATH);
		}
		else
		{
			// Revert to old content.
			ip2cUpdater->rollback();			
						
			// Must succeed now.
			ip2cParser->readDatabaseThreaded(filePath);
		}
	}
	else  
	{
		if (ip2cUpdater != NULL)
		{
			QString message = tr("IP2C database updated successfully.");
			gLog << message;
			statusBar()->showMessage(message);
		}
		
		ip2cJobsFinished();
	}
}

void MainWindow::ip2cJobsFinished()
{
	menuActionUpdateIP2C->setEnabled(true);
	Main::ip2c->setDataAccessLockEnabled(false);
	serverTableHandler->updateCountryFlags();		
	
	if (statusBar()->isAncestorOf(ip2cUpdateProgressBar))
	{
		statusBar()->removeWidget(ip2cUpdateProgressBar);
	}
	
	if (ip2cParser != NULL)
	{
		delete ip2cParser;
		ip2cParser = NULL;	
		
		QString message = tr("IP2C parsing finished.");
		
		gLog << message;
		statusBar()->showMessage(message);
	}
	
	if (ip2cUpdater != NULL)
	{
		delete ip2cUpdater;
		ip2cUpdater = NULL;
		
		QString message = tr("IP2C update finished.");
		
		gLog << message;
		statusBar()->showMessage(message);
	}
}

void MainWindow::ip2cParseDatabase()
{
	QString filePath = DoomseekerFilePaths::IP2C_QT_SEARCH_PATH;
	
	menuActionUpdateIP2C->setEnabled(false);
	
	statusBar()->showMessage(tr("Please wait. IP2C Database is being read and converted if necessary. This may take some time."));
	// Attempt to read IP2C database.
	ip2cParser = new IP2CParser(Main::ip2c);
	connect (ip2cParser, SIGNAL( parsingFinished(bool) ), this, SLOT( ip2cFinishedParsing(bool) ) );
	
	Main::ip2c->setDataAccessLockEnabled(true);
	ip2cParser->readDatabaseThreaded(filePath);
}

void MainWindow::ip2cStartUpdate()
{
	if (ip2cUpdater != NULL)
	{
		// If update is currently in progress then prevent re-starting.
		return;
	}

	gLog << tr("Starting IP2C update.");
	menuActionUpdateIP2C->setEnabled(false);
	Main::ip2c->setDataAccessLockEnabled(true);	
	
	ip2cUpdater = new IP2CUpdater();
	ip2cUpdater->setFilePath(DoomseekerFilePaths::ip2cDatabase());
	
	connect (ip2cUpdater, SIGNAL( databaseDownloadFinished(const QByteArray&) ), this, SLOT( ip2cFinishUpdate(const QByteArray&) ) );
	connect (ip2cUpdater, SIGNAL( downloadProgress(int, int) ), this, SLOT( ip2cDownloadProgress(int, int) ) );
	
	QString downloadUrl = gConfig.doomseeker.ip2CountryUrl;
	
	ip2cUpdater->downloadDatabase(downloadUrl);
	statusBar()->addPermanentWidget(ip2cUpdateProgressBar);
}

bool MainWindow::isAnythingToRefresh() const
{
	return hasCustomServers() || isAnyMasterEnabled();
}

bool MainWindow::isAnyMasterEnabled() const
{
	for (int i = 0; i < masterManager->numMasters(); ++i)
	{
		MasterClient* pMaster = (*masterManager)[i];
		
		if (pMaster->isEnabled())
		{
			return true;
		}
	}
	
	return false;
}

void MainWindow::masterManagerMessages(MasterClient* pSender, const QString& title, const QString& content, bool isError)
{
	QString message = tr("Master server for %1: %2").arg(pSender->plugin()->name).arg(content);

	if (isError)
	{
		message = tr("Error: %1").arg(message);
		statusBar()->showMessage(message);
	}
	
	gLog << message;
}

void MainWindow::menuBuddies()
{
	buddiesList->setVisible(!buddiesList->isVisible());
}

void MainWindow::menuCreateServer()
{
	CreateServerDlg dlg(this);
	autoRefreshTimer.stop();
	dlg.exec();
	initAutoRefreshTimer();
}

void MainWindow::menuHelpAbout()
{
	AboutDlg dlg(this);
	autoRefreshTimer.stop();
	dlg.exec();
	initAutoRefreshTimer();
}

void MainWindow::menuHelpHelp()
{
	if (HELP_SITE_URL.isEmpty() || !Strings::isUrlSafe(HELP_SITE_URL))
	{
		QMessageBox::critical(this, tr("Help error"), tr("No help found"), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	bool bSuccess = QDesktopServices::openUrl(HELP_SITE_URL);
	
	if (!bSuccess)
	{
		QMessageBox::critical(this, tr("Help error"), tr("Failed to open URL:\n%1").arg(HELP_SITE_URL), QMessageBox::Ok, QMessageBox::Ok);
		return;
	}
}

void MainWindow::menuIRCOptions()
{
	IRCConfigurationDialog dialog(this);
	dialog.initOptionsList();
	dialog.exec();
	
	if (ircDock != NULL)
	{
		ircDock->applyAppearanceSettings();
	}
}

void MainWindow::menuLog()
{
	logDock->setVisible(!logDock->isVisible());
}

void MainWindow::menuOptionsConfigure()
{
	DoomseekerConfigurationDialog configDialog(this);
	configDialog.initOptionsList();

	for(unsigned i = 0; i < Main::enginePlugins->numPlugins(); ++i)
	{
		const PluginInfo* pPluginInfo = (*Main::enginePlugins)[i]->info;
		
		// Retrieve INI Section for this plugin.
		QString pluginName = pPluginInfo->name;
		IniSection& configSection = gConfig.iniSectionForPlugin(pluginName);
		
		// Create the config box.
		ConfigurationBaseBox* pConfigurationBox = pPluginInfo->pInterface->configuration(configSection, &configDialog);
		configDialog.addEngineConfiguration(pConfigurationBox);
	}

	// Stop the auto refresh timer during configuration.
	autoRefreshTimer.stop();
	configDialog.exec();

	// Do some cleanups after config box finishes.
	initAutoRefreshTimer();
	
	// In case the master server addresses changed, notify the master clients.
	updateMasterAddresses();

	// If appearance changed - update the widgets.
	if (configDialog.appearanceChanged())
	{
		serverTableHandler->redraw();
		initTrayIcon();
	}

	// Refresh custom servers list:
	if (configDialog.customServersChanged())
	{
		serverTableHandler->serverModel()->removeCustomServers();
		masterManager->customServs()->readConfig(serverTableHandler, SLOT(serverUpdated(Server *, int)), SLOT(serverBegunRefreshing(Server *)) );
		refreshCustomServers();
	}
}

void MainWindow::menuUpdateIP2C()
{
	IP2CUpdateBox updateBox(this);
	
	connect(&updateBox, SIGNAL( accepted() ), this, SLOT( ip2cStartUpdate() ) );
	
	updateBox.exec();
}

void MainWindow::menuViewIRC()
{
	ircDock->setVisible(!ircDock->isVisible());
}

void MainWindow::menuWadSeeker()
{
	if (!checkWadseekerValidity())
	{
		return;
	}

	WadSeekerInterface wadSeekerInterface(this);
	wadSeekerInterface.exec();
}

void MainWindow::notifyFirstRun()
{
	// On first run prompt configuration box.
	if (gConfig.ini()->isFirstRun())
	{
		QMessageBox::information(NULL, tr("Welcome to Doomseeker"), tr("Before you start browsing for servers, please ensure that Doomseeker is properly configured."));
		menuActionConfigure->trigger();
	}
}

bool MainWindow::obtainJoinCommandLine(const Server* server, CommandLineInfo& cli, const QString& errorCaption)
{
	cli.applicationDir = "";
	cli.args.clear();
	cli.executable = QFileInfo("");

	if (server != NULL)
	{
		// Remember to check REFRESHING status first!
		if (server->isRefreshing())
		{
			QMessageBox::warning(this, errorCaption, tr("This server is still refreshing.\nPlease wait until it is finished."));
			gLog << tr("Attempted to obtain a join command line for a \"%1\" server that is under refresh.").arg(server->addressWithPort());
			return false;
		}
		// Fail if Doomseeker couldn't get data on this server.
		else if (!server->isKnown())
		{
			QMessageBox::critical(this, errorCaption, tr("Data for this server is not available.\nOperation failed."));
			gLog << tr("Attempted to obtain a join command line for an unknown server \"%1\"").arg(server->addressWithPort());
			return false;
		}
	
		// For MissingWads:
		const QString filesMissingCaption = tr("Doomseeker - files are missing");
		QString filesMissingMessage = tr("Following files are missing:\n");

		QString connectPassword;
		if(server->isLocked())
		{
			PasswordDlg password(this);
			int ret = password.exec();

			if(ret == QDialog::Accepted)
				connectPassword = password.connectPassword();
			else
				return false;
		}

		GameRunner* gameRunner = server->gameRunner();
		JoinError joinError = gameRunner->createJoinCommandLine(cli, connectPassword);
		delete gameRunner;

		const QString unknownError = tr("Unknown error.");
		const QString* error = NULL;

		switch (joinError.type)
		{
			case JoinError::Critical:
				if (!joinError.error.isEmpty())
				{
					error = &joinError.error;
				}
				else
				{
					error = &unknownError;
				}

				QMessageBox::critical(this, errorCaption, *error);
				gLog << tr("Error when obtaining join parameters for server \"%1\", game \"%2\": %3").arg(server->name()).arg(server->engineName()).arg(*error);
				return false;

			case JoinError::MissingWads:
				// Execute Wadseeker
				if (!joinError.missingIwad.isEmpty())
				{
					QString additionalInfo = tr("\n\
Make sure that this file is in one of the paths specified in Options -> File Paths.\n\
If you don't have this file you need to purchase the game associated with this IWAD.\n\
Wadseeker will not download IWADs.\n\n");

					filesMissingMessage += tr("IWAD: ") + joinError.missingIwad.toLower() + additionalInfo;
				}

				if (!joinError.missingWads.isEmpty())
				{
					filesMissingMessage += tr("PWADS: %1\nDo you want Wadseeker to find missing PWADs?").arg(joinError.missingWads.join(" "));
				}

				if (joinError.isMissingIwadOnly())
				{
					QMessageBox::critical(this, filesMissingCaption, filesMissingMessage, QMessageBox::Ok);
				}
				else
				{
					if (QMessageBox::question(this, filesMissingCaption, filesMissingMessage, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
					{
						if (!checkWadseekerValidity())
						{
							return false;
						}
					
						if (!joinError.missingIwad.isEmpty())
						{
							joinError.missingWads.append(joinError.missingIwad);
						}

						WadSeekerInterface wsi;
						wsi.setAutomatic(true, joinError.missingWads);
						wsi.wadseekerRef().setCustomSite(server->website());
						if (wsi.exec() == QDialog::Accepted)
						{
							return obtainJoinCommandLine(server, cli, errorCaption);
						}
					}
				}
				return false;

			case JoinError::NoError:
				break;

			default:
				break;
		}
	}

	return true;
}

void MainWindow::quitProgram()
{
	bWantToQuit = true;
	close();
}

void MainWindow::refreshCustomServers()
{
	CustomServers* customServers = masterManager->customServs();

	for(int i = 0;i < customServers->numServers();i++)
	{
		Server* server = (*customServers)[i];
		serverTableHandler->serverUpdated(server, Server::RESPONSE_NO_RESPONSE_YET);
		server->refresh(); // This will register server with refreshing thread.
	}
}

void MainWindow::refreshThreadBeginsWork()
{
	// disable refresh.
	serverTableHandler->serverTable()->setAllowAllRowsRefresh(false);
	statusBar()->showMessage(tr("Querying..."));
}

void MainWindow::refreshThreadEndsWork()
{
	toolBarGetServers->setEnabled(true);
	toolBarRefreshAll->setEnabled(true);

	serverTableHandler->serverTable()->setAllowAllRowsRefresh(true);
	statusBar()->showMessage(tr("Done"));
	updateTrayIconTooltipAndLogTotalRefresh();

	if (bTotalRefreshInProcess)
	{
		initAutoRefreshTimer();
	}

	bTotalRefreshInProcess = false;
}

void MainWindow::runGame(const Server* server)
{
	CommandLineInfo cli;
	if (obtainJoinCommandLine(server, cli, tr("Doomseeker - join server")))
	{
		GameRunner* gameRunner = server->gameRunner();

		Message message = gameRunner->runExecutable(cli, false);
		if (message.isError())
		{
			gLog << tr("Error while launching executable for server \"%1\", game \"%2\": %3").arg(server->name()).arg(server->engineName()).arg(message.content);
			QMessageBox::critical(this, tr("Doomseeker - launch executable"), message.content);
		}

		delete gameRunner;
	}
}

void MainWindow::setupIcons()
{
	QStyle& style = *QApplication::style();

	// File menu.
	menuActionQuit->setIcon(style.standardIcon(QStyle::SP_TitleBarCloseButton));
	
	// Help menu.
	menuActionHelp->setIcon(style.standardIcon(QStyle::SP_MessageBoxQuestion));
	menuActionAbout->setIcon(style.standardIcon(QStyle::SP_MessageBoxInformation));
}

void MainWindow::setupToolBar()
{
	QToolBar* pToolBar = new QToolBar(this);
	pToolBar->setMovable(false);

	// Refresh buttons
	toolBarGetServers = new QAction(QIcon(":/icons/arrow-down-double.png"), tr("Get Servers"), pToolBar);
	toolBarRefreshAll = new QAction(QIcon(":/icons/view-refresh-2.png"), tr("Refresh All"), pToolBar);
	
	// Search widgets
	toolBarSearch = new QLineEdit();
	toolBarSearch->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	toolBarSearch->setMinimumWidth(175);
	toolBarSearch->setMaximumWidth(175);

	QWidget* searchSeparator = new QWidget();
	searchSeparator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

	// Setup menu
	// Refresh buttons
	pToolBar->addAction(toolBarGetServers);
	pToolBar->addAction(toolBarRefreshAll);
	
	// File menu buttons.
	pToolBar->addSeparator();
	pToolBar->addAction(menuActionCreateServer);
	pToolBar->addAction(menuActionWadseeker);
	
	pToolBar->addSeparator();

	// Dockable windows buttons.
	pToolBar->addAction(buddiesList->toggleViewAction());
	pToolBar->addAction(logDock->toggleViewAction());
	pToolBar->addAction(ircDock->toggleViewAction());

	pToolBar->addWidget(searchSeparator);
	pToolBar->addWidget(new QLabel(tr("Search:"), pToolBar));
	pToolBar->addWidget(toolBarSearch);
	
	this->addToolBar(Qt::TopToolBarArea, pToolBar);
	setUnifiedTitleAndToolBarOnMac(true);
	connect(pToolBar, SIGNAL( actionTriggered(QAction*) ), this, SLOT( toolBarAction(QAction*) ) );
	
	// Toolbar controls (those that aren't handled by the generic toolbar slot).
	connect(toolBarSearch, SIGNAL( textChanged(const QString &) ), serverTableHandler, SLOT( updateSearch(const QString &) ));		
}

void MainWindow::showServerJoinCommandLine(const Server* server)
{
	CommandLineInfo cli;
	if (obtainJoinCommandLine(server, cli, tr("Doomseeker - join command line")))
	{
		CopyTextDlg ctd(cli.executable.absoluteFilePath() + " " + cli.args.join(" "), server->name(), this);
		ctd.exec();
	}
}

void MainWindow::toolBarAction(QAction* pAction)
{
	if (pAction == toolBarGetServers)
	{
		getServers();
	}
	else if (pAction == toolBarRefreshAll)
	{
		serverTableHandler->refreshAll();		
	}
}

void MainWindow::trayIcon_activated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)
	{
		if (isMinimized() || !isVisible())
		{
			bWasMaximized == true ? showMaximized() : showNormal();
			activateWindow();
		}
		else if (gConfig.doomseeker.bCloseToTrayIcon)
		{
			close();
		}
		else
		{
			showMinimized();
		}
	}
}

// NOTE: Probably would be better if the master manager wasn't tied to the
//       MainWindow class?
void MainWindow::updateMasterAddresses()
{
	for(int i = 0;i < masterManager->numMasters();i++)
		(*masterManager)[i]->updateAddress();
}

void MainWindow::updateTrayIconTooltipAndLogTotalRefresh()
{
	int numServers = masterManager->numServers();
	int numCustoms = masterManager->customServs()->numServers();
	int numPlayers = masterManager->numPlayers() + masterManager->customServs()->numPlayers();

	if (trayIcon != NULL)
	{
		QString tip;
		tip += "Servers: " + QString::number(numServers) + " + " + QString::number(numCustoms) + " custom\n";
		tip += "Players: " + QString::number(numPlayers);
		trayIcon->setToolTip(tip);
	}

	if (bTotalRefreshInProcess)
	{
		gLog << tr("Finished refreshing. Servers on the list: %1 (+ %2 custom). Players: %3.").arg(numServers).arg(numCustoms).arg(numPlayers);
	}
}

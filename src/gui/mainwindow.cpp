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

#include "gui/aboutDlg.h"
#include "gui/configureDlg.h"
#include "gui/copytextdlg.h"
#include "gui/createserver.h"
#include "gui/dockBuddiesList.h"
#include "gui/ip2cupdatebox.h"
#include "gui/mainwindow.h"
#include "gui/passwordDlg.h"
#include "gui/wadseekerinterface.h"
#include "gui/helpers/playersdiagram.h"
#include "gui/widgets/serversstatuswidget.h"
#include "serverapi/gamerunner.h"
#include "serverapi/messages.h"
#include "customservers.h"
#include "doomseekerfilepaths.h"
#include "log.h"
#include "pathfinder.h"
#include "main.h"
#include "strings.h"
#include <QAction>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileInfo>
#include <QIcon>
#include <QHeaderView>
#include <QMessageBox>
#include <QToolBar>
#include <QSizePolicy>

const QString MainWindow::HELP_SITE_URL = "http://skulltag.net/wiki/Doomseeker";

MainWindow::MainWindow(int argc, char** argv, IniSection& config)
: bTotalRefreshInProcess(false), buddiesList(NULL), bWasMaximized(false),
  bWantToQuit(false), configuration(config), logDock(NULL), masterManager(NULL),
  trayIcon(NULL), trayIconMenu(NULL)
{
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	setupUi(this);
	setupIcons();
	setupToolBar();	
	
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
	initIRCDock();
	initLogDock();

	serverTableHandler = new ServerListHandler(tableServers, configuration, this);
	connectEntities();

	// Window geometry settings
	configuration.createSetting("MainWindowX", x());
	configuration.createSetting("MainWindowY", y());
	configuration.createSetting("MainWindowWidth", width());
	configuration.createSetting("MainWindowHeight", height());

	move(configuration["MainWindowX"], configuration["MainWindowY"]);
	resize(configuration["MainWindowWidth"], configuration["MainWindowHeight"]);

	// Get the master
	masterManager = new MasterManager();
	connect(masterManager, SIGNAL( masterMessage(MasterClient*, const QString&, const QString&, bool) ), this, SLOT( masterManagerMessages(MasterClient*, const QString&, const QString&, bool) ) );

	// Allow us to enable and disable ports.
	fillQueryMenu(masterManager);

	// Init custom servers
	masterManager->customServs()->readConfig(configuration, serverTableHandler, SLOT(serverUpdated(Server *, int)), SLOT(serverBegunRefreshing(Server *)) );

	setWindowIcon(QIcon(":/icon.png"));

	// The buddies list must always be available so we can perform certain operations on it
	buddiesList = new DockBuddiesList(this);
	connect(buddiesList, SIGNAL( visibilityChanged(bool)), menuActionBuddies, SLOT( setChecked(bool)));
	connect(buddiesList, SIGNAL( joinServer(const Server*) ), this, SLOT( runGame(const Server*) ));
	buddiesList->scan(masterManager);
	buddiesList->hide();
	this->addDockWidget(Qt::LeftDockWidgetArea, buddiesList);

	// Auto refresh timer
	initAutoRefreshTimer();
	connect(&autoRefreshTimer, SIGNAL( timeout() ), this, SLOT( autoRefreshTimer_timeout() ));

	// Tray icon
	initTrayIcon();

	// Player diagram styles
	int slotStyle = configuration["SlotStyle"];
	PlayersDiagram::loadImages(slotStyle);
	
	// IP2C
	bool bParseIP2CDatabase = true;
	bool bPerformAutomaticIP2CUpdates = configuration["IP2CAutoUpdate"];
	
	if (bPerformAutomaticIP2CUpdates)
	{
		int maxAge = configuration["IP2CMaximumAge"];
	
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
	bool queryOnStartup = configuration["QueryOnStartup"];
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
	configuration["MainWindowMaximized"] = isMaximized();
	if (!isMaximized() && !isMinimized())
	{
		configuration["MainWindowX"] = x();
		configuration["MainWindowY"] = y();
		configuration["MainWindowWidth"] = width();
		configuration["MainWindowHeight"] = height();
	}

	QList<QAction*> menuQueryActions = menuQuery->actions();
	QList<QAction*>::iterator it;
	for (it = menuQueryActions.begin(); it != menuQueryActions.end(); ++it)
	{
	    QAction* action = *it;

	    if (!action->text().isEmpty())
	    {
	        Main::ini->setting(QString(action->text()).replace(' ', ""), "Query") = action->isChecked();
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
	if (configuration["QueryAutoRefreshDontIfActive"] && !isMinimized())
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
	QString targetDirPath = Main::ini->setting("Wadseeker", "TargetDirectory");
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
	if (trayIcon != NULL && configuration["CloseToTrayIcon"] && !bWantToQuit)
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
	connect(menuActionLog, SIGNAL( triggered() ), this, SLOT( menuLog() ));
	connect(menuActionUpdateIP2C, SIGNAL( triggered() ), this, SLOT( menuUpdateIP2C() ) );
	connect(menuActionQuit, SIGNAL( triggered() ), this, SLOT( quitProgram() ));
	connect(menuActionViewIRC, SIGNAL( triggered() ) , this, SLOT( menuViewIRC() ));
	connect(menuActionWadseeker, SIGNAL( triggered() ), this, SLOT( menuWadSeeker() ));
	connect(serverTableHandler, SIGNAL( serverDoubleClicked(const Server*) ), this, SLOT( runGame(const Server*) ) );
	connect(serverTableHandler, SIGNAL( displayServerJoinCommandLine(const Server*) ), this, SLOT( showServerJoinCommandLine(const Server*) ) );
	
	// Toolbar controls (those that aren't handled by the generic toolbar slot).
	connect(toolBarSearch, SIGNAL( textChanged(const QString &) ), serverTableHandler, SLOT( updateSearch(const QString &) ));	
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

		if (!Main::ini->retrieveSetting(name, "Query").isNull())
		{
			bool enabled = Main::ini->retrieveSetting(name, "Query");
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

	IniSection& cfg = configuration;

	bool bEnabled = cfg["QueryAutoRefreshEnabled"];

	if (!bEnabled)
	{
		autoRefreshTimer.stop();
	}
	else
	{
		IniVariable &setting = cfg["QueryAutoRefreshEverySeconds"];
		unsigned delay = setting;

		// Make sure delay is in given limit.
		if (delay < MIN_DELAY)
		{
			setting = MIN_DELAY;
			delay = MIN_DELAY;
		}
		else if (delay > MAX_DELAY)
		{
			setting = MAX_DELAY;
			delay = MAX_DELAY;
		}

		delay *= 1000; // seconds to miliseconds

		autoRefreshTimer.setSingleShot(false);
		autoRefreshTimer.start(delay);
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
	connect(ircDock, SIGNAL( visibilityChanged(bool)), menuActionViewIRC, SLOT( setChecked(bool)) );
	ircDock->hide();
	this->addDockWidget(Qt::BottomDockWidgetArea, ircDock);
}

void MainWindow::initLogDock()
{
	logDock = new LogDock(this);
	connect(logDock, SIGNAL( visibilityChanged(bool)), menuActionLog, SLOT( setChecked(bool)));
	logDock->hide();
	this->addDockWidget(Qt::BottomDockWidgetArea, logDock);

	connect(&gLog, SIGNAL( newEntry(const QString&) ), logDock, SLOT( appendLogEntry(const QString&) ) );

	// Also add anything that already might be in the log to the box.
	logDock->appendLogEntry(gLog.content());
}

void MainWindow::initTrayIcon()
{
	bool isEnabled = configuration["UseTrayIcon"];
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
	
	QString downloadUrl = configuration["IP2CUrl"];
	
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

void MainWindow::menuLog()
{
	logDock->setVisible(!logDock->isVisible());
}

void MainWindow::menuOptionsConfigure()
{
	ConfigureDlg configDialog(this);

	for(unsigned i = 0; i < Main::enginePlugins->numPlugins(); ++i)
	{
		const PluginInfo* pPluginInfo = (*Main::enginePlugins)[i]->info;
		ConfigurationBoxInfo* pConfigurationBoxInfo = pPluginInfo->pInterface->configuration(Main::ini->createSection(QString(pPluginInfo->name).replace(' ', "")), &configDialog);
		configDialog.addEngineConfiguration(pConfigurationBoxInfo);
	}

	// Stop the auto refresh timer during configuration.
	autoRefreshTimer.stop();
	configDialog.exec();

	// Do some cleanups after config box finishes.
	initAutoRefreshTimer();

	if (configDialog.appearanceChanged())
	{
		serverTableHandler->redraw();
		initTrayIcon();
	}

	// Refresh custom servers list:
	if (configDialog.customServersChanged())
	{
		serverTableHandler->serverModel()->removeCustomServers();
		masterManager->customServs()->readConfig(configuration, serverTableHandler, SLOT(serverUpdated(Server *, int)), SLOT(serverBegunRefreshing(Server *)) );
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

	toolBarGetServers = new QAction(QIcon(":/icons/arrow-down-double.png"), tr("Get Servers"), pToolBar);
	toolBarRefreshAll = new QAction(QIcon(":/icons/view-refresh-2.png"), tr("Refresh All"), pToolBar);
	
	toolBarSearch = new QLineEdit();
	toolBarSearch->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	toolBarSearch->setMinimumWidth(175);
	toolBarSearch->setMaximumWidth(175);

	QWidget* searchSeparator = new QWidget();
	searchSeparator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

	pToolBar->addAction(toolBarGetServers);
	pToolBar->addAction(toolBarRefreshAll);

	pToolBar->addWidget(searchSeparator);
	pToolBar->addWidget(new QLabel(tr("Search:"), pToolBar));
	pToolBar->addWidget(toolBarSearch);
	
	this->addToolBar(Qt::TopToolBarArea, pToolBar);
	connect(pToolBar, SIGNAL( actionTriggered(QAction*) ), this, SLOT( toolBarAction(QAction*) ) );
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
		else if (configuration["CloseToTrayIcon"])
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

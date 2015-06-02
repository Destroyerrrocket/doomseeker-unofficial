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
#include "configuration/queryspeed.h"
#include "gui/configuration/irc/ircconfigurationdialog.h"
#include "gui/configuration/doomseekerconfigurationdialog.h"
#include "gui/helpers/playersdiagram.h"
#include "gui/irc/ircdock.h"
#include "gui/irc/ircsounds.h"
#include "gui/models/serverlistmodel.h"
#include "gui/widgets/serversstatuswidget.h"
#include "gui/aboutdialog.h"
#include "gui/copytextdlg.h"
#include "gui/createserverdialog.h"
#include "gui/demomanager.h"
#include "gui/dockBuddiesList.h"
#include "gui/ip2cupdatebox.h"
#include "gui/logdock.h"
#include "gui/mainwindow.h"
#include "gui/programargshelpdialog.h"
#include "gui/serverdetailsdock.h"
#include "gui/serverfilterdock.h"
#include "gui/serverlist.h"
#include "gui/wadseekerinterface.h"
#include "gui/wadseekershow.h"
#include "ip2c/ip2cloader.h"
#include "irc/configuration/chatnetworkscfg.h"
#include "irc/configuration/ircconfig.h"
#include "pathfinder/pathfinder.h"
#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"
#include "refresher/refresher.h"
#include "serverapi/broadcast.h"
#include "serverapi/broadcastmanager.h"
#include "serverapi/gameclientrunner.h"
#include "serverapi/mastermanager.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "updater/autoupdater.h"
#include "updater/updatechannel.h"
#include "updater/updateinstaller.h"
#include "updater/updatepackage.h"
#include "apprunner.h"
#include "commandline.h"
#include "connectionhandler.h"
#include "customservers.h"
#include "doomseekerfilepaths.h"
#include "fileutils.h"
#include "gamedemo.h"
#include "joincommandlinebuilder.h"
#include "pathfinder/wadpathfinder.h"
#include "log.h"
#include "main.h"
#include "strings.h"
#include "ui_mainwindow.h"
#include <cassert>
#include <QAction>
#include <QApplication>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileInfo>
#include <QHeaderView>
#include <QIcon>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QToolBar>
#include <QSizePolicy>

const QString MainWindow::HELP_SITE_URL = "http://doomseeker.drdteam.org/help";

/**
 *	@brief Menu action for Query Menu
 *
 *	Replaces the original QAction to make toggling of master clients easier.
 *	The constructor automatically connects the passed MasterClient's
 *	setEnabled() slot to this QueryMenuAction toggled() signal.
 */
class QueryMenuAction : public QAction
{
	public:
		QueryMenuAction(const EnginePlugin *plugin, ServersStatusWidget *statusWidget, QObject* parent = NULL)
		:QAction(parent)
		{
			this->pPlugin = plugin;

			if (plugin != NULL)
			{
				connect(this, SIGNAL( toggled(bool) ), plugin->data()->masterClient,
					SLOT( setEnabled(bool) ) );
				connect(this, SIGNAL( toggled(bool) ), statusWidget, SLOT( setMasterEnabledStatus(bool) ) );
			}
		}

		const EnginePlugin* plugin() const
		{
			return pPlugin;
		}

	private:
		const EnginePlugin* pPlugin;
};

DClass<MainWindow> : public Ui::MainWindowWnd
{
public:
	PrivData() : bTotalRefreshInProcess(false), buddiesList(NULL),
	bWasMaximized(false), bWantToQuit(false), logDock(NULL),
	masterManager(NULL), trayIcon(NULL), trayIconMenu(NULL)
	{
	}

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
	BroadcastManager *broadcastManager;

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
	ServerDetailsDock* detailsDock;
	ServerFilterDock* serverFilterDock;
	ServerListHandler* serverTableHandler;

	MasterManager* masterManager;
	QHash<const EnginePlugin*, QueryMenuAction*> queryMenuPorts;
	QHash<const EnginePlugin*, ServersStatusWidget*> serversStatusesWidgets;
	QAction* toolBarGetServers;
	QSystemTrayIcon* trayIcon;
	QMenu* trayIconMenu;
	/// Update should be discarded if this changes.
	UpdateChannel* updateChannelOnUpdateStart;
	int updaterInstallerErrorCode;

	ConnectionHandler *connectionHandler;
	QDockWidget *mainDock;
};

DPointeredNoCopy(MainWindow)

MainWindow::MainWindow(QApplication* application, int argc, char** argv)
{
	d->autoUpdater = NULL;
	d->mainDock = NULL;
	d->connectionHandler = NULL;
	d->updateChannelOnUpdateStart = new UpdateChannel();
	d->updaterInstallerErrorCode = 0;

	d->application = application;

	this->setAttribute(Qt::WA_DeleteOnClose, true);
	d->setupUi(this);
	setupIcons();

	initAutoUpdaterWidgets();

	d->updatesConfirmationWidget->hide();
	d->updatesDownloadedWidget->hide();

	// Hide menu options which aren't supported on target platform.
	#ifndef WITH_AUTOUPDATES
		d->menuActionCheckForUpdates->setVisible(false);
	#endif

	if (gPlugins->numPlugins() == 0)
	{
		QString message = tr(
			"Warning: \n"
			"Doomseeker failed to detect any plugins.\n"
			"While the core application will still work its functionality is going to be limited.\n"
			"One of the proper locations for plugin modules is the \"engines/\" directory.\n"
		);
		gLog << message;
		QMessageBox::critical(NULL, tr("Doomseeker - plugin load failure"), message);
	}

	initIP2CUpdater();

	// The buddies list must always be available so we can perform certain operations on it
	d->buddiesList = new DockBuddiesList(this);
	d->menuView->addAction(d->buddiesList->toggleViewAction());
	d->buddiesList->toggleViewAction()->setText(tr("&Buddies"));
	d->buddiesList->toggleViewAction()->setShortcut(tr("CTRL+B"));

	connect(d->buddiesList, SIGNAL(joinServer(ServerPtr)), this, SLOT(runGame(ServerPtr)));
	d->buddiesList->hide();
	this->addDockWidget(Qt::LeftDockWidgetArea, d->buddiesList);
	initLogDock();
	initIRCDock();
	initServerFilterDock();
	initMainDock();
	splitDockWidget(d->mainDock, d->serverFilterDock, Qt::Horizontal);

	// Spawn Server Table Handler.
	d->serverTableHandler = new ServerListHandler(d->tableServers, this);
	connectEntities();

	d->broadcastManager = new BroadcastManager(this);
	d->serverTableHandler->connect(d->broadcastManager,
		SIGNAL(newServerDetected(ServerPtr, int)), SLOT(serverUpdated(ServerPtr, int)));
	d->serverTableHandler->connect(d->broadcastManager,
		SIGNAL(newServerDetected(ServerPtr, int)), SLOT(registerServer(ServerPtr)));
	d->serverTableHandler->connect(d->broadcastManager,
		SIGNAL(serverLost(ServerPtr)), SLOT(deregisterServer(ServerPtr)));

	initServerDetailsDock();
	tabifyDockWidget(d->ircDock, d->detailsDock);

	// Restore checked states.
	d->menuActionRecordDemo->setChecked(gConfig.doomseeker.bRecordDemo);

	// Get the master
	d->masterManager = new MasterManager();
	d->buddiesList->scan(d->masterManager);
	connect(d->masterManager, SIGNAL( masterMessage(MasterClient*, const QString&, const QString&, bool) ),
		this, SLOT( masterManagerMessages(MasterClient*, const QString&, const QString&, bool) ) );
	connect(d->masterManager, SIGNAL( masterMessageImportant(MasterClient*, const Message&) ),
		this, SLOT( masterManagerMessagesImportant(MasterClient*, const Message&) ));

	// Allow us to enable and disable ports.
	fillQueryMenu(d->masterManager);

	// Init custom servers
	d->masterManager->customServs()->readConfig(d->serverTableHandler,
		SLOT(serverUpdated(ServerPtr, int)),
		SLOT(serverBegunRefreshing(ServerPtr)) );

	setWindowIcon(QIcon(":/icon.png"));

	// Auto refresh timer
	initAutoRefreshTimer();
	connect(&d->autoRefreshTimer, SIGNAL( timeout() ), this, SLOT( autoRefreshTimer_timeout() ));

	// Tray icon
	initTrayIcon();

	setupToolBar();

	// Player diagram styles
	int slotStyle = gConfig.doomseeker.slotStyle;
	PlayersDiagram::loadImages(slotStyle);

	// IP2C
	d->ip2cLoader = new IP2CLoader();
	connectIP2CLoader(d->ip2cLoader);
	d->ip2cLoader->load();

	restoreState(QByteArray::fromBase64(gConfig.doomseeker.mainWindowState.toUtf8()));
	restoreGeometry(gConfig.doomseeker.mainWindowGeometry);

	// Start first refresh from a timer. We want the main window fully
	// set up before refresh.
	QTimer::singleShot(1, this, SLOT(postInitAppStartup()));
}

MainWindow::~MainWindow()
{
	// Window geometry settings
	gConfig.doomseeker.mainWindowGeometry = saveGeometry();
	gConfig.doomseeker.mainWindowState = saveState().toBase64();

	if (d->updateChannelOnUpdateStart != NULL)
	{
		delete d->updateChannelOnUpdateStart;
	}
	if (d->autoUpdater != NULL)
	{
		d->autoUpdater->disconnect();
		delete d->autoUpdater;
	}
	if(d->connectionHandler)
		delete d->connectionHandler;

	QList<QAction*> menuQueryActions = d->menuQuery->actions();
	QList<QAction*>::iterator it;
	foreach (QAction *action, menuQueryActions)
	{
		QString pluginName = action->text();

		if (!pluginName.isEmpty())
		{
			IniSection pluginConfig = gConfig.iniSectionForPlugin(pluginName);
			pluginConfig["Query"] = action->isChecked();
		}
	}

	if (d->trayIcon != NULL)
	{
		d->trayIcon->setVisible(false);
		delete d->trayIcon;
		d->trayIcon = NULL;
	}

	if (d->trayIconMenu != NULL)
	{
		delete d->trayIconMenu;
		d->trayIconMenu = NULL;
	}

	delete d->serverTableHandler;

	if(d->masterManager != NULL)
	{
		delete d->masterManager;
	}

	if (d->ip2cLoader != NULL)
	{
		delete d->ip2cLoader;
	}
}

void MainWindow::abortAutoUpdater()
{
	if (d->autoUpdater != NULL)
	{
		d->autoUpdater->abort();
	}
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
	d->toolBarGetServers->setEnabled(false);
}

DockBuddiesList *MainWindow::buddiesList()
{
	return d->buddiesList;
}

void MainWindow::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::ActivationChange && isActiveWindow() && !isMinimized() && !isHidden())
	{
		d->serverTableHandler->cleanUp();
		event->accept();
	}
	QMainWindow::changeEvent(event);
}

void MainWindow::checkForUpdates(bool bUserTriggered)
{
	if (d->autoUpdater != NULL)
	{
		if (d->autoUpdater->isRunning())
		{
			QMessageBox::warning(this, tr("Doomseeker - Auto Update"),
				tr("Update is already in progress."));
			return;
		}
		else
		{
			delete d->autoUpdater;
			d->autoUpdater = NULL;
		}
	}
	gLog << tr("Removing old update packages from local temporary space.");
	QStringList removeFilter(QString("%1*").arg(DataPaths::UPDATE_PACKAGE_FILENAME_PREFIX));
	FileUtils::rmAllFiles(DoomseekerFilePaths::updatePackagesStorageDir(),
		removeFilter);

	showAndLogStatusMessage(tr("Checking for updates..."));
	d->autoUpdater = new AutoUpdater();
	this->connect(d->autoUpdater, SIGNAL(statusMessage(QString)),
		SLOT(showAndLogStatusMessage(QString)));
	this->connect(d->autoUpdater, SIGNAL(finished()),
		SLOT(onAutoUpdaterFinish()));
	this->connect(d->autoUpdater, SIGNAL(downloadAndInstallConfirmationRequested()),
		SLOT(onAutoUpdaterDownloadAndInstallConfirmationRequest()));
	this->connect(d->autoUpdater, SIGNAL(overallProgress(int, int, const QString&)),
		SLOT(onAutoUpdaterOverallProgress(int, int, const QString&)));
	this->connect(d->autoUpdater, SIGNAL(packageDownloadProgress(qint64, qint64)),
		SLOT(onAutoUpdaterFileProgress(qint64, qint64)));

	QMap<QString, QList<QString> > ignoredPackagesRevisions;
	if (!bUserTriggered)
	{
		foreach (const QString& package, gConfig.autoUpdates.lastKnownUpdateRevisions.keys())
		{
			QString revision = gConfig.autoUpdates.lastKnownUpdateRevisions[package];
			QList<QString> list;
			list << revision;
			ignoredPackagesRevisions.insert(package, list);
		}
	}
	d->autoUpdater->setIgnoreRevisions(ignoredPackagesRevisions);

	UpdateChannel channel = UpdateChannel::fromName(gConfig.autoUpdates.updateChannelName);
	d->autoUpdater->setChannel(channel);
	*d->updateChannelOnUpdateStart = channel;

	bool bRequireConfirmation = true;
	if (!bUserTriggered)
	{
		bRequireConfirmation = (gConfig.autoUpdates.updateMode
			!= DoomseekerConfig::AutoUpdates::UM_FullAuto);
	}
	d->autoUpdater->setRequireDownloadAndInstallConfirmation(bRequireConfirmation);
	d->autoUpdaterStatusBarWidget->show();
	d->autoUpdater->start();
}


void MainWindow::checkForUpdatesAuto()
{
	const bool bUserTriggered = true;
	checkForUpdates(!bUserTriggered);
}

void MainWindow::checkForUpdatesUserTriggered()
{
	const bool bUserTriggered = true;
	checkForUpdates(bUserTriggered);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	// Check if tray icon is available and if we want to minimize to tray icon
	// when 'X' button is pressed. Real quit requests are handled by
	// quitProgram() method. This method sets d->bWantToQuit to true.
	if (d->trayIcon != NULL && gConfig.doomseeker.bCloseToTrayIcon && !d->bWantToQuit)
	{
		d->bWasMaximized = isMaximized();
		event->ignore();
		hide();
	}
	else
	{
		event->accept();
	}
}

void MainWindow::confirmUpdateInstallation()
{
	assert(d->autoUpdater != NULL && "MainWindow::confirmUpdateInstallation()");
	d->updatesConfirmationWidget->hide();
	d->autoUpdater->confirmDownloadAndInstall();
}

void MainWindow::connectIP2CLoader(IP2CLoader* loader)
{
	this->connect(d->ip2cLoader, SIGNAL( finished() ), SLOT( ip2cJobsFinished()));
	this->connect(d->ip2cLoader, SIGNAL( downloadProgress(qint64, qint64) ),
		SLOT( ip2cDownloadProgress(qint64, qint64) ) );
}

void MainWindow::discardUpdates()
{
	assert(d->autoUpdater != NULL && "MainWindow::confirmUpdateInstallation()");
	d->updatesConfirmationWidget->hide();
	// User rejected this update so let's add the packages
	// to the ignore list so user won't be nagged again.
	const QList<UpdatePackage>& pkgList = d->autoUpdater->newUpdatePackages();
	foreach (const UpdatePackage& pkg, pkgList)
	{
		gConfig.autoUpdates.lastKnownUpdateRevisions.insert(pkg.name, pkg.revision);
	}
	d->autoUpdater->abort();
}

void MainWindow::connectEntities()
{
	// Connect refreshing thread.
	connect(gRefresher, SIGNAL( block() ), this, SLOT( blockRefreshButtons() ) );
	connect(gRefresher, SIGNAL( finishedQueryingMaster(MasterClient*) ), this, SLOT( finishedQueryingMaster(MasterClient*) ) );
	connect(gRefresher, SIGNAL( sleepingModeEnter() ), this, SLOT( refreshThreadEndsWork() ) );
	connect(gRefresher, SIGNAL( sleepingModeEnter() ), d->buddiesList, SLOT( scan() ) );
	connect(gRefresher, SIGNAL( sleepingModeExit() ), this, SLOT( refreshThreadBeginsWork() ) );

	// Controls
	connect(d->menuActionAbout, SIGNAL( triggered() ), this, SLOT( menuHelpAbout() ));
	connect(d->menuActionAboutQt, SIGNAL( triggered() ), d->application, SLOT( aboutQt() ));
	connect(d->menuActionBuddies, SIGNAL( triggered() ), this, SLOT( menuBuddies() ));
	connect(d->menuActionConfigure, SIGNAL( triggered() ), this, SLOT( menuOptionsConfigure() ));
	connect(d->menuActionCreateServer, SIGNAL( triggered() ), this, SLOT( menuCreateServer() ));
	connect(d->menuActionHelp, SIGNAL( triggered() ), this, SLOT ( menuHelpHelp() ) );
	connect(d->menuActionIRCOptions, SIGNAL( triggered() ), this, SLOT( menuIRCOptions() ) );
	connect(d->menuActionLog, SIGNAL( triggered() ), this, SLOT( menuLog() ));
	connect(d->menuActionManageDemos, SIGNAL( triggered() ), this, SLOT( menuManageDemos() ) );
	connect(d->menuActionRecordDemo, SIGNAL( triggered() ), this, SLOT( menuRecordDemo() ) );
	connect(d->menuActionUpdateIP2C, SIGNAL( triggered() ), this, SLOT( menuUpdateIP2C() ) );
	connect(d->menuActionQuit, SIGNAL( triggered() ), this, SLOT( quitProgram() ));
	connect(d->menuActionViewIRC, SIGNAL( triggered() ) , this, SLOT( menuViewIRC() ));
	connect(d->menuActionWadseeker, SIGNAL( triggered() ), this, SLOT( menuWadSeeker() ));
	connect(d->serverFilterDock, SIGNAL(filterUpdated(const ServerListFilterInfo&)),
		this, SLOT(updateServerFilter(const ServerListFilterInfo&)) );
	connect(d->serverFilterDock, SIGNAL(nonEmptyServerGroupingAtTopToggled(bool)),
		d->serverTableHandler, SLOT(setGroupServersWithPlayersAtTop(bool)) );
	connect(d->serverTableHandler, SIGNAL(serverFilterModified(ServerListFilterInfo)),
		d->serverFilterDock, SLOT(setFilterInfo(ServerListFilterInfo)));
	connect(d->serverTableHandler, SIGNAL( serverDoubleClicked(ServerPtr) ), this, SLOT( runGame(ServerPtr) ) );
	connect(d->serverTableHandler, SIGNAL( displayServerJoinCommandLine(const ServerPtr&) ), this, SLOT( showServerJoinCommandLine(const ServerPtr&) ) );
	connect(d->serverTableHandler, SIGNAL( findMissingWADs(const ServerPtr&) ), this, SLOT( findMissingWADs(const ServerPtr&) ) );
	connect(d->serverTableHandler, SIGNAL( serverInfoUpdated(ServerPtr) ), this, SLOT( serverAddedToList(ServerPtr) ) );
	connect(d->buddiesList, SIGNAL(scanCompleted()), d->serverTableHandler, SLOT(redraw()));
}

void MainWindow::fillQueryMenu(MasterManager* masterManager)
{
	// This is called only once from the constructor. No clears to
	// d->queryMenuPorts are ever performed. Not even in the destructor.
	for(unsigned i = 0; i < gPlugins->numPlugins(); ++i)
	{
		const EnginePlugin* plugin = gPlugins->info(i);
		if(!plugin->data()->hasMasterClient() && !plugin->data()->hasBroadcast())
		{
			continue;
		}

		if (plugin->data()->hasMasterClient())
		{
			MasterClient* pMasterClient = plugin->data()->masterClient;
			pMasterClient->updateAddress();
			masterManager->addMaster(pMasterClient);
		}

		if (plugin->data()->hasBroadcast())
		{
			d->broadcastManager->registerPlugin(plugin);
			d->serverTableHandler->connect(plugin->data()->broadcast,
				SIGNAL(serverLost(ServerPtr)), SLOT(removeServer(ServerPtr)));
		}

		// Now is a good time to also populate the status bar widgets
		ServersStatusWidget *statusWidget = new ServersStatusWidget(plugin);
		d->serversStatusesWidgets.insert(plugin, statusWidget);

		this->connect(statusWidget, SIGNAL( clicked(const EnginePlugin*) ) ,
			SLOT( togglePluginQueryEnabled(const EnginePlugin*) ) );
		statusWidget->connect(d->broadcastManager,
			SIGNAL(newServerDetected(ServerPtr, int)),
			SLOT(registerServerIfSamePlugin(ServerPtr)));
		statusWidget->connect(d->broadcastManager,
			SIGNAL(serverLost(ServerPtr)), SLOT(deregisterServer(ServerPtr)));


		statusBar()->addPermanentWidget(statusWidget);

		QString name = gPlugins->info(i)->data()->name;
		QueryMenuAction* query = new QueryMenuAction(plugin, statusWidget, d->menuQuery);
		d->queryMenuPorts.insert(plugin, query);

		d->menuQuery->addAction(query);

		query->setCheckable(true);
		query->setIcon(plugin->icon());
		query->setText(name);

		IniSection pluginConfig = gConfig.iniSectionForPlugin(name);

		if (!pluginConfig.retrieveSetting("Query").value().isNull())
		{
			bool enabled = pluginConfig["Query"];
			setQueryPluginEnabled(plugin, enabled);
		}
		else
		{
			// if no setting is found for this engine
			// set default to true:
			setQueryPluginEnabled(plugin, true);
		}
	}
}

void MainWindow::findMissingWADs(const ServerPtr &server)
{
	// Display a message if all WADs are present.
	bool allfound = true;
	QList<PWad> wads = server->wads();
	PathFinder pathFinder = server->wadPathFinder();
	WadPathFinder wadFinder(pathFinder);
	foreach(const PWad &wad, wads)
	{
		if(!wadFinder.find(wad.name()).isValid())
		{
			allfound = false;
			break;
		}
	}
	if(allfound)
	{
		QMessageBox::information(this, tr("All WADs found"), tr("All of the WADs used by this server are present."));
		return;
	}

	CommandLineInfo cli;
	ConnectionHandler connectionHandler(server, this);
	// Use a command line builder to trigger missing wads dialog.
	JoinCommandLineBuilder *builder = new JoinCommandLineBuilder(server, GameDemo::NoDemo, this);
	builder->setRequireOptionals(true);
	builder->setPasswords("", "");
	builder->obtainJoinCommandLine();
}

void MainWindow::finishConfiguration(DoomseekerConfigurationDialog &configDialog, bool lookupHostsChanged)
{
	// In case the master server addresses changed, notify the master clients.
	updateMasterAddresses();
	gRefresher->setDelayBetweenResends(gConfig.doomseeker.querySpeed().delayBetweenSingleServerAttempts);

	// If appearance changed - update the widgets.
	if (configDialog.appearanceChanged())
	{
		d->tableServers->setShowGrid(gConfig.doomseeker.bDrawGridInServerTable);
		d->serverTableHandler->redraw();
		initTrayIcon();
	}

	// Do the following only if setting changed from false to true.
	if (lookupHostsChanged)
	{
		d->serverTableHandler->lookupHosts();
	}

	// Refresh custom servers list:
	if (configDialog.customServersChanged())
	{
		d->serverTableHandler->serverModel()->removeCustomServers();
		d->masterManager->customServs()->readConfig(d->serverTableHandler, SLOT(serverUpdated(ServerPtr, int)), SLOT(serverBegunRefreshing(ServerPtr)) );
		refreshCustomServers();
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
		connect((*master)[i].data(), SIGNAL(updated(ServerPtr, int)),
			d->serverTableHandler, SLOT(serverUpdated(ServerPtr, int)) );

		connect((*master)[i].data(), SIGNAL(begunRefreshing(ServerPtr)),
			d->serverTableHandler, SLOT(serverBegunRefreshing(ServerPtr)) );
	}
}

void MainWindow::getServers()
{
	// Check if this operation has any sense.
	if (!isAnythingToRefresh())
	{
		gLog << tr("Senseless refresh operation attempted.");

		QString message = tr("Doomseeker is unable to proceed with the refresh"
			" operation because the following problem has occured:\n\n");

		if (gPlugins->numPlugins() == 0)
		{
			message += tr("Plugins are missing from the \"engines/\" directory.");
		}
		else if (!isAnyMasterEnabled())
		{
			message += tr("No master servers are enabled in the \"Query\" menu.");
		}
		else
		{
			message += tr("Unknown error occured.");
		}

		gLog << message;
		QMessageBox::warning(this, tr("Doomseeker - senseless operation"), message);
		return;
	}

	d->bTotalRefreshInProcess = true;
	d->autoRefreshTimer.stop();
	gLog << tr("Total refresh process initialized!");
	d->serverTableHandler->clearTable();
	refreshCustomServers();
	refreshLanServers();

	bool bAtLeastOneEnabled = false;

	if (!isAnyMasterEnabled())
	{
		gLog << tr("Warning: No master servers were enabled for this refresh. "
			"Check your Query menu or \"engines/\" directory. Custom servers will still refresh.");
	}

	d->masterManager->clearServers();
	for (int i = 0; i < d->masterManager->numMasters(); ++i)
	{
		MasterClient* pMaster = (*d->masterManager)[i];

		if (pMaster->isEnabled())
		{
			gRefresher->registerMaster(pMaster);
		}
	}
}

bool MainWindow::hasCustomServers() const
{
	CustomServers* customServers = d->masterManager->customServs();
	return customServers->numServers() > 0;
}

void MainWindow::initAutoRefreshTimer()
{
	const unsigned MIN_DELAY = 30;
	const unsigned MAX_DELAY = 3600;

	bool bEnabled = gConfig.doomseeker.bQueryAutoRefreshEnabled;

	if (!bEnabled)
	{
		d->autoRefreshTimer.stop();
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

		d->autoRefreshTimer.setSingleShot(false);
		d->autoRefreshTimer.start(delayMs);
	}
}

void MainWindow::initAutoUpdaterWidgets()
{
	static const int FILE_BAR_WIDTH = 50;
	static const int OVERALL_BAR_WIDTH = 180;

	d->autoUpdaterStatusBarWidget = new QWidget(statusBar());
	d->autoUpdaterStatusBarWidget->setLayout(new QHBoxLayout(d->autoUpdaterStatusBarWidget));
	d->autoUpdaterStatusBarWidget->layout()->setContentsMargins(QMargins(0, 0, 0, 0));
	statusBar()->addPermanentWidget(d->autoUpdaterStatusBarWidget);
	d->autoUpdaterStatusBarWidget->hide();

	d->autoUpdaterLabel = new QLabel(d->autoUpdaterStatusBarWidget);
	d->autoUpdaterLabel->setText(tr("Auto Updater:"));
	d->autoUpdaterStatusBarWidget->layout()->addWidget(d->autoUpdaterLabel);

	d->autoUpdaterFileProgressBar = mkStdProgressBarForStatusBar();
	d->autoUpdaterFileProgressBar->setFormat("%p%");
	d->autoUpdaterFileProgressBar->setMaximumWidth(FILE_BAR_WIDTH);
	d->autoUpdaterFileProgressBar->setMinimumWidth(FILE_BAR_WIDTH);
	d->autoUpdaterStatusBarWidget->layout()->addWidget(d->autoUpdaterFileProgressBar);

	d->autoUpdaterOverallProgressBar = mkStdProgressBarForStatusBar();
	d->autoUpdaterOverallProgressBar->setMaximumWidth(OVERALL_BAR_WIDTH);
	d->autoUpdaterOverallProgressBar->setMinimumWidth(OVERALL_BAR_WIDTH);
	d->autoUpdaterStatusBarWidget->layout()->addWidget(d->autoUpdaterOverallProgressBar);

	d->autoUpdaterAbortButton = new QPushButton(statusBar());
	d->autoUpdaterAbortButton->setToolTip(tr("Abort update."));
	d->autoUpdaterAbortButton->setIcon(QIcon(":/icons/x.png"));
	this->connect(d->autoUpdaterAbortButton, SIGNAL(clicked()),
		SLOT(abortAutoUpdater()));
	d->autoUpdaterStatusBarWidget->layout()->addWidget(d->autoUpdaterAbortButton);
}

void MainWindow::initIP2CUpdater()
{
	static const int PROGRESSBAR_WIDTH = 220;

	d->ip2cUpdateProgressBar = mkStdProgressBarForStatusBar();
	d->ip2cUpdateProgressBar->setFormat(tr("IP2C Update"));
	d->ip2cUpdateProgressBar->hide();
	d->ip2cUpdateProgressBar->setMaximumWidth(PROGRESSBAR_WIDTH);
	d->ip2cUpdateProgressBar->setMinimumWidth(PROGRESSBAR_WIDTH);
	statusBar()->addPermanentWidget(d->ip2cUpdateProgressBar);
}

void MainWindow::initIRCDock()
{
	d->ircDock = new IRCDock(this);
	d->menuView->addAction(d->ircDock->toggleViewAction());
	d->ircDock->toggleViewAction()->setText(tr("&IRC"));
	d->ircDock->toggleViewAction()->setShortcut(tr("CTRL+I"));
	d->ircDock->hide();
	this->addDockWidget(Qt::BottomDockWidgetArea, d->ircDock);

	if (ChatNetworksCfg().isAnyNetworkOnAutoJoin())
	{
		this->d->ircDock->setVisible(true);
		this->d->ircDock->performNetworkAutojoins();
	}
}

void MainWindow::initLogDock()
{
	d->logDock = new LogDock(this);
	d->menuView->addAction(d->logDock->toggleViewAction());
	d->logDock->toggleViewAction()->setText(tr("&Log"));
	d->logDock->toggleViewAction()->setShortcut(tr("CTRL+L"));
	d->logDock->hide();
	this->addDockWidget(Qt::BottomDockWidgetArea, d->logDock);

	connect(&gLog, SIGNAL( newEntry(const QString&) ), d->logDock, SLOT( appendLogEntry(const QString&) ) );

	// Also add anything that already might be in the log to the box.
	d->logDock->appendLogEntry(gLog.content());
}

void MainWindow::initMainDock()
{
	setDockNestingEnabled(true); // This line allows us to essentually treat a dock as a central widget.

	// Make a dock out of the central MainWindow widget and drop that widget
	// from the MainWindow itself.
	d->mainDock = new QDockWidget(tr("Servers"));
	d->mainDock->setTitleBarWidget(new QWidget(this));
	d->mainDock->setObjectName("ServerList");
	d->mainDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	d->mainDock->setWidget(centralWidget());
	setCentralWidget(0);
	addDockWidget(Qt::RightDockWidgetArea, d->mainDock);
}

void MainWindow::initServerDetailsDock()
{
	d->detailsDock = new ServerDetailsDock(this);
	d->menuView->addAction(d->detailsDock->toggleViewAction());
	d->detailsDock->toggleViewAction()->setText(tr("Server &Details"));
	d->detailsDock->toggleViewAction()->setShortcut(tr("CTRL+D"));
	d->detailsDock->hide();
	addDockWidget(Qt::BottomDockWidgetArea, d->detailsDock);

	d->detailsDock->connect(d->serverTableHandler, SIGNAL( serversSelected(QList<ServerPtr>&) ), SLOT( displaySelection(QList<ServerPtr> &) ));
}

void MainWindow::initServerFilterDock()
{
	d->serverFilterDock = new ServerFilterDock(this);

	d->menuView->addAction(d->serverFilterDock->toggleViewAction());
	d->serverFilterDock->hide();
	this->addDockWidget(Qt::RightDockWidgetArea, d->serverFilterDock);
}

void MainWindow::initTrayIcon()
{
	bool isEnabled = gConfig.doomseeker.bUseTrayIcon;
	if (!isEnabled || !QSystemTrayIcon::isSystemTrayAvailable())
	{
		if (d->trayIcon != NULL)
		{
			delete d->trayIcon;
			d->trayIcon = NULL;
		}

		if (d->trayIconMenu != NULL)
		{
			delete d->trayIconMenu;
			d->trayIconMenu = NULL;
		}
	}
	else if (d->trayIcon == NULL)
	{
		QAction* trayAction;
		d->trayIconMenu = new QMenu(this);
		trayAction = d->trayIconMenu->addAction("Exit");
		connect(trayAction, SIGNAL( triggered() ), this, SLOT( quitProgram() ) );

		// This should be automatically deleted when main window closes
		d->trayIcon = new QSystemTrayIcon(this);
		connect(d->trayIcon, SIGNAL( activated(QSystemTrayIcon::ActivationReason) ), this, SLOT( trayIcon_activated(QSystemTrayIcon::ActivationReason) ) );

		updateTrayIconTooltipAndLogTotalRefresh();

		d->trayIcon->setContextMenu(d->trayIconMenu);
		d->trayIcon->setIcon(QIcon(":/icon.png"));
		d->trayIcon->setVisible(true);
	}
}

void MainWindow::ip2cDownloadProgress(qint64 current, qint64 max)
{
	d->ip2cUpdateProgressBar->setMaximum(max);
	d->ip2cUpdateProgressBar->setValue(current);
}

void MainWindow::ip2cJobsFinished()
{
	d->menuActionUpdateIP2C->setEnabled(true);
	d->serverTableHandler->updateCountryFlags();
	d->ip2cUpdateProgressBar->hide();

	if (d->ip2cLoader != NULL)
	{
		delete d->ip2cLoader;
		d->ip2cLoader = NULL;
	}
}

void MainWindow::ip2cStartUpdate()
{
	if (d->ip2cLoader != NULL)
	{
		// If update is currently in progress then prevent re-starting.
		return;
	}

	d->menuActionUpdateIP2C->setEnabled(false);
	d->ip2cUpdateProgressBar->show();

	d->ip2cLoader = new IP2CLoader();
	connectIP2CLoader(d->ip2cLoader);
	d->ip2cLoader->update();
}

bool MainWindow::isAnythingToRefresh() const
{
	return hasCustomServers() || isAnyMasterEnabled();
}

bool MainWindow::isAnyMasterEnabled() const
{
	for (int i = 0; i < d->masterManager->numMasters(); ++i)
	{
		MasterClient* pMaster = (*d->masterManager)[i];

		if (pMaster->isEnabled())
		{
			return true;
		}
	}

	return false;
}

void MainWindow::masterManagerMessages(MasterClient* pSender, const QString& title, const QString& content, bool isError)
{
	QString message = tr("Master server for %1: %2").arg(pSender->plugin()->data()->name).arg(content);

	if (isError)
	{
		message = tr("Error: %1").arg(message);
		statusBar()->showMessage(message);
	}

	gLog << message;
}

void MainWindow::masterManagerMessagesImportant(MasterClient* pSender, const Message& objMessage)
{
	QString strFullMessage = tr("%1: %2")
		.arg(pSender->plugin()->data()->name)
		.arg(objMessage.contents());
	d->importantMessagesWidget->addMessage(strFullMessage, objMessage.timestamp());
}

void MainWindow::menuBuddies()
{
	d->buddiesList->setVisible(!d->buddiesList->isVisible());
}

void MainWindow::menuCreateServer()
{
	// This object will auto-delete on close.
	CreateServerDialog* dialog = new CreateServerDialog(NULL);
	dialog->setWindowIcon(this->windowIcon());
	dialog->show();
}

void MainWindow::menuHelpAbout()
{
	AboutDialog dlg(this);
	d->autoRefreshTimer.stop();
	dlg.exec();
	initAutoRefreshTimer();
}

void MainWindow::menuHelpHelp()
{
	if (HELP_SITE_URL.isEmpty() || !Strings::isUrlSafe(HELP_SITE_URL))
	{
		QMessageBox::critical(this, tr("Help error"), tr("No help found."), QMessageBox::Ok, QMessageBox::Ok);
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

	if (d->ircDock != NULL)
	{
		d->ircDock->applyAppearanceSettings();

		// This could probably be optimized to not re-read files from drive
		// if audio options didn't change but currently there are only two
		// files, so no harm should be done.
		d->ircDock->sounds().loadFromConfig();
	}
}

void MainWindow::menuLog()
{
	d->logDock->setVisible(!d->logDock->isVisible());
}

void MainWindow::menuManageDemos()
{
	DemoManagerDlg dm;
	dm.exec();
}

void MainWindow::menuOptionsConfigure()
{
	DoomseekerConfigurationDialog::openConfiguration();
}

void MainWindow::menuRecordDemo()
{
	gConfig.doomseeker.bRecordDemo = d->menuActionRecordDemo->isChecked();
}

void MainWindow::menuUpdateIP2C()
{
	IP2CUpdateBox updateBox(this);

	connect(&updateBox, SIGNAL( accepted() ), this, SLOT( ip2cStartUpdate() ) );

	updateBox.exec();
}

void MainWindow::menuViewIRC()
{
	d->ircDock->setVisible(!d->ircDock->isVisible());
}

void MainWindow::menuWadSeeker()
{
	if (gWadseekerShow->checkWadseekerValidity(this))
	{
		WadseekerInterface *wadseeker = WadseekerInterface::create(NULL);
		wadseeker->setAttribute(Qt::WA_DeleteOnClose);
		wadseeker->show();
	}
}

QProgressBar* MainWindow::mkStdProgressBarForStatusBar()
{
	QProgressBar* pBar = new QProgressBar(statusBar());
	pBar->setAlignment(Qt::AlignCenter);
	pBar->setTextVisible(true);
	pBar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	return pBar;
}

void MainWindow::notifyFirstRun()
{
	// On first run prompt configuration box.
	QMessageBox::information(NULL, tr("Welcome to Doomseeker"),
		tr("Before you start browsing for servers, please ensure that Doomseeker is properly configured."));
	d->menuActionConfigure->trigger();
}

void MainWindow::onAutoUpdaterDownloadAndInstallConfirmationRequest()
{
	d->updatesConfirmationWidget->show();
}

void MainWindow::onAutoUpdaterFileProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	d->autoUpdaterFileProgressBar->setValue(bytesReceived);
	d->autoUpdaterFileProgressBar->setMaximum(bytesTotal);
}

void MainWindow::onAutoUpdaterFinish()
{
	showAndLogStatusMessage(tr("Program update detection & download finished with status: [%1] %2")
		.arg((int)d->autoUpdater->errorCode()).arg(d->autoUpdater->errorString()));
	gConfig.autoUpdates.bPerformUpdateOnNextRun = false;
	if (d->autoUpdater->errorCode() == AutoUpdater::EC_Ok)
	{
		UpdateChannel channel = UpdateChannel::fromName(gConfig.autoUpdates.updateChannelName);
		if (channel == *d->updateChannelOnUpdateStart)
		{
			if (!d->autoUpdater->downloadedPackagesFilenames().isEmpty())
			{
				gLog << tr("Updates will be installed on next program start.");
				d->updatesDownloadedWidget->show();
				gConfig.autoUpdates.bPerformUpdateOnNextRun = true;
			}
		}
		else
		{
			gLog << tr("Update channel was changed during update process. Discarding update.");
		}
	}
	gConfig.saveToFile();
	d->autoUpdaterStatusBarWidget->hide();
	d->updatesConfirmationWidget->hide();
	d->autoUpdater->deleteLater();
	d->autoUpdater = NULL;
}

void MainWindow::onAutoUpdaterOverallProgress(int current, int total,
	const QString& msg)
{
	d->autoUpdaterOverallProgressBar->setValue(current);
	d->autoUpdaterOverallProgressBar->setMaximum(total);
	d->autoUpdaterOverallProgressBar->setFormat(msg);
}

void MainWindow::postInitAppStartup()
{
	// Load server filter from config.
	d->serverFilterDock->setFilterInfo(gConfig.serverFilter.info);

	// Check query on statup
	// Let's see if we have any plugins first. If not, display error.
	if (gPlugins->numPlugins() > 0)
	{
		bool bGettingServers = false;
		bool queryOnStartup = gConfig.doomseeker.bQueryOnStartup;
		if (queryOnStartup)
		{
			// If "Query on startup" option is enabled we should
			// attempt to refresh any masters that are enabled
			// in the Query menu.

			if (isAnyMasterEnabled())
			{
				bGettingServers = true;
				getServers();
			}
			else
			{
				gLog << tr("Query on startup warning: No master servers are enabled in the Query menu.");
			}
		}

		// If we already successfully called the getServers() method
		// there is no need to call refreshCustomsServers().
		if (!bGettingServers && hasCustomServers())
		{
			// Custom servers should be refreshed no matter what.
			// They will not block the app in any way, there is no reason
			// not to refresh them.
			refreshCustomServers();
		}
	}
	else
	{
		// There are no plugins so something is really bad.
		// Display error message.
		QMessageBox::critical(NULL, tr("Doomseeker critical error"),
			tr("Doomseeker was unable to find any plugin libraries.\n"
				"Although the application will still work it will not be possible "
				"to fetch any server info or launch any game.\n\n"
				"Please check if there are any files in \"engines/\" directory.\n"
				"To fix this problem you may try downloading Doomseeker "
				"again from the site specified in the Help|About box and reinstalling "
				"Doomseeker."));
	}
}

QueryMenuAction* MainWindow::queryMenuActionForPlugin(const EnginePlugin* plugin)
{
	if (plugin == NULL)
	{
		return NULL;
	}

	if (d->queryMenuPorts.contains(plugin))
	{
		return d->queryMenuPorts[plugin];
	}

	return NULL;
}

void MainWindow::quitProgram()
{
	d->bWantToQuit = true;
	QApplication::closeAllWindows();
}

void MainWindow::refreshCustomServers()
{
	CustomServers* customServers = d->masterManager->customServs();

	for(int i = 0;i < customServers->numServers();i++)
	{
		ServerPtr server = (*customServers)[i];
		d->serverTableHandler->serverUpdated(server, Server::RESPONSE_NO_RESPONSE_YET);
		gRefresher->registerServer(server.data());
	}
}

void MainWindow::refreshLanServers()
{
	foreach (ServerPtr server, d->broadcastManager->servers())
	{
		d->serverTableHandler->serverUpdated(server, server->lastResponse());
		gRefresher->registerServer(server.data());
	}
}

void MainWindow::refreshThreadBeginsWork()
{
	// disable refresh.
	d->serverTableHandler->serverTable()->setAllowAllRowsRefresh(false);
	statusBar()->showMessage(tr("Querying..."));
}

void MainWindow::refreshThreadEndsWork()
{
	d->toolBarGetServers->setEnabled(true);

	d->serverTableHandler->serverTable()->setAllowAllRowsRefresh(true);
	d->serverTableHandler->cleanUpForce();
	statusBar()->showMessage(tr("Done"));
	updateTrayIconTooltipAndLogTotalRefresh();

	if (d->bTotalRefreshInProcess)
	{
		initAutoRefreshTimer();
	}

	d->bTotalRefreshInProcess = false;
	QList<ServerPtr> selectedServers = d->serverTableHandler->selectedServers();
	d->detailsDock->displaySelection(selectedServers);
}

void MainWindow::restartAndInstallUpdatesNow()
{
	Main::bInstallUpdatesAndRestart = true;
	quitProgram();
}

void MainWindow::runGame(const ServerPtr &server)
{
	if(d->connectionHandler)
		delete d->connectionHandler;

	d->connectionHandler = new ConnectionHandler(server, this);
	d->connectionHandler->run();
}

void MainWindow::setQueryPluginEnabled(const EnginePlugin* plugin, bool bEnabled)
{
	assert(plugin != NULL);

	QueryMenuAction* pAction = queryMenuActionForPlugin(plugin);
	if (pAction != NULL)
	{
		pAction->setChecked(bEnabled);
		if (plugin->data()->hasMasterClient())
		{
			plugin->data()->masterClient->setEnabled(bEnabled);
		}
		if (plugin->data()->hasBroadcast())
		{
			plugin->data()->broadcast->setEnabled(bEnabled);
		}
		d->serversStatusesWidgets[plugin]->setMasterEnabledStatus(bEnabled);
	}
}

void MainWindow::serverAddedToList(const ServerPtr &pServer)
{
	if (pServer->isKnown())
	{
		const QString& gameMode = pServer->gameMode().name();
		d->serverFilterDock->addGameModeToComboBox(gameMode);
	}
}

void MainWindow::setDisplayUpdaterProcessFailure(int errorCode)
{
	assert(this->d->updaterInstallerErrorCode == 0 &&
		"MainWindow::setDisplayUpdaterProcessFailure()");
	this->d->updaterInstallerErrorCode = errorCode;
	QTimer::singleShot(0, this, SLOT(showUpdaterProcessErrorDialog()));
}

void MainWindow::setDisplayUpdateInstallerError(int errorCode)
{
	assert(this->d->updaterInstallerErrorCode == 0 &&
		"MainWindow::setDisplayUpdateInstallerError()");
	this->d->updaterInstallerErrorCode = errorCode;
	QTimer::singleShot(0, this, SLOT(showUpdateInstallErrorDialog()));
}

void MainWindow::setupIcons()
{
	QStyle& style = *QApplication::style();

	// File menu.
	d->menuActionQuit->setIcon(style.standardIcon(QStyle::SP_TitleBarCloseButton));

	// Help menu.
	d->menuActionHelp->setIcon(style.standardIcon(QStyle::SP_MessageBoxQuestion));
	d->menuActionAbout->setIcon(style.standardIcon(QStyle::SP_MessageBoxInformation));
}

void MainWindow::setupToolBar()
{
	QToolBar* pToolBar = new QToolBar(tr("Main Toolbar"), this);
	pToolBar->setMovable(false);
	pToolBar->setObjectName("Toolbar");

	// Refresh buttons
	d->toolBarGetServers = new QAction(QIcon(":/icons/refresh.png"), tr("Get Servers"), pToolBar);

	// Setup menu
	// Refresh buttons
	pToolBar->addAction(d->toolBarGetServers);

	// File menu buttons.
	pToolBar->addSeparator();
	pToolBar->addAction(d->menuActionCreateServer);
	pToolBar->addAction(d->menuActionWadseeker);

	// Demo buttons
	pToolBar->addSeparator();
	pToolBar->addAction(d->menuActionManageDemos);
	pToolBar->addAction(d->menuActionRecordDemo);

	pToolBar->addSeparator();

	// Dockable windows buttons.
	pToolBar->addAction(d->buddiesList->toggleViewAction());
	pToolBar->addAction(d->logDock->toggleViewAction());
	pToolBar->addAction(d->ircDock->toggleViewAction());
	pToolBar->addAction(d->serverFilterDock->toggleViewAction());
	pToolBar->addAction(d->detailsDock->toggleViewAction());

	// Quick Search
	QLineEdit *qs = d->serverFilterDock->createQuickSearch();
	qs->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	qs->setMinimumWidth(175);
	qs->setMaximumWidth(175);

	QWidget* searchSeparator = new QWidget();
	searchSeparator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
	pToolBar->addWidget(searchSeparator);
	pToolBar->addWidget(new QLabel(tr("Search:"), pToolBar));
	pToolBar->addWidget(qs);

	this->addToolBar(Qt::TopToolBarArea, pToolBar);
	setUnifiedTitleAndToolBarOnMac(true);
	connect(pToolBar, SIGNAL( actionTriggered(QAction*) ), this, SLOT( toolBarAction(QAction*) ) );
}

void MainWindow::showAndLogStatusMessage(const QString &message)
{
	gLog << message;
	statusBar()->showMessage(message);
}

void MainWindow::showProgramArgsHelp()
{
	ProgramArgsHelpDialog *dialog = new ProgramArgsHelpDialog(this);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	dialog->show();
}

void MainWindow::showServerJoinCommandLine(const ServerPtr &server)
{
	CommandLineInfo cli;
	ConnectionHandler connectionHandler(server, this);
	GameDemo demo = gConfig.doomseeker.bRecordDemo ? GameDemo::Unmanaged : GameDemo::NoDemo;
	JoinCommandLineBuilder *builder = new JoinCommandLineBuilder(server, demo, this);
	this->connect(builder, SIGNAL(commandLineBuildFinished()), SLOT(showServerJoinCommandLineOnBuilderFinished()));
	builder->obtainJoinCommandLine();
}

void MainWindow::showServerJoinCommandLineOnBuilderFinished()
{
	JoinCommandLineBuilder *builder = static_cast<JoinCommandLineBuilder*>(sender());
	CommandLineInfo cli = builder->builtCommandLine();
	if (cli.isValid())
	{
		QString execPath = cli.executable.absoluteFilePath();
		QStringList args = cli.args;

		CommandLine::escapeExecutable(execPath);
		CommandLine::escapeArgs(args);

		CopyTextDlg ctd(execPath + " " + args.join(" "), builder->server()->name(), this);
		ctd.exec();
	}
	else
	{
		if (!builder->error().isEmpty())
		{
			QMessageBox::critical(this, tr("Doomseeker - show join command line"),
				tr("Command line cannot be built:\n%1").arg(builder->error()));
		}
	}
	builder->deleteLater();
}

void MainWindow::showUpdaterProcessErrorDialog()
{
	QString explanation;
	if (this->d->updaterInstallerErrorCode != UpdateInstaller::PEC_GeneralFailure)
	{
		QString errorCodeExplanation = UpdateInstaller::processErrorCodeToStr(
			(UpdateInstaller::ProcessErrorCode) this->d->updaterInstallerErrorCode);
		explanation = tr("Update installation problem:\n%1").arg(errorCodeExplanation);
	}
	else
	{
		explanation = tr("Update installation failed.");
	}
	QMessageBox::critical(this, tr("Doomseeker - Auto Update problem"),
		tr("%1\n\nRemaining updates have been discarded.").arg(explanation));
}

void MainWindow::showUpdateInstallErrorDialog()
{
	QString msg = tr("Update install problem:\n%1\n\nRemaining updates have been discarded.")
		.arg(UpdateInstaller::errorCodeToStr(
			(UpdateInstaller::ErrorCode)this->d->updaterInstallerErrorCode));
	QMessageBox::critical(this, tr("Doomseeker - Auto Update problem"), msg);
}

void MainWindow::stopAutoRefreshTimer()
{
	d->autoRefreshTimer.stop();
}

void MainWindow::togglePluginQueryEnabled(const EnginePlugin *plugin)
{
	QueryMenuAction* pAction = queryMenuActionForPlugin(plugin);
	assert(pAction != NULL);

	setQueryPluginEnabled(plugin, !pAction->isChecked());
}

void MainWindow::toolBarAction(QAction* pAction)
{
	if (pAction == d->toolBarGetServers)
	{
		getServers();
	}
}

void MainWindow::trayIcon_activated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)
	{
		if (isMinimized() || !isVisible())
		{
			d->bWasMaximized == true ? showMaximized() : showNormal();
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
	for(int i = 0;i < d->masterManager->numMasters();i++)
	{
		(*d->masterManager)[i]->updateAddress();
	}

}

void MainWindow::updateServerFilter(const ServerListFilterInfo& filterInfo)
{
	d->serverTableHandler->applyFilter(filterInfo);
	d->lblServerFilterApplied->setVisible(filterInfo.isFilteringAnything());
}

void MainWindow::updateTrayIconTooltipAndLogTotalRefresh()
{
	int numServers = d->masterManager->numServers();
	int numCustoms = d->masterManager->customServs()->numServers();
	int numPlayers = d->masterManager->numPlayers() + d->masterManager->customServs()->numPlayers();

	if (d->trayIcon != NULL)
	{
		QString tip;
		tip += "Servers: " + QString::number(numServers) + " + " + QString::number(numCustoms) + " custom\n";
		tip += "Players: " + QString::number(numPlayers);
		d->trayIcon->setToolTip(tip);
	}

	if (d->bTotalRefreshInProcess)
	{
		gLog << tr("Finished refreshing. Servers on the list: %1 (+ %2 custom). Players: %3.")
			.arg(numServers).arg(numCustoms).arg(numPlayers);
	}
}

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
#include <QSizePolicy>

const QString MainWindow::HELP_SITE_URL = "http://skulltag.net/wiki/Doomseeker";

MainWindow::MainWindow(int argc, char** argv, Config* config)
: bTotalRefreshInProcess(false), buddiesList(NULL), bWasMaximized(false),
  bWantToQuit(false), configuration(config), logDock(NULL), masterManager(NULL),
  trayIcon(NULL), trayIconMenu(NULL)
{
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	setupUi(this);
	
	ip2cParser = NULL;
	
	initIP2CUpdater();
	initLogDock();

	serverTableHandler = new ServerListHandler(tableServers, configuration, this);
	connectEntities();

	// Window geometry settings
	configuration->createSetting("MainWindowX", x());
	configuration->createSetting("MainWindowY", y());
	configuration->createSetting("MainWindowWidth", width());
	configuration->createSetting("MainWindowHeight", height());

	move(configuration->setting("MainWindowX")->integer(), configuration->setting("MainWindowY")->integer());
	resize(configuration->setting("MainWindowWidth")->integer(), configuration->setting("MainWindowHeight")->integer());

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
	int slotStyle = configuration->setting("SlotStyle")->integer();
	PlayersDiagram::loadImages(slotStyle);

	// check query on statup
	bool queryOnStartup = configuration->setting("QueryOnStartup")->integer() != 0;
	if (queryOnStartup)
	{
		btnGetServers_Click();
	}
	else
	{
		// Custom servers should be refreshed no matter what.
		// They will not block the app in any way, there is no reason
		// not to refresh them.
		refreshCustomServers();
	}
	
	ip2cParseDatabase();
}

MainWindow::~MainWindow()
{
	// Window geometry settings
	configuration->setting("MainWindowMaximized")->setValue(isMaximized());
	if (!isMaximized() && !isMinimized())
	{
		configuration->setting("MainWindowX")->setValue(x());
		configuration->setting("MainWindowY")->setValue(y());
		configuration->setting("MainWindowWidth")->setValue(width());
		configuration->setting("MainWindowHeight")->setValue(height());
	}

	QList<QAction*> menuQueryActions = menuQuery->actions();
	QList<QAction*>::iterator it;
	for (it = menuQueryActions.begin(); it != menuQueryActions.end(); ++it)
	{
	    QAction* action = *it;

	    if (!action->text().isEmpty())
	    {
	        QString settingName = QString(action->text()).replace(' ', "") + "Query";
	        configuration->setting(settingName)->setValue(action->isChecked());
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
	if (configuration->setting("QueryAutoRefreshDontIfActive")->boolean() && !isMinimized())
	{
		if (QApplication::activeWindow() != 0)
		{
			return;
		}
	}

	btnGetServers_Click();
}

void MainWindow::blockRefreshButtons()
{
	btnGetServers->setEnabled(false);
	btnRefreshAll->setEnabled(false);
}

void MainWindow::btnRefreshAll_Click()
{
	serverTableHandler->refreshAll();
}

void MainWindow::btnGetServers_Click()
{
	bTotalRefreshInProcess = true;
	autoRefreshTimer.stop();
	gLog << tr("Total refresh process initialized!");
	serverTableHandler->clearTable();
	refreshCustomServers();
	
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

void MainWindow::closeEvent(QCloseEvent* event)
{
	// Check if tray icon is available and if we want to minimize to tray icon
	// when 'X' button is pressed. Real quit requests are handled by
	// quitProgram() method. This method sets bWantToQuit to true.
	if (trayIcon != NULL && configuration->setting("CloseToTrayIcon")->boolean() && !bWantToQuit)
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
	connect(btnGetServers, SIGNAL( clicked() ), this, SLOT( btnGetServers_Click() ));
	connect(btnRefreshAll, SIGNAL( clicked() ), this, SLOT( btnRefreshAll_Click() ));
	connect(menuActionAbout, SIGNAL( triggered() ), this, SLOT( menuHelpAbout() ));
	connect(menuActionBuddies, SIGNAL( triggered() ), this, SLOT( menuBuddies() ));
	connect(menuActionConfigure, SIGNAL( triggered() ), this, SLOT( menuOptionsConfigure() ));
	connect(menuActionCreateServer, SIGNAL( triggered() ), this, SLOT( menuCreateServer() ));
	connect(menuActionHelp, SIGNAL( triggered() ), this, SLOT ( menuHelpHelp() ) );
	connect(menuActionLog, SIGNAL( triggered() ), this, SLOT( menuLog() ));
	connect(menuActionUpdateIP2C, SIGNAL( triggered() ), this, SLOT( menuUpdateIP2C() ) );
	connect(menuActionQuit, SIGNAL( triggered() ), this, SLOT( quitProgram() ));
	connect(menuActionWadseeker, SIGNAL( triggered() ), this, SLOT( menuWadSeeker() ));
	connect(serverSearch, SIGNAL( textChanged(const QString &) ), serverTableHandler, SLOT( updateSearch(const QString &) ));
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

		if (configuration->settingExists(name + "Query"))
		{
			bool enabled = static_cast<bool>(configuration->setting(name + "Query")->integer());
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

void MainWindow::initAutoRefreshTimer()
{
	const unsigned MIN_DELAY = 30;
	const unsigned MAX_DELAY = 3600;

	Config* cfg = configuration;

	bool bEnabled = cfg->setting("QueryAutoRefreshEnabled")->boolean();

	if (!bEnabled)
	{
		autoRefreshTimer.stop();
	}
	else
	{
		SettingsData* setting = cfg->setting("QueryAutoRefreshEverySeconds");
		unsigned delay = setting->integer();

		// Make sure delay is in given limit.
		if (delay < MIN_DELAY)
		{
			setting->setValue(MIN_DELAY);
			delay = MIN_DELAY;
		}
		else if (delay > MAX_DELAY)
		{
			setting->setValue(MAX_DELAY);
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
	bool isEnabled = configuration->setting("UseTrayIcon")->boolean();
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
				
		QFile file(filePath);
		
		// Backup old database.
		if (file.exists())
		{
			file.open(QIODevice::ReadOnly);
			ip2cOldContent = file.readAll();
			file.close();
		}
		
		// Write new data.
		if (!file.open(QIODevice::WriteOnly))
		{
			gLog << tr("Unable to save IP2C database at path: %1").arg(filePath);
			
			ip2cOldContent.clear();
			ip2cJobsFinished();
			return;
		}
		
		file.write(downloadedData);
		file.close();		
	
		ip2cParseDatabase();
	}
}

void MainWindow::ip2cFinishedParsing(bool bSuccess)
{
	QString filePath = DoomseekerFilePaths::ip2cDatabase();
	QFile file(filePath);

	if (!bSuccess)
	{
		QString message = tr("Failed to read IP2C database. Reverting...");
		gLog << message;
		statusBar()->showMessage(message);
		
		if (ip2cOldContent.isEmpty())
		{
			gLog << "IP2C revert attempt failed. Nothing to go back to.";
			// Delete file in this case.
			file.remove();
		}
		else
		{
			// Revert to old content.
			file.open(QIODevice::WriteOnly);
			file.write(ip2cOldContent);
			file.close();
			
			// Prevent infinite recurrency.
			ip2cOldContent.clear();
			
			// Must succeed now.
			ip2cParser->readDatabase(filePath);
		}
	}
	else if (ip2cUpdater != NULL)
	{
		QString message = tr("IP2C database updated successfully.");
		gLog << message;
		statusBar()->showMessage(message);
		
		serverTableHandler->updateCountryFlags();
	}
	
	if (!ip2cParser->isParsing())
	{
		// IP2C might still be parsing if we reverted to the old database.
	
		ip2cJobsFinished();
		delete ip2cParser;
		ip2cParser = NULL;	
	}
}

void MainWindow::ip2cJobsFinished()
{
	menuActionUpdateIP2C->setEnabled(true);
	
	if (statusBar()->isAncestorOf(ip2cUpdateProgressBar))
	{
		statusBar()->removeWidget(ip2cUpdateProgressBar);
	}
	
	if (ip2cUpdater != NULL)
	{
		delete ip2cUpdater;
		ip2cUpdater = NULL;
	}
}

void MainWindow::ip2cParseDatabase()
{
	QString filePath = DoomseekerFilePaths::ip2cDatabase();
	
	menuActionUpdateIP2C->setEnabled(false);
	
	statusBar()->showMessage(tr("Please wait. IP2C Database is being read and converted if necessary. This may take some time."));
	// Attempt to read IP2C database.
	ip2cParser = new IP2CParser(Main::ip2c);
	connect (ip2cParser, SIGNAL( parsingFinished(bool) ), this, SLOT( ip2cFinishedParsing(bool) ) );
	
	ip2cParser->readDatabase(filePath);
}

void MainWindow::ip2cStartUpdate()
{
	if (ip2cUpdater != NULL)
	{
		// If update is currently in progress then prevent re-starting.
		return;
	}

	menuActionUpdateIP2C->setEnabled(false);
	
	ip2cUpdater = new IP2CUpdater();
	
	connect (ip2cUpdater, SIGNAL( databaseDownloadFinished(const QByteArray&) ), this, SLOT( ip2cFinishUpdate(const QByteArray&) ) );
	connect (ip2cUpdater, SIGNAL( downloadProgress(int, int) ), this, SLOT( ip2cDownloadProgress(int, int) ) );
	
	QString downloadUrl = configuration->setting("IP2CUrl")->string();
	
	ip2cUpdater->downloadDatabase(downloadUrl);
	statusBar()->addPermanentWidget(ip2cUpdateProgressBar);
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
	menuActionBuddies->setChecked(buddiesList->isVisible());
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
	menuActionLog->setChecked(logDock->isVisible());
}

void MainWindow::menuOptionsConfigure()
{
	ConfigureDlg configDialog(configuration, this);

	for(unsigned i = 0; i < Main::enginePlugins->numPlugins(); ++i)
	{
		const PluginInfo* pPluginInfo = (*Main::enginePlugins)[i]->info;
		ConfigurationBoxInfo* pConfigurationBoxInfo = pPluginInfo->pInterface->configuration(configuration, &configDialog);
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

void MainWindow::menuWadSeeker()
{
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
		const QString errorCaption = tr("Doomseeker - join server");

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
		JoinError jError = gameRunner->createJoinCommandLine(cli, connectPassword);
		delete gameRunner;

		const QString unknownError = tr("Unknown error.");
		const QString* error = NULL;

		switch (jError.type)
		{
			case JoinError::Critical:
				if (!jError.error.isEmpty())
				{
					error = &jError.error;
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
				if (!jError.missingIwad.isEmpty())
				{
					filesMissingMessage += tr("IWAD: ") + jError.missingIwad.toLower() + "\n";
				}

				if (!jError.missingWads.isEmpty())
				{
					filesMissingMessage += tr("PWADS: %1\nDo you want Wadseeker to find missing WADS?").arg(jError.missingWads.join(" "));
				}

				if (QMessageBox::question(this, filesMissingCaption, filesMissingMessage, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				{
					if (!jError.missingIwad.isEmpty())
					{
						jError.missingWads.append(jError.missingIwad);
					}

					WadSeekerInterface wsi;
					wsi.setAutomatic(true, jError.missingWads);
					wsi.wadseekerRef().setCustomSite(server->website());
					if (wsi.exec() == QDialog::Accepted)
					{
						return obtainJoinCommandLine(server, cli, errorCaption);
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
	btnGetServers->setEnabled(true);
	btnRefreshAll->setEnabled(true);

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
		QString error;
		MessageResult result = gameRunner->runExecutable(cli, false);
		if (result.isError)
		{
			gLog << tr("Error while launching executable for server \"%1\", game \"%2\": %3").arg(server->name()).arg(server->engineName()).arg(error);
			QMessageBox::critical(this, tr("Doomseeker - launch executable"), error);
		}

		delete gameRunner;
	}
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

void MainWindow::trayIcon_activated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)
	{
		if (isMinimized() || !isVisible())
		{
			bWasMaximized == true ? showMaximized() : showNormal();
			activateWindow();
		}
		else if (configuration->setting("CloseToTrayIcon")->boolean())
		{
			close();
		}
		else
		{
			showMinimized();
		}
	}
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

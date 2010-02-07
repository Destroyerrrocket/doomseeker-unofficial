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
#include "gui/mainwindow.h"
#include "gui/passwordDlg.h"
#include "gui/wadseekerinterface.h"
#include "gui/widgets/serversstatuswidget.h"
#include "customservers.h"
#include "log.h"
#include "pathfinder.h"
#include "main.h"
#include <QAction>
#include <QDockWidget>
#include <QFileInfo>
#include <QIcon>
#include <QHeaderView>
#include <QMessageBox>

MainWindow::MainWindow(int argc, char** argv)
: bTotalRefreshInProcess(false), bWasMaximized(false), bWantToQuit(false),
  // private
  buddiesList(NULL), logDock(NULL), mc(NULL), trayIcon(NULL), trayIconMenu(NULL)
{
	Main::mainWindow = this;
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	setupUi(this);

	initLogDock();

	serverTableHandler = new SLHandler(tableServers);
	connectEntities();

	// Window geometry settings
	Main::config->createSetting("MainWindowX", x());
	Main::config->createSetting("MainWindowY", y());
	Main::config->createSetting("MainWindowWidth", width());
	Main::config->createSetting("MainWindowHeight", height());

	move(Main::config->setting("MainWindowX")->integer(), Main::config->setting("MainWindowY")->integer());
	resize(Main::config->setting("MainWindowWidth")->integer(), Main::config->setting("MainWindowHeight")->integer());

	// Get the master
	mc = new MasterManager();
	connect(mc, SIGNAL( message(const QString&, const QString&, bool) ), this, SLOT( masterManagerMessages(const QString&, const QString&, bool) ) );

	// Allow us to enable and disable ports.
	fillQueryMenu(mc);

	// Init custom servers
	mc->customServs()->readConfig(Main::config, serverTableHandler, SLOT(serverUpdated(Server *, int)), SLOT(serverBegunRefreshing(Server *)) );

	setWindowIcon(QIcon(":/icon.png"));

	// The buddies list must always be available so we can perform certain operations on it
	buddiesList = new DockBuddiesList(this);
	connect(buddiesList, SIGNAL( visibilityChanged(bool)), menuActionBuddies, SLOT( setChecked(bool)));
	connect(buddiesList, SIGNAL( joinServer(const Server*) ), this, SLOT( runGame(const Server*) ));
	buddiesList->scan(mc);
	buddiesList->hide();
	this->addDockWidget(Qt::LeftDockWidgetArea, buddiesList);

	// IP2C
	connect(Main::ip2c, SIGNAL( databaseUpdated() ), serverTableHandler, SLOT( updateCountryFlags() ) );
	if (Main::ip2c->needsUpdate() &&
	   QMessageBox::question(this, tr("IP2C Database Update"), tr("Your IP2C database is missing or needs to be updated.  Would you like to download the database now?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
	{
		Main::ip2c->downloadDatabase(statusBar());
	}

	// Auto refresh timer
	initAutoRefreshTimer();
	connect(&autoRefreshTimer, SIGNAL( timeout() ), this, SLOT( autoRefreshTimer_timeout() ));

	// Tray icon
	initTrayIcon();

	// check query on statup
	bool queryOnStartup = Main::config->setting("QueryOnStartup")->integer() != 0;
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
}

MainWindow::~MainWindow()
{
	// Window geometry settings
	Main::config->setting("MainWindowMaximized")->setValue(isMaximized());
	if (!isMaximized() && !isMinimized())
	{
		Main::config->setting("MainWindowX")->setValue(x());
		Main::config->setting("MainWindowY")->setValue(y());
		Main::config->setting("MainWindowWidth")->setValue(width());
		Main::config->setting("MainWindowHeight")->setValue(height());
	}

	QList<QAction*> menuQueryActions = menuQuery->actions();
	QList<QAction*>::iterator it;
	for (it = menuQueryActions.begin(); it != menuQueryActions.end(); ++it)
	{
	    QAction* action = *it;

	    if (!action->text().isEmpty())
	    {
	        QString settingName = QString(action->text()).replace(' ', "") + "Query";
	        Main::config->setting(settingName)->setValue(action->isChecked());
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

	if(mc != NULL)
		delete mc;
}

void MainWindow::autoRefreshTimer_timeout()
{
	if (Main::config->setting("QueryAutoRefreshDontIfActive")->boolean() && !isMinimized())
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
	pLog << tr("Total refresh process initialized!");
	serverTableHandler->clearTable();
	refreshCustomServers();
	Main::refreshingThread->registerMaster(mc);
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
	if (trayIcon != NULL && Main::config->setting("CloseToTrayIcon")->boolean() && !bWantToQuit)
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
	connect(menuActionLog, SIGNAL( triggered() ), this, SLOT( menuLog() ));
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
		if(!plugin->generalEngineInfo().hasMasterServer)
		{
//			queryMenuPorts.append(NULL);
			continue;
		}

		MasterClient* mClient = plugin->masterClient();
		masterManager->addMaster(mClient);

		// Now is a good time to also populate the status bar widgets
		ServersStatusWidget *statusWidget = new ServersStatusWidget(plugin->icon(), mClient);
		statusBar()->addPermanentWidget(statusWidget);

		QString name = (*Main::enginePlugins)[i]->info->name;
		QQueryMenuAction* query = new QQueryMenuAction(mClient, statusWidget, menuQuery);
		menuQuery->addAction(query);

		query->setCheckable(true);
		query->setIcon(plugin->icon());
		query->setText(name);

		if (Main::config->settingExists(name + "Query"))
		{
			bool enabled = static_cast<bool>(Main::config->setting(name + "Query")->integer());
			mClient->setEnabled(enabled);
			query->setChecked(enabled);
			statusWidget->setEnabled(enabled);
		}
		else
		{
			// if no setting is found for this engine
			// set default as follows:
			mClient->setEnabled(true);
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

	refreshServers(mc);
}

void MainWindow::initAutoRefreshTimer()
{
	const unsigned MIN_DELAY = 30;
	const unsigned MAX_DELAY = 3600;

	Config* cfg = Main::config;

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

void MainWindow::initLogDock()
{
	logDock = new LogDock(this);
	connect(logDock, SIGNAL( visibilityChanged(bool)), menuActionLog, SLOT( setChecked(bool)));
	logDock->hide();
	this->addDockWidget(Qt::BottomDockWidgetArea, logDock);

	connect(&pLog, SIGNAL( newEntry(const QString&) ), logDock, SLOT( appendLogEntry(const QString&) ) );

	// Also add anything that already might be in the log to the box.
	logDock->appendLogEntry(pLog.content());
}

void MainWindow::initTrayIcon()
{
	bool isEnabled = Main::config->setting("UseTrayIcon")->boolean();
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

void MainWindow::masterManagerMessages(const QString& title, const QString& content, bool isError)
{
	pLog << tr("Message from master manager. TITLE: %1 | CONTENT: %2 | IS ERROR: %3").arg(title).arg(content).arg(isError ? tr("yes") : tr("no"));
	if (isError)
	{
		QMessageBox::critical(this, title, content, QMessageBox::Ok, QMessageBox::Ok);
	}
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

void MainWindow::menuLog()
{
	logDock->setVisible(!logDock->isVisible());
	menuActionLog->setChecked(logDock->isVisible());
}

void MainWindow::menuOptionsConfigure()
{
	ConfigureDlg dlg(Main::config, this);

	for(unsigned i = 0; i < Main::enginePlugins->numPlugins(); ++i)
	{
		ConfigurationBoxInfo* ec = (*Main::enginePlugins)[i]->info->pInterface->configuration(Main::config, &dlg);
		dlg.addEngineConfiguration(ec);
	}

	// Stop the auto refresh timer during configuration.
	autoRefreshTimer.stop();
	dlg.exec();

	// Do some cleanups after config box finishes.
	initAutoRefreshTimer();

	if (dlg.appearanceChanged())
	{
		serverTableHandler->redraw();
		initTrayIcon();
	}

	// Refresh custom servers list:
	if (dlg.customServersChanged())
	{
		serverTableHandler->serverModel()->removeCustomServers();
		mc->customServs()->readConfig(Main::config, serverTableHandler, SLOT(serverUpdated(Server *, int)), SLOT(serverBegunRefreshing(Server *)) );
		refreshCustomServers();
	}
}

void MainWindow::menuWadSeeker()
{
	WadSeekerInterface wsi(this);
	wsi.exec();
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

		JoinError jError = server->createJoinCommandLine(cli, connectPassword);
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
				pLog << tr("Error when obtaining join parameters for server \"%1\", game \"%2\": %3").arg(server->name()).arg(server->engineName()).arg(*error);
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

				if (QMessageBox::question(Main::mainWindow, filesMissingCaption, filesMissingMessage, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
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
	CustomServers* cs = mc->customServs();

	for(int i = 0;i < cs->numServers();i++)
	{
		serverTableHandler->serverUpdated((*cs)[i], Server::RESPONSE_NO_RESPONSE_YET);
		(*cs)[i]->refresh(); // This will register server with refreshing thread.
	}
}

void MainWindow::refreshServers(MasterClient* master)
{
	for(int i = 0;i < master->numServers();i++)
	{
		serverTableHandler->serverUpdated((*master)[i], Server::RESPONSE_NO_RESPONSE_YET);
		(*master)[i]->refresh(); // This will register server with refreshing thread.
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
		QString error;
		if (!server->runExecutable(cli, false, error))
		{
			pLog << tr("Error while launching executable for server \"%1\", game \"%2\": %3").arg(server->name()).arg(server->engineName()).arg(error);
			QMessageBox::critical(this, tr("Doomseeker - launch executable"), error);

		}
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
		else if (Main::config->setting("CloseToTrayIcon")->boolean())
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
	int numServers = mc->numServers();
	int numCustoms = mc->customServs()->numServers();
	int numPlayers = mc->numPlayers() + mc->customServs()->numPlayers();

	if (trayIcon != NULL)
	{
		QString tip;
		tip += "Servers: " + QString::number(numServers) + " + " + QString::number(numCustoms) + " custom\n";
		tip += "Players: " + QString::number(numPlayers);
		trayIcon->setToolTip(tip);
	}

	if (bTotalRefreshInProcess)
	{
		pLog << tr("Finished refreshing. Servers on the list: %1 (+ %2 custom). Players: %3.").arg(numServers).arg(numCustoms).arg(numPlayers);
	}
}

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
#include "gui/createserver.h"
#include "gui/dockBuddiesList.h"
#include "gui/dockserverinfo.h"
#include "gui/mainwindow.h"
#include "gui/passwordDlg.h"
#include "gui/wadseekerinterface.h"
#include "customservers.h"
#include "pathfinder.h"
#include "main.h"
#include <QAction>
#include <QDockWidget>
#include <QFileInfo>
#include <QIcon>
#include <QHeaderView>
#include <QMessageBox>

MainWindow::MainWindow(int argc, char** argv) : mc(NULL), buddiesList(NULL), trayIcon(NULL), trayIconMenu(NULL), bWantToQuit(false)
{
	Main::mainWindow = this;
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	setupUi(this);

	// Connect refreshing thread.
	connect(Main::refreshingThread, SIGNAL( finishedQueryingMaster(MasterClient*) ), this, SLOT( finishedQueryingMaster(MasterClient*) ) );
	connect(Main::refreshingThread, SIGNAL( sleepingModeEnter() ), this, SLOT( refreshThreadEndsWork() ) );
	connect(Main::refreshingThread, SIGNAL( sleepingModeExit() ), this, SLOT( refreshThreadBeginsWork() ) );

	// Window geometry settings
	Main::config->createSetting("MainWindowX", x());
	Main::config->createSetting("MainWindowY", y());
	Main::config->createSetting("MainWindowWidth", width());
	Main::config->createSetting("MainWindowHeight", height());

	move(Main::config->setting("MainWindowX")->integer(), Main::config->setting("MainWindowY")->integer());
	resize(Main::config->setting("MainWindowWidth")->integer(), Main::config->setting("MainWindowHeight")->integer());

	serverTableHandler = new SLHandler(tableServers);
	serverInfo = NULL;

	// Allow us to enable and disable ports.
	queryMenuPorts = new const QAction*[Main::enginePlugins.numPlugins()];
	for(int i = 0;i < Main::enginePlugins.numPlugins();i++)
	{
	    QString name = Main::enginePlugins[i]->info->name;
		QAction *query = menuQuery->addAction(name, this, SLOT( enablePort() ));
		query->setIcon(Main::enginePlugins[i]->info->pInterface->icon());
		query->setCheckable(true);

		if (Main::config->settingExists(name + "Query"))
		{
			query->setChecked( static_cast<bool>(Main::config->setting(name + "Query")->integer()) );
		}
		else
		{
			// if no setting is found for this engine
			// set default as follows:
			query->setChecked(true);
		}
		queryMenuPorts[i] = query;
	}
	// Get the master
	mc = new MasterManager();
	connect(mc, SIGNAL( message(const QString&, const QString&, bool) ), this, SLOT( masterManagerMessages(const QString&, const QString&, bool) ) );

	// Init custom servers
	mc->customServs()->readConfig(Main::config, serverTableHandler, SLOT(serverUpdated(Server *, int)), SLOT(serverBegunRefreshing(Server *)) );

	setWindowIcon(QIcon(":/icon.png"));

	// The buddies list must always be available so we can perform certain operations on it
	buddiesList = new DockBuddiesList(this);
	connect(buddiesList, SIGNAL( visibilityChanged(bool)), menuActionBuddies, SLOT( setChecked(bool)));
	buddiesList->scan(mc);
	buddiesList->hide();
	this->addDockWidget(Qt::LeftDockWidgetArea, buddiesList);

	connect(btnGetServers, SIGNAL( clicked() ), this, SLOT( btnGetServers_Click() ));
	connect(btnRefreshAll, SIGNAL( clicked() ), this, SLOT( btnRefreshAll_Click() ));
	connect(menuActionAbout, SIGNAL( triggered() ), this, SLOT( menuHelpAbout() ));
	connect(menuActionBuddies, SIGNAL( triggered() ), this, SLOT( menuBuddies() ));
	connect(menuActionConfigure, SIGNAL( triggered() ), this, SLOT( menuOptionsConfigure() ));
	connect(menuActionCreateServer, SIGNAL( triggered() ), this, SLOT( menuCreateServer() ));
	connect(menuActionQuit, SIGNAL( triggered() ), this, SLOT( quitProgram() ));
	connect(menuActionServerInfo, SIGNAL( triggered() ), this, SLOT( menuServerInfo() ));
	connect(menuActionWadseeker, SIGNAL( triggered() ), this, SLOT( menuWadSeeker() ));
	connect(serverSearch, SIGNAL( textChanged(const QString &) ), serverTableHandler, SLOT( updateSearch(const QString &) ));
	connect(serverTableHandler, SIGNAL( serverDoubleClicked(const Server*) ), this, SLOT( runGame(const Server*) ) );
	connect(serverTableHandler, SIGNAL( serversSelected(QList<Server*>&) ), this, SLOT( updateServerInfo(QList<Server*>&) ) );

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

	// This must be executed in order to set port query booleans
	// after the Query menu actions settings are read.
	enablePort();

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
		refreshServers(true); // This should include only refreshing customs
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
	        QString settingName = action->text() + "Query";
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

	delete[] queryMenuPorts;
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

void MainWindow::btnRefreshAll_Click()
{
	serverTableHandler->refreshAll();
}

void MainWindow::btnGetServers_Click()
{
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

void MainWindow::enablePort()
{
	for(int i = 0;i < Main::enginePlugins.numPlugins();i++)
	{
		mc->enableMaster(i, queryMenuPorts[i]->isChecked());
	}
}

void MainWindow::finishedQueryingMaster(MasterClient* master)
{
	if (mc->numServers() == 0 && mc->customServs()->numServers() == 0)
	{
		return;
	}

	serverTableHandler->clearTable();

	for(int i = 0;i < mc->numServers();i++)
	{
		connect((*mc)[i], SIGNAL(updated(Server *, int)), serverTableHandler, SLOT(serverUpdated(Server *, int)) );
		connect((*mc)[i], SIGNAL(begunRefreshing(Server *)), serverTableHandler, SLOT(serverBegunRefreshing(Server *)) );
	}

	refreshServers(false);
}

void MainWindow::masterManagerMessages(const QString& title, const QString& content, bool isError)
{
	if (isError)
	{
		QMessageBox::critical(this, title, content, QMessageBox::Ok, QMessageBox::Ok);
	}
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

		updateTrayIconTooltip();

		trayIcon->setContextMenu(trayIconMenu);
		trayIcon->setIcon(QIcon(":/icon.png"));
		trayIcon->setVisible(true);
	}
}

void MainWindow::menuBuddies()
{
	if (buddiesList->isVisible())
		buddiesList->hide();
	else
		buddiesList->show();

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

void MainWindow::menuOptionsConfigure()
{
	ConfigureDlg dlg(Main::config, this);

	for(int i = 0;i < Main::enginePlugins.numPlugins();i++)
	{
		ConfigurationBoxInfo* ec = Main::enginePlugins[i]->info->pInterface->configuration(Main::config, &dlg);
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
		refreshServers(true);
	}
}

void MainWindow::menuServerInfo()
{
	if (serverInfo == NULL)
	{
		serverInfo = new DockServerInfo(this);
		connect(serverInfo, SIGNAL( visibilityChanged(bool)), menuActionServerInfo, SLOT( setChecked(bool)));
		this->addDockWidget(Qt::RightDockWidgetArea, serverInfo);

		QList<Server*> slist = serverTableHandler->selectedServers();
		if (slist.count() == 1)
		{
			serverInfo->updateServerInfo(slist[0]);
		}
	}
	else
	{
	    if (serverInfo->isVisible())
	    {
	        serverInfo->hide();
	    }
	    else
	    {
	        serverInfo->show();
	    }
	}
}

void MainWindow::menuWadSeeker()
{
	WadSeekerInterface wsi(this);
	wsi.exec();
}

void MainWindow::quitProgram()
{
	bWantToQuit = true;
	close();
}

void MainWindow::refreshServers(bool onlyCustom)
{
	CustomServers* cs = mc->customServs();

	for(int i = 0;i < cs->numServers();i++)
	{
		(*cs)[i]->refresh(); // This will register server with refreshing thread.
	}

	if (!onlyCustom)
	{
		for(int i = 0;i < mc->numServers();i++)
		{
			(*mc)[i]->refresh(); // This will register server with refreshing thread.
		}
	}
}

void MainWindow::refreshThreadBeginsWork()
{
	// disable refresh.
	btnGetServers->setEnabled(false);
	btnRefreshAll->setEnabled(false);
	serverTableHandler->serverTable()->setAllowAllRowsRefresh(false);
	statusBar()->showMessage(tr("Querying..."));
}

void MainWindow::refreshThreadEndsWork()
{
	btnGetServers->setEnabled(true);
	btnRefreshAll->setEnabled(true);
	serverTableHandler->serverTable()->setAllowAllRowsRefresh(true);
	statusBar()->showMessage(tr("Done"));
}

void MainWindow::runGame(const Server* server)
{
	QString connectPassword;
	if(server->isLocked())
	{
		PasswordDlg password(this);
		int ret = password.exec();

		if(ret == QDialog::Accepted)
			connectPassword = password.connectPassword();
		else
			return;
	}
	server->join(connectPassword);
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

void MainWindow::updateServerInfo(QList<Server*>& servers)
{
	if (serverInfo != NULL)
	{
		if (servers.isEmpty())
		{
			serverInfo->updateServerInfo(NULL);
		}
		else if (servers.count() == 1)
		{
			serverInfo->updateServerInfo(servers[0]);
		}
	}
}

void MainWindow::updateTrayIconTooltip()
{
	if (trayIcon != NULL)
	{
		QString tip;
		tip += "Servers: " + QString::number(mc->numServers()) + " + " + QString::number(mc->customServs()->numServers()) + " custom\n";
		tip += "Players: " + QString::number(mc->numPlayers() + mc->customServs()->numPlayers());
		trayIcon->setToolTip(tip);
	}
}

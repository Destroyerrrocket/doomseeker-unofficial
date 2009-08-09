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

MainWindow::MainWindow(int argc, char** argv) : mc(NULL), buddiesList(NULL)
{
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	setupUi(this);

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
	connect(btnRefreshAll, SIGNAL( clicked() ), serverTableHandler, SLOT( refreshAll() ));
	connect(menuActionAbout, SIGNAL( triggered() ), this, SLOT( menuHelpAbout() ));
	connect(menuActionBuddies, SIGNAL( triggered() ), this, SLOT( menuBuddies() ));
	connect(menuActionConfigure, SIGNAL( triggered() ), this, SLOT( menuOptionsConfigure() ));
	connect(menuActionCreateServer, SIGNAL( triggered() ), this, SLOT( menuCreateServer() ));
	connect(menuActionQuit, SIGNAL( triggered() ), this, SLOT( close() ));
	connect(menuActionServerInfo, SIGNAL( triggered() ), this, SLOT( menuServerInfo() ));
	connect(menuActionWadseeker, SIGNAL( triggered() ), this, SLOT( menuWadSeeker() ));
	connect(serverSearch, SIGNAL( textChanged(const QString &) ), serverTableHandler, SLOT( updateSearch(const QString &) ));
	connect(serverTableHandler, SIGNAL( serverDoubleClicked(const Server*) ), this, SLOT( runGame(const Server*) ) );
	connect(serverTableHandler, SIGNAL( serversSelected(QList<Server*>&) ), this, SLOT( updateServerInfo(QList<Server*>&) ) );

	// check query on statup
	bool queryOnStartup = Main::config->setting("QueryOnStartup")->integer() != 0;
	if (queryOnStartup)
		btnGetServers_Click();
	else
	{
		// Custom servers should be refreshed no matter what.
		// They will not block the app in any way, there is no reason
		// not to refresh them.
		refreshServers(true); // This should include only refreshing customs
	}

	// IP2C
	connect(Main::ip2c, SIGNAL( databaseUpdated() ), serverTableHandler, SLOT( updateCountryFlags() ) );
	if (Main::ip2c->needsUpdate())
	{
		Main::ip2c->downloadDatabase();
	}

	// This must be executed in order to set port query booleans
	// after the Query menu actions settings are read.
	enablePort();
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

	delete serverTableHandler;

	if(mc != NULL)
		delete mc;
	delete[] queryMenuPorts;
}

void MainWindow::btnGetServers_Click()
{
	mc->refresh();

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

void MainWindow::checkRefreshFinished()
{
	btnGetServers->setEnabled(true);
	btnRefreshAll->setEnabled(true);
	serverTableHandler->serverTable()->setAllowAllRowsRefresh(true);
	statusBar()->showMessage(tr("Done"));
}

void MainWindow::enablePort()
{
	for(int i = 0;i < Main::enginePlugins.numPlugins();i++)
	{
		mc->enableMaster(i, queryMenuPorts[i]->isChecked());
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
	dlg.exec();
}

void MainWindow::menuHelpAbout()
{
	AboutDlg dlg(this);
	dlg.exec();
}

void MainWindow::menuOptionsConfigure()
{
	ConfigureDlg dlg(Main::config, this);

	for(int i = 0;i < Main::enginePlugins.numPlugins();i++)
	{
		ConfigurationBoxInfo* ec = Main::enginePlugins[i]->info->pInterface->configuration(Main::config, &dlg);
		dlg.addEngineConfiguration(ec);
	}

	dlg.exec();

	// Do some cleanups after config box finishes.

	// Refresh custom servers list:
	if (dlg.customServersChanged())
	{
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

void MainWindow::refreshServers(bool onlyCustom)
{
	serverTableHandler->serverModel()->removeCustomServers();
	CustomServers* cs = mc->customServs();
	for(int i = 0;i < cs->numServers();i++)
	{
		(*cs)[i]->refresh();
	}

	if (!onlyCustom)
	{
		for(int i = 0;i < mc->numServers();i++)
		{
			(*mc)[i]->refresh();
		}

		ServerRefresher* guardian = new ServerRefresher(NULL);
		connect(guardian, SIGNAL( allServersRefreshed() ), this, SLOT(checkRefreshFinished()) );
		connect(guardian, SIGNAL( allServersRefreshed() ), buddiesList, SLOT(scan()) );
		guardian->startGuardian();

		// disable refresh.
		btnGetServers->setEnabled(false);
		btnRefreshAll->setEnabled(false);
		serverTableHandler->serverTable()->setAllowAllRowsRefresh(false);
		statusBar()->showMessage(tr("Querying..."));
	}
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

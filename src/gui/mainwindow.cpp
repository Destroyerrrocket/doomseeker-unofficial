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
#include "gui/mainwindow.h"
#include "gui/configureDlg.h"
#include "gui/dockserverinfo.h"
#include "gui/engineSkulltagConfig.h"
#include "gui/wadseekerinterface.h"
#include "pathfinder.h"
#include "main.h"
#include <QAction>
#include <QDockWidget>
#include <QFileInfo>
#include <QProcess>
#include <QHeaderView>
#include <QMessageBox>

MainWindow::MainWindow(int argc, char** argv) : mc(NULL)
{
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	setupUi(this);

	serverTableHandler = new SLHandler(tableServers);
	serverInfo = NULL;

	// Allow us to enable and disable ports.
	queryMenuPorts = new const QAction*[Main::enginePlugins.numPlugins()];
	for(int i = 0;i < Main::enginePlugins.numPlugins();i++)
	{
		QAction *query = menuQuery->addAction(Main::enginePlugins[i]->info->name, this, SLOT( enablePort() ));
		query->setCheckable(true);
		query->setChecked(true);
		queryMenuPorts[i] = query;
	}

	// Get the master
	mc = new MasterManager();

	connect(btnGetServers, SIGNAL( clicked() ), this, SLOT( btnGetServers_Click() ));
	connect(btnRefreshAll, SIGNAL( clicked() ), serverTableHandler, SLOT( refreshAll() ));
	connect(menuActionAbout, SIGNAL( triggered() ), this, SLOT( menuHelpAbout() ));
	connect(menuActionConfigure, SIGNAL( triggered() ), this, SLOT( menuOptionsConfigure() ));
	connect(menuActionQuit, SIGNAL( triggered() ), this, SLOT( close() ));
	connect(menuActionServerInfo, SIGNAL( triggered() ), this, SLOT( menuServerInfo() ));
	connect(menuActionWadseeker, SIGNAL( triggered() ), this, SLOT( menuWadSeeker() ));
	connect(serverTableHandler, SIGNAL( serverDoubleClicked(const Server*) ), this, SLOT( runGame(const Server*) ) );
	connect(serverTableHandler, SIGNAL( serversSelected(QList<Server*>&) ), this, SLOT( updateServerInfo(QList<Server*>&) ) );
}

MainWindow::~MainWindow()
{
	if(mc != NULL)
		delete mc;
	delete[] queryMenuPorts;
}
/////////////////////////////////////////////////////////
// Slots
void MainWindow::checkRefreshFinished()
{
	btnGetServers->setEnabled(true);
	btnRefreshAll->setEnabled(true);
	serverTableHandler->serverTable()->setAllowAllRowsRefresh(true);
}

void MainWindow::btnGetServers_Click()
{
	mc->refresh();

	if (mc->numServers() == 0)
	{
		return;
	}

	serverTableHandler->clearTable();

	for(int i = 0;i < mc->numServers();i++)
	{
		QObject::connect((*mc)[i], SIGNAL(updated(Server *, int)), serverTableHandler, SLOT(serverUpdated(Server *, int)) );
		(*mc)[i]->refresh();
	}

	ServerRefresher* guardian = new ServerRefresher(NULL);
	connect(guardian, SIGNAL( allServersRefreshed() ), this, SLOT(checkRefreshFinished()) );
	guardian->startGuardian();

	// disable refresh.
	btnGetServers->setEnabled(false);
	btnRefreshAll->setEnabled(false);
	serverTableHandler->serverTable()->setAllowAllRowsRefresh(false);
}

void MainWindow::enablePort()
{
	for(int i = 0;i < Main::enginePlugins.numPlugins();i++)
	{
		mc->enableMaster(i, queryMenuPorts[i]->isChecked());
	}
}

void MainWindow::menuHelpAbout()
{
	AboutDlg dlg(this);
	dlg.exec();
}

void MainWindow::menuOptionsConfigure()
{
	ConfigureDlg dlg(Main::config, this);

	ConfigurationBoxInfo* ec = EngineSkulltagConfigBox::createStructure(Main::config, &dlg);
	dlg.addEngineConfiguration(ec);

	dlg.exec();
}

void MainWindow::menuServerInfo()
{
	if (serverInfo == NULL)
	{
		serverInfo = new DockServerInfo(this);
		this->addDockWidget(Qt::RightDockWidgetArea, serverInfo);

		QList<Server*> slist = serverTableHandler->selectedServers();
		if (slist.count() == 1)
		{
			serverInfo->updateServerInfo(slist[0]);
		}
		menuActionServerInfo->setChecked(true);
	}
	else
	{
	    if (serverInfo->isVisible())
	    {
	        serverInfo->hide();
	        menuActionServerInfo->setChecked(false);
	    }
	    else
	    {
	        serverInfo->show();
	        menuActionServerInfo->setChecked(true);
	    }
	}
}

void MainWindow::menuWadSeeker()
{
	WadSeekerInterface wsi(this);
	wsi.exec();
}

void MainWindow::runGame(const Server* server)
{
	const QString errorCaption = tr("Doomseeker - error");
	SettingsData* setting = Main::config->setting(server->clientBinary());
	if (setting->string().isEmpty())
	{
		QMessageBox::critical(this, errorCaption, tr("No executable specified for this engine."));
		return;
	}

	QFileInfo fileinfo(setting->string());

	if (!fileinfo.exists() || fileinfo.isDir())
	{
		QMessageBox::critical(this, errorCaption, tr("File: ") + fileinfo.absoluteFilePath() + tr("\ndoesn't exist or is a directory"));
		return;
	}

	PathFinder pf(Main::config);
	QStringList args;
	QStringList missingPwads;
	bool iwadFound = false;

	server->connectParameters(args, pf, iwadFound);

	// Pwads
	if (server->numWads() != 0)
	{
		args << "-file";
	}

	for (int i = 0; i < server->numWads(); ++i)
	{
		QString pwad = pf.findWad(server->wad(i));
		if (pwad.isEmpty())
		{
			missingPwads << server->wad(i);
		}
		else
		{
			args << pwad;
		}
	}

	if (!iwadFound || !missingPwads.isEmpty())
	{
		const QString filesMissingCaption = tr("Doomseeker - files are missing");
		QString error = tr("Following files are missing:\n");

		if (!iwadFound)
		{
			error += tr("IWAD: ") + server->iwadName().toLower() + "\n";
		}

		if (!missingPwads.isEmpty())
		{
			error += tr("PWADS: ") + missingPwads.join(" ");
		}

		QMessageBox::critical(this, filesMissingCaption, error);
		return;
	}

	printf("Starting: %s %s", fileinfo.absoluteFilePath().toAscii().constData(), args.join(" ").toAscii().constData());

	QProcess proc;
	if( !proc.startDetached(fileinfo.absoluteFilePath(), args, fileinfo.absolutePath()) )
	{
		QMessageBox::critical(this, errorCaption, tr("File: ") + fileinfo.absoluteFilePath() + tr("\ncannot be run"));
		return;
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

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

#include "main.h"
#include "sdeapi/config.hpp"
#include "sdeapi/pluginloader.hpp"
#include "gui/dockserverinfo.h"
#include "gui/serverlist.h"
#include "ui_mainwindow.h"

#include <QString>
#include <QStandardItem>

class MainWindow : public QMainWindow, private Ui::MainWindowWnd
{
	Q_OBJECT

	public:
		MainWindow(int argc, char** argv);
		~MainWindow();

	public slots:
		void checkRefreshFinished();
		void btnGetServers_Click();
		void menuHelpAbout();
		void menuOptionsConfigure();
		void menuServerInfo();
		void runGame(const Server*);
		void updateServerInfo(QList<Server*>&);


	private:
		DockServerInfo*		serverInfo;
		SLHandler*			serverTableHandler;
		Config* 			config;
		PluginLoader		enginePlugins;
};

#endif

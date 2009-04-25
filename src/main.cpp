//------------------------------------------------------------------------------
// main.cpp
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include <QApplication>
#include <QObject>
#include <QThreadPool>

#include "gui/mainwindow.h"
#include "main.h"

PluginLoader Main::enginePlugins(MAKEID('E','N','G','N'), "./engines/");
Config *Main::config = new Config();

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	Main::config->locateConfigFile(argc, argv);

	MainWindow* mw = new MainWindow(argc, argv);
	mw->show();

	delete Main::config;
	return app.exec();
}

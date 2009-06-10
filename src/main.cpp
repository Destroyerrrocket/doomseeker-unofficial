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
#include "server.h"
#include "wadseeker/wadseeker.h"

PluginLoader Main::enginePlugins(MAKEID('E','N','G','N'), "./engines/");
QWidget* Main::mainWindow = NULL;
Config *Main::config = new Config();
IP2C *Main::ip2c = new IP2C("IpToCountry.csv", QUrl("http://software77.net/geo-ip?DL=1"));
bool Main::running = true;

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	Main::config->locateConfigFile(argc, argv);

	// Initial settings values
	Main::config->createSetting("QueryThreads", 50);
	Main::config->createSetting("QueryTimeout", 5000);
	QStringList urlList = Wadseeker::defaultSitesListEncoded();
	Main::config->createSetting("WadseekerSearchURLs", urlList.join(";"));
	Main::config->createSetting("WadseekerConnectTimeoutSeconds", WADSEEKER_CONNECT_TIMEOUT_SECONDS_DEFAULT);
	Main::config->createSetting("WadseekerDownloadTimeoutSeconds", WADSEEKER_DOWNLOAD_TIMEOUT_SECONDS_DEFAULT);

	// Init plugin settings
	Main::enginePlugins.initConfig();

	MainWindow* mw = new MainWindow(argc, argv);
	Main::mainWindow = mw;
	mw->show();

	int ret = app.exec();
	Main::running = false;

	Main::config->saveConfig();
	delete Main::config;

	delete Main::ip2c;

	return ret;
}

void Main::translateServerAddress(const QString& settingValue, QString& hostname, short& port, const QString& defaultHostname, const short defaultPort)
{
	QStringList addressAndPort = settingValue.split(":");
	if (addressAndPort.size() == 0 || addressAndPort.size() > 2)
	{ // if something is not right set default settings
		hostname = defaultHostname;
	}
	else
	{
		hostname = addressAndPort[0];
		if (addressAndPort.size() == 2)
		{
			port = addressAndPort[1].toShort();
		}
	}

	if (port == 0)
		port = defaultPort;
}

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
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QThreadPool>

#include "gui/mainwindow.h"
#include "log.h"
#include "main.h"
#include "server.h"
#include "wadseeker/wadseeker.h"

Config*				Main::config = new Config();
PluginLoader* 		Main::enginePlugins = NULL;
IP2C*				Main::ip2c = NULL;
QWidget*			Main::mainWindow = NULL;
RefreshingThread*	Main::refreshingThread = new RefreshingThread();
bool				Main::running = true;
QString				Main::workingDirectory = "./";

int main(int argc, char* argv[])
{
	pLog << "Starting Doomseeker. Hello World! :)";

	// Check for command line parameters
	bool updateip2c = false;
	for(int i = 0;i < argc;i++)
	{
		if(strcmp(argv[i], "--updateip2c") == 0)
			updateip2c = true;
		else if(strcmp(argv[i], "--help") == 0)
		{
			// Print information to the log and terminate.
			pLog << QObject::tr("Available command line parameters:");
			pLog << QObject::tr("	--updateip2c : Updates the IP2C database.");
			return 0;
		}
	}

	Main::enginePlugins = new PluginLoader(MAKEID('E','N','G','N'), "./engines/");
	QApplication app(argc, argv);

	QString firstArg = argv[0];
	int lastSlash = qMax<int>(firstArg.lastIndexOf('\\'), firstArg.lastIndexOf('/'));
	if(lastSlash != -1)
		Main::workingDirectory = firstArg.mid(0, lastSlash+1);

	// If no plugins were found in ./ try looking in the directory in argv[0].
	if(Main::enginePlugins->numPlugins() == 0)
		Main::enginePlugins->resetPluginsDirectory(Main::workingDirectory.mid(0, lastSlash+1) + "engines/");

	pLog << QObject::tr("Initializing IP2C database.");
	Main::ip2c = new IP2C(Main::workingDirectory + "IpToCountry.csv", QUrl("http://software77.net/geo-ip?DL=1"));
	if(updateip2c)
	{
		pLog << QObject::tr("Starting the IP2C updater.");
		// We'll use a small window to display the update progress.
		QMainWindow updateProgressBox;
		updateProgressBox.setWindowTitle(QObject::tr("IP2C Updater"));
		updateProgressBox.setCentralWidget(new QLabel("Updating the IP2C database...\nOnce the progress bar disappears you may close this window."));
		if(QMessageBox::question(&updateProgressBox, QObject::tr("IP2C Updater"), QObject::tr("Update the IP2C database now?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
		{
			updateProgressBox.show();
			Main::ip2c->downloadDatabase(updateProgressBox.statusBar());
			return app.exec();
		}
		return 0;
	}

	pLog << QObject::tr("Initializing configuration file.");
	Main::config->locateConfigFile(argc, argv);

	// Initial settings values
	Main::config->createSetting("CustomServersColor", (0x94 << 16) | (0xff << 8) | (0xff)); // r | g | b
	Main::config->createSetting("MainWindowMaximized", 0);
	Main::config->createSetting("UseTrayIcon", false); // tray icon
	Main::config->createSetting("CloseToTrayIcon", false); // tray icon
	Main::config->createSetting("QueryAutoRefreshEnabled", false);
	Main::config->createSetting("QueryAutoRefreshEverySeconds", 30);
	Main::config->createSetting("QueryAutoRefreshDontIfActive", true);
	Main::config->createSetting("QueryOnStartup", true);
	Main::config->createSetting("QueryTries", 7);
	Main::config->createSetting("QueryTimeout", 1000);
	QStringList urlList = Wadseeker::defaultSitesListEncoded();
	Main::config->createSetting("WadseekerSearchURLs", urlList.join(";"));
	Main::config->createSetting("WadseekerSearchInIdgames", true);
	Main::config->createSetting("WadseekerIdgamesPriority", 0); // 0 == After all other sites
	Main::config->createSetting("WadseekerIdgamesURL", Wadseeker::defaultIdgamesUrl());
	Main::config->createSetting("WadseekerConnectTimeoutSeconds", WADSEEKER_CONNECT_TIMEOUT_SECONDS_DEFAULT);
	Main::config->createSetting("WadseekerDownloadTimeoutSeconds", WADSEEKER_DOWNLOAD_TIMEOUT_SECONDS_DEFAULT);

	// Init plugin settings
	pLog << QObject::tr("Initializing configuration for plugins.");
	Main::enginePlugins->initConfig();

	// Refreshing thread setup:
	pLog << QObject::tr("Starting refreshing thread.");
	Main::refreshingThread->setDelayBetweenResends(Main::config->setting("QueryTimeout")->integer());
	Main::refreshingThread->start();

	// Create main window
	pLog << QObject::tr("Preparing GUI.");
	MainWindow* mw = new MainWindow(argc, argv);
	if (Main::config->setting("MainWindowMaximized")->boolean())
	{
		mw->showMaximized();
	}
	else
	{
		mw->show();
	}

	pLog << QObject::tr("Init finished.");
	pLog.addUnformattedEntry("================================\n");
	int ret = app.exec();

	Main::refreshingThread->quit();
	Main::running = false;

	Main::config->saveConfig();
	delete Main::config;
	delete Main::ip2c;

	while (Main::refreshingThread->isRunning());
	delete Main::refreshingThread;
	delete Main::enginePlugins;

	return ret;
}

bool Main::isCharOnCharList(char c, const QString& charList)
{
	for (int i = 0; i < charList.length(); ++i)
	{
		if (charList[i] == c)
			return true;
	}

	return false;
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

QString& Main::trimr(QString& str, const QString& charList)
{
	int i;
	for (i = str.length() - 1; i >= 0; --i)
	{
		if (!isCharOnCharList(str[i].toAscii(), charList))
			break;
	}
	++i;

	return str.remove(i, str.length() - i);
}

QString& Main::triml(QString& str, const QString& charList)
{
	int i;
	for (i = 0; i < str.length(); ++i)
	{
		if (!isCharOnCharList(str[i].toAscii(), charList))
			break;
	}

	return str.remove(0, i);
}

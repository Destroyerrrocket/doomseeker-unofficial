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
#include <QDir>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QThreadPool>

#include "gui/mainwindow.h"
#include "gui/remoteconsole.h"
#include "log.h"
#include "main.h"
#include "serverapi/server.h"
#include "strings.h"
#include "wadseeker/wadseeker.h"

Config*				Main::config = new Config();
PluginLoader* 		Main::enginePlugins = NULL;
IP2C*				Main::ip2c = NULL;
QWidget*			Main::mainWindow = NULL;
RefreshingThread*	Main::refreshingThread = new RefreshingThread();
bool				Main::running = true;
QString				Main::workingDirectory = "./";

Main::Main(int argc, char* argv[])
: application(NULL), arguments(argv), argumentsCount(argc),
  startRcon(false), updateIP2CAndQuit(false)
{
}

Main::~Main()
{
	running = false;
	if (refreshingThread != NULL)
	{
		refreshingThread->quit();
		while (refreshingThread->isRunning());
		delete refreshingThread;
	}

	if (config != NULL)
	{
		config->saveConfig();
		delete config;
	}

	if (ip2c != NULL)
	{
		delete ip2c;
	}

	if (enginePlugins != NULL)
	{
		delete enginePlugins;
	}

	if (application != NULL)
	{
		delete application;
	}
}

// This method is an exception to sorting everything in alphabetical order
// because it's... the main method.
int Main::run()
{
	if (!interpretCommandLineParameters())
	{
		return 0;
	}

	gLog << "Starting Doomseeker. Hello World! :)";

	initDataDirectories();

	enginePlugins = new PluginLoader(MAKEID('E','N','G','N'), dataDirectories, "engines/");
	application = new QApplication(argumentsCount, arguments);

	int ip2cReturn = initIP2C();
	if (updateIP2CAndQuit)
	{
		return ip2cReturn;
	}

	initMainConfig();
	initPluginConfig();

	if (startRcon)
	{
		if (!createRemoteConsole())
			return 0;
	}
	else
	{
		setupRefreshingThread();

		createMainWindow();
	}

	gLog << tr("Init finished.");
	gLog.addUnformattedEntry("================================\n");

	return application->exec();
}

void Main::createMainWindow()
{
	gLog << tr("Preparing GUI.");

	MainWindow* mainWnd = new MainWindow(argumentsCount, arguments, config);
	if (config->setting("MainWindowMaximized")->boolean())
	{
		mainWnd->showMaximized();
	}
	else
	{
		mainWnd->show();
	}

	mainWindow = mainWnd;
}

bool Main::createRemoteConsole()
{
	gLog << tr("Starting RCon client.");
	if(rconPluginName.isEmpty())
	{
		RemoteConsole *rc = new RemoteConsole();
		if(rc->isValid())
			rc->show();
	}
	else
	{
		// Find plugin
		int pIndex = enginePlugins->pluginIndexFromName(rconPluginName);
		if(pIndex == -1)
		{
			gLog << tr("Couldn't find specified plugin: ") + rconPluginName;
			return false;
		}

		// Check for RCon Availability.
		const EnginePlugin *plugin = (*enginePlugins)[pIndex]->info->pInterface;
		Server *server = plugin->server(QHostAddress(rconAddress), rconPort);
		if(!server->hasRcon())
		{
			delete server;
			gLog << tr("Plugin does not support RCon.");
			return false;
		}

		// Start it!
		RemoteConsole *rc = new RemoteConsole(server);
		rc->show();
	}
	return true;
}

void Main::initDataDirectories()
{
	dataDirectories << "./";
#if defined(Q_OS_LINUX)
	#ifndef INSTALL_PREFIX // For safety lets check for the defintion
		#define INSTALL_PREFIX "/usr"
	#endif
	// check in /usr/local/share/doomseeker/ on Linux
	dataDirectories << INSTALL_PREFIX "/share/doomseeker/";
#endif

	dataDirectories << ":/";
	QDir::setSearchPaths("data", dataDirectories);
}

int Main::initIP2C()
{
	const QString IP2C_FILENAME = "data:IpToCountry.csv";
	const QUrl IP2C_URL = QUrl("http://software77.net/geo-ip?DL=1");

	gLog << tr("Initializing IP2C database.");
	ip2c = new IP2C(IP2C_FILENAME, IP2C_URL);

	if(updateIP2CAndQuit)
	{
		gLog << tr("Starting the IP2C updater.");
		// We'll use a small window to display the update progress.
		QMainWindow updateProgressBox;
		updateProgressBox.setWindowTitle(tr("IP2C Updater"));

		QLabel* label = new QLabel(tr("Updating the IP2C database...\nOnce the progress bar disappears you may close this window."));

		updateProgressBox.setCentralWidget(label);
		if(QMessageBox::question(&updateProgressBox, tr("IP2C Updater"), tr("Update the IP2C database now?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
		{
			updateProgressBox.show();
			ip2c->downloadDatabase(updateProgressBox.statusBar());
			connect(ip2c, SIGNAL(databaseUpdated()), application, SLOT(quit()));
			return application->exec();
		}
		return 0;
	}

	return 0;
}

void Main::initMainConfig()
{
	gLog << tr("Initializing configuration file.");
	config->locateConfigFile(argumentsCount, arguments);

	// Initial settings values
	config->createSetting("CustomServersColor", (0x94 << 16) | (0xff << 8) | (0xff)); // r | g | b
	config->createSetting("MainWindowMaximized", 0);
	config->createSetting("UseTrayIcon", false); // tray icon
	config->createSetting("CloseToTrayIcon", false); // tray icon
	config->createSetting("QueryAutoRefreshEnabled", false);
	config->createSetting("QueryAutoRefreshEverySeconds", 180);
	config->createSetting("QueryAutoRefreshDontIfActive", true);
	config->createSetting("QueryOnStartup", true);
	config->createSetting("QueryTries", 7);
	config->createSetting("QueryTimeout", 1000);
	config->createSetting("TellMeWhereAreTheWADsWhenIHoverCursorOverWADSColumn", true);	
	QStringList urlList = Wadseeker::defaultSitesListEncoded();
	config->createSetting("WadseekerSearchURLs", urlList.join(";"));
	config->createSetting("WadseekerSearchInIdgames", true);
	config->createSetting("WadseekerIdgamesPriority", 0); // 0 == After all other sites
	config->createSetting("WadseekerIdgamesURL", Wadseeker::defaultIdgamesUrl());
	config->createSetting("WadseekerConnectTimeoutSeconds", WADSEEKER_CONNECT_TIMEOUT_SECONDS_DEFAULT);
	config->createSetting("WadseekerDownloadTimeoutSeconds", WADSEEKER_DOWNLOAD_TIMEOUT_SECONDS_DEFAULT);
}

void Main::initPluginConfig()
{
	gLog << tr("Initializing configuration for plugins.");
	enginePlugins->initConfig();
}

bool Main::interpretCommandLineParameters()
{
	for(int i = 0; i < argumentsCount; ++i)
	{
		if(strcmp(arguments[i], "--datadir") == 0 && i+1 < argumentsCount)
		{
			++i;
			dataDirectories.prepend(arguments[i]);
		}
		else if(strcmp(arguments[i], "--rcon") == 0)
		{
			startRcon = true;
			if(i+2 < argumentsCount)
			{
				rconPluginName = arguments[i+1];
				Strings::translateServerAddress(arguments[i+2], rconAddress, rconPort, "localhost", 10666);
				i += 2;
			}
		}
		else if(strcmp(arguments[i], "--updateip2c") == 0)
		{
			updateIP2CAndQuit = true;
		}
		else if(strcmp(arguments[i], "--help") == 0)
		{
			gLog.setTimestampsEnabled(false);
			// Print information to the log and terminate.
			gLog << tr("Available command line parameters:");
			gLog << tr("	--datadir : Sets an explicit search location for IP2C data along with plugins.");
			gLog << tr("	--rcon [plugin] [ip] : Launch the rcon client for the specified ip.");
			gLog << tr("	--updateip2c : Updates the IP2C database.");
			return false;
		}
	}

	QString firstArg = arguments[0];
	int lastSlash = qMax<int>(firstArg.lastIndexOf('\\'), firstArg.lastIndexOf('/'));
	if(lastSlash != -1)
	{
		QString workingDir = firstArg.mid(0, lastSlash+1);
		Main::workingDirectory = Strings::trim(workingDir, "\"");
	}
	dataDirectories << Main::workingDirectory;

	return true;
}

void Main::setupRefreshingThread()
{
	gLog << tr("Starting refreshing thread.");
	refreshingThread->setDelayBetweenResends(config->setting("QueryTimeout")->integer());
	refreshingThread->start();
}

//==============================================================================

#ifdef _MSC_VER
	#ifndef _DEBUG
		#define USE_WINMAIN_AS_ENTRY_POINT
	#endif
#endif

#ifdef USE_WINMAIN_AS_ENTRY_POINT
#include <windows.h>
void getCommandLineArgs(QStringList& outList)
{
	QString commandLine = GetCommandLineA();
	bool bIsInsideQuotationMarks = false;
	int indexCopyFrom = 0;

	for (int i = 0; i < commandLine.length(); ++i)
	{
		if (!bIsInsideQuotationMarks)
		{
			if (commandLine[i] == ' ' || commandLine[i] == '\t')
			{
				QString parameter = commandLine.mid(indexCopyFrom, i - indexCopyFrom).trimmed();
				if (!parameter.isEmpty())
				{
					outList << parameter;
				}
				indexCopyFrom = i + 1;
			}
		}

		if (commandLine[i] == '\"')
		{
			bIsInsideQuotationMarks = !bIsInsideQuotationMarks;
		}
	}

	if (indexCopyFrom < commandLine.length())
	{
		// Get the last parameter.
		// This one shouldn't be covered by the loop above.
		QString lastParameter = commandLine.mid(indexCopyFrom).trimmed();

		// Better be safe than sorry though.
		if (!lastParameter.isEmpty())
		{
			outList << lastParameter;
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)
{
	int argc = 0;
	char** argv = NULL;

	// Good job Microsoft. Now I have to work around your decision of removing
	// useful argc/argv parameters.
	QStringList commandLine;
	getCommandLineArgs(commandLine);

	// At least one is ensured to be here.
	argc = commandLine.size();
	argv = new char*[argc];

	for (int i = 0; i < commandLine.size(); ++i)
	{
		const QString& parameter = commandLine[i];
		argv[i] = new char[parameter.size() + 1];
		strcpy(argv[i], parameter.toAscii().constData());
	}
	
	Main* pMain = new Main(argc, argv);
	int returnValue = pMain->run();

	// Cleans up after the program.
	delete pMain;

	// On the other hand we could just ignore the fact that this array is left
	// hanging in the memory because Windows will clean it up for us...
	for (int i = 0; i < argc; ++i)
	{
		delete [] argv[i];
	}
	delete [] argv;
	

	return returnValue;
}
#else
int main(int argc, char* argv[])
{
	Main* pMain = new Main(argc, argv);
	int returnValue = pMain->run();

	// Cleans up after the program.
	delete pMain;

	return returnValue;
}
#endif


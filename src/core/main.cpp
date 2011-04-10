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
#include <QHashIterator>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QThreadPool>

#include "configuration/doomseekerconfig.h"
#include "gui/mainwindow.h"
#include "gui/remoteconsole.h"
#include "irc/configuration/ircconfig.h"
#include "doomseekerfilepaths.h"
#include "ini.h"
#include "ip2cparser.h"
#include "log.h"
#include "main.h"
#include "sdeapi/config.hpp"
#include "serverapi/server.h"
#include "strings.h"
#include "tests/testruns.h"
#include "wadseeker/wadseeker.h"

const QString		Main::DOOMSEEKER_CONFIG_FILENAME = "doomseeker.cfg";
const QString		Main::DOOMSEEKER_INI_FILENAME = "doomseeker.ini";
const QString		Main::DOOMSEEKER_IRC_INI_FILENAME = "doomseeker-irc.ini";
const QString		Main::IP2C_FILENAME = "IpToCountry.csv";

DataPaths*			Main::dataPaths;
PluginLoader* 		Main::enginePlugins = NULL;
IP2C*				Main::ip2c = NULL;
QWidget*			Main::mainWindow = NULL;
RefreshingThread*	Main::refreshingThread = new RefreshingThread();
bool				Main::running = true;
QString				Main::workingDirectory = "./";

Main::Main(int argc, char* argv[])
: application(NULL), arguments(argv), argumentsCount(argc),
  startRcon(false)
{
	bTestMode = false;
	bPortableMode = false;
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

	// We can't save a config if we haven't initalized the program!
	if(application != NULL)
	{
		gConfig.saveToFile();
		gConfig.dispose();
		
		gIRCConfig.saveToFile();
		gIRCConfig.dispose();
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

	if (dataPaths != NULL)
	{
		delete dataPaths;
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

	application = new QApplication(argumentsCount, arguments);

	gLog << "Starting Doomseeker. Hello World! :)";
	gLog << "Setting up data directories.";

	if (!initDataDirectories())
	{
		// Inform the user which directories cannot be created and QUIT.
		QStringList failedDirsList = dataPaths->directoriesExist();
		QString failedDirsString = failedDirsList.join("\n");

		QString errorMessage = tr("Doomseeker will not run because following directories cannot be created:");
		errorMessage += "\n" + failedDirsString;

		QMessageBox::critical(NULL, tr("Doomseeker startup error"), errorMessage);
		return 0;
	}

	enginePlugins = new PluginLoader(MAKEID('E','N','G','N'), dataDirectories, "engines/");

	if (bTestMode)
	{
		return runTestMode();
	}

	int ip2cReturn = initIP2C();

	convertCfgToIni();
	initMainConfig();
	initPluginConfig();
	initIRCConfig();

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

int Main::runTestMode()
{
	// Setup
	gLog << "Entering test mode.";
	gLog << "";
	TestCore testCore;

	// Call tests here.
	TestRuns::pTestCore = &testCore;
	TestRuns::callTests();

	// Summary
	QString strSucceded   = "Tests succeeded: %1";
	QString strFailed     = "Tests failed:    %1";
	QString strPercentage = "Pass percentage: %1%";

	float passPercentage = (float)testCore.numTestsSucceeded() / (float)testCore.numTests();
	passPercentage *= 100.0f;

	gLog << "==== TESTS SUMMARY: ====";
	gLog << strSucceded.arg(testCore.numTestsSucceeded(), 6);
	gLog << strFailed.arg(testCore.numTestsFailed(), 6);
	gLog << strPercentage.arg(passPercentage, 6, 'f', 2);
	gLog << "==== Done.          ====";

	return testCore.numTestsFailed();
}

void Main::convertCfgToIni()
{
	// Check to see if an old config exists
	QString configFile = Main::dataPaths->programsDataDirectoryPath() + "/" + DOOMSEEKER_CONFIG_FILENAME;
	if(!QFileInfo(configFile).exists())
		return;

	gLog << "Converting old configuration file.";

	const unsigned int NUM_SECTIONS = 7;
	static const char* const sectionNames[7] =
	{
		"Doomseeker",
		"Wadseeker",
		"Chocolate Doom",
		"Odamex",
		"Skulltag",
		"Vavoom",
		"ZDaemon"
	};
	IniSection *sections[7];

	Config oldConfig;
	// This method will only execute it's behavior if there is anything to
	// salvage and nothing to overwrite.
	preserveOldConfigBackwardsCompatibility();
	oldConfig.locateConfigFile(DOOMSEEKER_CONFIG_FILENAME);
	
	QString configDirPath = dataPaths->programsDataDirectoryPath();
	QString iniFilePath = configDirPath + "/" + DOOMSEEKER_INI_FILENAME;
	
	Ini ini(iniFilePath);

	for(unsigned short i = 0;i < NUM_SECTIONS;i++)
	{
		sections[i] = &ini.createSection(sectionNames[i]);
	}

	const QHash<QString, SettingsData *> settings = oldConfig.getSettings();
	QHashIterator<QString, SettingsData *> iter(settings);
	while(iter.hasNext())
	{
		iter.next();

		IniSection *section = sections[0];
		QString key = iter.key();
		for(unsigned short i = 1;i < NUM_SECTIONS;i++)
		{
			if(key.startsWith(sectionNames[i]))
			{
				section = sections[i];
				key = key.right(key.length() - strlen(sectionNames[i]));
				break;
			}
		}
		if(iter.value()->type() == SettingsData::ST_STR)
			section->createSetting(key, iter.value()->string());
		else
			section->createSetting(key, iter.value()->integer());
	}

	oldConfig.remove();

	// Allow Doomseeker to re-create following settings from scratch:
	ini.deleteSetting("Doomseeker", "CustomServersColor");
}

void Main::createMainWindow()
{
	gLog << tr("Preparing GUI.");

	MainWindow* mainWnd = new MainWindow(application, argumentsCount, arguments);
	if (gConfig.doomseeker.bMainWindowMaximized)
	{
		mainWnd->showMaximized();
	}
	else
	{
		mainWnd->show();
	}

	mainWindow = mainWnd;

	mainWnd->notifyFirstRun();
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

bool Main::initDataDirectories()
{
	dataPaths = new DataPaths(bPortableMode);
	DoomseekerFilePaths::pDataPaths = dataPaths;
	if (!dataPaths->createDirectories())
	{
		return false;
	}

	// I think this directory should take priority, if user, for example,
	// wants to update the ip2country file.
	dataDirectories << dataPaths->programsDataDirectoryPath();
	dataDirectories << Main::workingDirectory;

	// Continue with standard dirs:
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

	return true;
}

int Main::initIP2C()
{
	const QString IP2C_FILENAME = "data:IpToCountry.csv";
	const QUrl IP2C_URL = QUrl();

	gLog << tr("Initializing IP2C database.");
	ip2c = new IP2C();

	return 0;
}

void Main::initIRCConfig()
{
	gLog << tr("Initializing IRC configuration file.");
	
	// This macro initializes the Singleton.
	gIRCConfig;
	
	// Now try to access the configuration stored on drive.
	QString configDirPath = dataPaths->programsDataDirectoryPath();
	if (configDirPath.isEmpty())
	{
		return;
	}

	QString filePath = configDirPath + "/" + DOOMSEEKER_IRC_INI_FILENAME;
	if (gIRCConfig.setIniFile(filePath))
	{
		gIRCConfig.readFromFile();
	}
}

void Main::initMainConfig()
{
	gLog << tr("Initializing configuration file.");
	
	// This macro initializes the Singleton.
	gConfig;
	
	// Now try to access the configuration stored on drive.
	QString configDirPath = dataPaths->programsDataDirectoryPath();
	if (configDirPath.isEmpty())
	{
		gLog << tr("Could not get an access to the settings directory. Configuration will not be saved.");
		return;
	}

	QString filePath = configDirPath + "/" + DOOMSEEKER_INI_FILENAME;
	if (gConfig.setIniFile(filePath))
	{
		gConfig.readFromFile();
	}
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
		const char* arg = arguments[i];

		if(strcmp(arg, "--datadir") == 0 && i+1 < argumentsCount)
		{
			++i;
			dataDirectories.prepend(arguments[i]);
		}
		else if(strcmp(arg, "--rcon") == 0)
		{
			startRcon = true;
			if(i+2 < argumentsCount)
			{
				rconPluginName = arguments[i+1];
				Strings::translateServerAddress(arguments[i+2], rconAddress, rconPort, "localhost", 10666);
				i += 2;
			}
		}
		else if(strcmp(arg, "--help") == 0)
		{
			gLog.setTimestampsEnabled(false);
			// Print information to the log and terminate.
			gLog << tr("Available command line parameters:");
			gLog << tr("	--datadir : Sets an explicit search location for IP2C data along with plugins.");
			gLog << tr("	--rcon [plugin] [ip] : Launch the rcon client for the specified ip.");
			gLog << tr("	--portable : Starts application in portable mode.");
			return false;
		}
		else if (strcmp(arg, "--portable") == 0)
		{
			bPortableMode = true;
		}
		else if (strcmp(arg, "--tests") == 0)
		{
			bTestMode = true;
		}
	}

	QString firstArg = arguments[0];
	int lastSlash = qMax<int>(firstArg.lastIndexOf('\\'), firstArg.lastIndexOf('/'));
	if(lastSlash != -1)
	{
		QString workingDir = firstArg.mid(0, lastSlash+1);
		Main::workingDirectory = Strings::trim(workingDir, "\"");
	}

	return true;
}

void Main::preserveOldConfigBackwardsCompatibility()
{
	QFileInfo oldConfigLocation(Main::workingDirectory + "/" + DOOMSEEKER_CONFIG_FILENAME);
	QFileInfo newConfigLocation(dataPaths->programsDataDirectoryPath() + "/" + DOOMSEEKER_CONFIG_FILENAME);

	// QFile::copy() is said not to overwrite existing files but let's check
	// anyway.
	if (oldConfigLocation.exists() && oldConfigLocation.isFile() && !newConfigLocation.exists())
	{
		// If this fails just forget about it.
		QFile::copy(oldConfigLocation.absoluteFilePath(), newConfigLocation.absoluteFilePath());
	}
}

void Main::setupRefreshingThread()
{
	gLog << tr("Starting refreshing thread.");
	refreshingThread->setDelayBetweenResends(gConfig.doomseeker.queryTimeout);
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

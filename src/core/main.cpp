//------------------------------------------------------------------------------
// main.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 Braden "Blzut3" Obrzut <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QHashIterator>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QSslConfiguration>
#include <QThreadPool>
#include <QTimer>

#include <cstdio>

#include "configuration/doomseekerconfig.h"
#include "configuration/passwordscfg.h"
#include "configuration/queryspeed.h"
#include "connectionhandler.h"
#include "gui/mainwindow.h"
#include "gui/remoteconsole.h"
#include "ip2c/ip2c.h"
#include "ini/ini.h"
#include "irc/configuration/ircconfig.h"
#include "serverapi/server.h"
#include "application.h"
#include "cmdargshelp.h"
#include "commandlinetokenizer.h"
#include "datapaths.h"
#include "doomseekerfilepaths.h"
#include "localization.h"
#include "log.h"
#include "main.h"
#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"
#include "refresher/refresher.h"
#include "serverapi/server.h"
#include "strings.hpp"
#include "tests/testruns.h"
#include "wadseeker/wadseeker.h"
#include "updater/updateinstaller.h"
#include "versiondump.h"

QString Main::argDataDir;
bool Main::bInstallUpdatesAndRestart = false;
QList<LocalizationInfo> Main::localizations;


Main::Main(int argc, char* argv[])
: arguments(argv), argumentsCount(argc),
  startRcon(false)
{
	bIsFirstRun = false;
	bTestMode = false;
	bPortableMode = false;
	bVersionDump = false;
	logVerbosity = LV_Default;
	updateFailedCode = 0;

	qRegisterMetaType<ServerPtr>("ServerPtr");
	qRegisterMetaType<ServerCPtr>("ServerCPtr");
}

Main::~Main()
{
	if (Application::isInit())
	{
		gApp->stopRunning();
	}

	if (Refresher::isInstantiated())
	{
		Refresher::instance()->quit();
		Refresher::deinstantiate();
	}

	// We can't save a config if we haven't initalized the program!
	if (Application::isInit())
	{
		gConfig.saveToFile();
		gConfig.dispose();

		gIRCConfig.saveToFile();
		gIRCConfig.dispose();
	}

	IP2C::deinstantiate();

	PluginLoader::deinit();
	Application::deinit();
}

int Main::connectToServerByURL()
{
	ConnectionHandler *handler = ConnectionHandler::connectByUrl(connectUrl);

	if(handler)
	{
		connect(handler, SIGNAL(finished(int)), gApp, SLOT(quit()));
		handler->run();
		int ret = gApp->exec();
		delete handler;
		return ret;
	}
	return 0;
}

// This method is an exception to sorting everything in alphabetical order
// because it's... the main method.
int Main::run()
{
	if (!interpretCommandLineParameters())
	{
		return 0;
	}
	applyLogVerbosity();

	Application::init(argumentsCount, arguments);
#ifdef Q_OS_MAC
	// In Mac OS X it is abnormal to have menu icons unless it's a shortcut to a file of some kind.
	gApp->setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

	gLog << "Starting Doomseeker. Hello World! :)";
	gLog << "Setting up data directories.";

	if (!initDataDirectories())
	{
		// Inform the user which directories cannot be created and QUIT.
		QStringList failedDirsToCreateList = gDefaultDataPaths->directoriesExist();
		// Also, in case the permissions are incorrect inform more properly the user.
		QStringList failedDirsPermissionsList = gDefaultDataPaths->directoriesWithoutPermissions();

		// We will transform the list to text. In case a list is empty, we should give a more acurate error message
		// of what is going wrong.
		QString failedDirsToCreateString = failedDirsToCreateList.join("\n");
		QString failedDirsPermissionsString = failedDirsPermissionsList.join("\n");
		// Case where dir cannot be crated
		if (failedDirsToCreateString != "")
		{
			QString errorMessage = tr("Doomseeker will not run because following directories cannot be created:");
			errorMessage += "\n" + failedDirsToCreateString;
			QMessageBox::critical(NULL, tr("Doomseeker startup error"), errorMessage);
			return 0;
		}
		// Case where dir has unproper permissions
		else if (failedDirsPermissionsString != "")
		{
			QString errorMessage = tr("Doomseeker will not run because following directories have incorrect permissions:");
			errorMessage += "\n" + failedDirsPermissionsString;
			QMessageBox::critical(NULL, tr("Doomseeker startup error"), errorMessage);
			return 0;
		}
		// Other unexpected cases (These lines of code should never be reached.)
		else
		{
			QString errorMessage = tr("Doomseeker will not run because some directories cannot be used properly");
			QMessageBox::critical(NULL, tr("Doomseeker startup error"), errorMessage);
			return 0;
		}
	}

	initCaCerts();

	PluginLoader::init(gDefaultDataPaths->pluginSearchLocationPaths());
	if (bVersionDump)
	{
		return runVersionDump();
	}
	PluginUrlHandler::registerAll();

	if (bTestMode)
	{
		return runTestMode();
	}

	initMainConfig();
	#ifdef WITH_AUTOUPDATES
	// Handle pending update installations.
	UpdateInstaller::ErrorCode updateInstallerResult
		= (UpdateInstaller::ErrorCode)installPendingUpdates();
	if (updateInstallerResult == UpdateInstaller::EC_Ok)
	{
		return 0;
	}
	#endif

	initLocalizationsDefinitions();
	initIP2C();
	initPasswordsConfig();
	initPluginConfig();
	initIRCConfig();

	if (startRcon)
	{
		QTimer::singleShot(0, this, SLOT(runRemoteConsole()));
	}
	else if (connectUrl.isValid())
	{
		setupRefreshingThread();
		return connectToServerByURL();
	}
	else
	{
		setupRefreshingThread();
		createMainWindow();
		#ifdef WITH_AUTOUPDATES
		// Handle auto update: display update failure or start auto update
		// check/download.
		if (updateFailedCode != 0)
		{
			// This is when updater program failed to install the update.
			gApp->mainWindow()->setDisplayUpdaterProcessFailure(updateFailedCode);
		}
		else if (updateInstallerResult != UpdateInstaller::EC_NothingToUpdate)
		{
			// This is when Doomseeker failed to start the updater program.
			gApp->mainWindow()->setDisplayUpdateInstallerError(updateInstallerResult);
		}
		else
		{
			if (gConfig.autoUpdates.updateMode != DoomseekerConfig::AutoUpdates::UM_Disabled)
			{
				QTimer::singleShot(0, gApp->mainWindow(), SLOT(checkForUpdatesAuto()));
			}
		}
		#endif
	}

	gLog << tr("Init finished.");
	gLog.addUnformattedEntry("================================\n");

	int returnCode = gApp->exec();

	#ifdef WITH_AUTOUPDATES
	if (bInstallUpdatesAndRestart)
	{
		// Code must be reset because the install method
		// doesn't do the actual installation if it's not equal to zero.
		updateFailedCode = 0;
		int installResult = installPendingUpdates();
		if (installResult != UpdateInstaller::EC_Ok
			&& installResult != UpdateInstaller::EC_NothingToUpdate)
		{
			QMessageBox::critical(NULL, tr("Doomseeker - Updates Install Failure"),
				UpdateInstaller::errorCodeToStr((UpdateInstaller::ErrorCode)installResult));
		}
	}
	#endif

	return returnCode;
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

int Main::runVersionDump()
{
	QFile outfile;
	QString error;
	if (!versionDumpFile.isEmpty())
	{
		outfile.setFileName(versionDumpFile);
		if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			error = tr("Failed to open file '%1'.").arg(versionDumpFile);
		}
	}
	else
	{
		// Use stdout instead.
		if (!outfile.open(stdout, QIODevice::WriteOnly))
		{
			error = tr("Failed to open stdout.");
		}
	}
	if (!error.isEmpty())
	{
		gLog.setPrintingToStderr(true);
		gLog << error;
		return 2;
	}

	gLog << tr("Dumping version info to file in JSON format.");
	VersionDump::dumpJsonToIO(outfile);
	return 0;
}

void Main::applyLogVerbosity()
{
	gLog.setPrintingToStderr(shouldLogToStderr());
}

void Main::createMainWindow()
{
	gLog << tr("Preparing GUI.");

	gApp->setMainWindow(new MainWindow(gApp, argumentsCount, arguments));
	gApp->mainWindow()->show();

	if (bIsFirstRun)
	{
		gApp->mainWindow()->notifyFirstRun();
	}
}

void Main::runRemoteConsole()
{
	gLog << tr("Starting RCon client.");
	if(rconPluginName.isEmpty())
	{
		bool canAnyEngineRcon = false;
		for(unsigned int i = 0;i < gPlugins->numPlugins();i++)
		{
			const EnginePlugin* info = gPlugins->plugin(i)->info();
			if (info->server(QHostAddress("localhost"), 0)->hasRcon())
			{
				canAnyEngineRcon = true;
				break;
			}
		}
		if (!canAnyEngineRcon)
		{
			QString error = tr("None of the currently loaded game plugins supports RCon.");
			gLog << error;
			QMessageBox::critical(NULL, tr("Doomseeker RCon"), error);
			gApp->exit(2);
			return;
		}

		RemoteConsole *rc = new RemoteConsole();
		rc->show();
	}
	else
	{
		// Find plugin
		int pIndex = gPlugins->pluginIndexFromName(rconPluginName);
		if(pIndex == -1)
		{
			gLog << tr("Couldn't find specified plugin: ") + rconPluginName;
			gApp->exit(2);
			return;
		}

		// Check for RCon Availability.
		const EnginePlugin *plugin = gPlugins->plugin(pIndex)->info();
		ServerPtr server = plugin->server(QHostAddress(rconAddress), rconPort);
		if(!server->hasRcon())
		{
			gLog << tr("Plugin does not support RCon.");
			gApp->exit(2);
			return;
		}

		// Start it!
		RemoteConsole *rc = new RemoteConsole(server);
		rc->show();
	}
}

void Main::initCaCerts()
{
	QString certsFilePath = DoomseekerFilePaths::cacerts();
	QFile certsFile(certsFilePath);
	if (!certsFilePath.isEmpty() && certsFile.exists())
	{
		gLog << tr("Loading extra CA certificates from '%1'.").arg(certsFilePath);
		certsFile.open(QIODevice::ReadOnly);
		QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
		QList<QSslCertificate> cacerts = sslConf.caCertificates();
		QList<QSslCertificate> extraCerts = QSslCertificate::fromDevice(&certsFile);
		gLog << tr("Appending %n extra CA certificate(s).", NULL, extraCerts.size());
		cacerts.append(extraCerts);
		sslConf.setCaCertificates(cacerts);
		QSslConfiguration::setDefaultConfiguration(sslConf);
		certsFile.close();
	}
}

bool Main::initDataDirectories()
{
	DataPaths::initDefault(bPortableMode);
	DoomseekerFilePaths::pDataPaths = gDefaultDataPaths;
	if (!gDefaultDataPaths->createDirectories())
	{
		return false;
	}

	// I think this directory should take priority, if user, for example,
	// wants to update the ip2country file.
	dataDirectories << gDefaultDataPaths->localDataLocationPath();
	dataDirectories << gDefaultDataPaths->workingDirectory();

	// Continue with standard dirs:
	dataDirectories << "./";
#if defined(Q_OS_LINUX)
	// check in /usr/local/share/doomseeker/ on Linux
	dataDirectories << INSTALL_PREFIX "/share/doomseeker/";
#endif

	dataDirectories << ":/";
	QDir::setSearchPaths("data", dataDirectories);

	return true;
}

int Main::initIP2C()
{
	gLog << tr("Initializing IP2C database.");
	IP2C::instance();

	return 0;
}

void Main::initIRCConfig()
{
	gLog << tr("Initializing IRC configuration file.");

	// This macro initializes the Singleton.
	gIRCConfig;

	// Now try to access the configuration stored on drive.
	QString configPath = DoomseekerFilePaths::ircIni();
	if (!configPath.isEmpty())
	{
		if (gIRCConfig.setIniFile(configPath))
		{
			gIRCConfig.readFromFile();
		}
	}
}

void Main::initLocalizationsDefinitions()
{
	gLog << tr("Loading translations definitions");
	localizations = Localization::loadLocalizationsList(
		DataPaths::staticDataSearchDirs(DataPaths::TRANSLATIONS_DIR_NAME));

	QString localization = gConfig.doomseeker.localization;
	gLog << tr("Loading translation \"%1\".").arg(localization);
	bool bSuccess = Localization::loadTranslation(localization);
	if (bSuccess)
	{
		gLog << tr("Translation loaded.");
	}
	else
	{
		gLog << tr("Failed to load translation.");
	}
}

void Main::initMainConfig()
{
	gLog << tr("Initializing configuration file.");

	// This macro initializes the Singleton.
	gConfig;

	// Now try to access the configuration stored on drive.
	QString configDirPath = gDefaultDataPaths->programsDataDirectoryPath();
	if (configDirPath.isEmpty())
	{
		gLog << tr("Could not get an access to the settings directory. Configuration will not be saved.");
		return;
	}

	QString filePath = DoomseekerFilePaths::ini();

	// Check for first run.
	QFileInfo iniFileInfo(filePath);
	bIsFirstRun = !iniFileInfo.exists();

	// Init the config.
	if (gConfig.setIniFile(filePath))
	{
		gConfig.readFromFile();
	}
}

void Main::initPasswordsConfig()
{
	gLog << tr("Initializing passwords configuration file.");
	// Now try to access the configuration stored on drive.
	QString configDirPath = gDefaultDataPaths->programsDataDirectoryPath();
	if (configDirPath.isEmpty())
	{
		return;
	}
	QString filePath = DoomseekerFilePaths::passwordIni();
	PasswordsCfg::initIni(filePath);
}

void Main::initPluginConfig()
{
	gLog << tr("Initializing configuration for plugins.");
	gPlugins->initConfig();
}

int Main::installPendingUpdates()
{
	UpdateInstaller::ErrorCode updateInstallerResult = UpdateInstaller::EC_NothingToUpdate;
	if (gConfig.autoUpdates.bPerformUpdateOnNextRun)
	{
		gConfig.autoUpdates.bPerformUpdateOnNextRun = false;
		gConfig.saveToFile();
		// Update should only be attempted if program was not called
		// with "--update-failed" arg (previous update didn't fail).
		if (updateFailedCode == 0)
		{
			UpdateInstaller updateInstaller;
			updateInstallerResult = updateInstaller.startInstallation();
		}
	}
	return updateInstallerResult;
}

bool Main::interpretCommandLineParameters()
{
	//first argument is the command to run the program, example: ./doomseeker. better use 1 instead of 0
	for(int i = 1; i < argumentsCount; ++i)
	{
		const char* arg = arguments[i];

		if(strcmp(arg, "--connect") == 0)
		{
			if (i+1 < argumentsCount)
			{
				++i;
				connectUrl = QUrl(arguments[i]);
			}
			else
			{
				//basically prevent the program from running if there are no arguments given.
				gLog.setTimestampsEnabled(false);
				gLog << tr("doomseeker: expected one argument in option %1").arg(arg);
				logHelpOnBadRun();
				return false;
			}
		}
		else if(strcmp(arg, "--datadir") == 0)
		{
			if (i+1 < argumentsCount)
			{
				++i;
				dataDirectories.prepend(arguments[i]);
				argDataDir = arguments[i];
			}
			else
			{
				gLog.setTimestampsEnabled(false);
				gLog << tr("doomseeker: expected one argument in option %1").arg(arg);
				logHelpOnBadRun();
				return false;
			}
		}
		else if(strcmp(arg, "--rcon") == 0)
		{
			startRcon = true;
			if(i+2 < argumentsCount)
			{
				rconPluginName = arguments[++i];
				Strings::translateServerAddress(arguments[++i], rconAddress, rconPort, "localhost:10666");
			}
			else
			{
				gLog.setTimestampsEnabled(false);
				gLog << tr("doomseeker: expected two arguments in option %1").arg(arg);
				logHelpOnBadRun();
				return false;
			}
		}
		else if(strcmp(arg, "--help") == 0)
		{
			gLog.setTimestampsEnabled(false);
			// Print information to the log and terminate.
			gLog << tr("Available command line parameters:\n");
			gLog << CmdArgsHelp::argsHelp();
			return false;
		}
		else if (strcmp(arg, "--update-failed") == 0)
		{
			++i;
			updateFailedCode = QString(arguments[i]).toInt();
		}
		else if (strcmp(arg, "--portable") == 0)
		{
			bPortableMode = true;
		}
		else if (strcmp(arg, "--quiet") == 0)
		{
			logVerbosity = LV_Quiet;
		}
		else if (strcmp(arg, "--tests") == 0)
		{
			bTestMode = true;
		}
		else if (strcmp(arg, "--verbose") == 0)
		{
			logVerbosity = LV_Verbose;
		}
		else if (strcmp(arg, "--version-json") == 0)
		{
			bVersionDump = true;
			if (i + 1 < argumentsCount)
			{
				++i;
				QString filename = arguments[i];
				if (filename != "-" && filename != "")
				{
					versionDumpFile = filename;
				}
			}

		}
		else
		{
			gLog.setTimestampsEnabled(false);
			gLog << tr("doomseeker: unrecognized option '%1'").arg(arg);
			logHelpOnBadRun();
			return false;
		}
	}

	return true;
}

void Main::logHelpOnBadRun()
{
    gLog << tr("Available command line parameters:\n");
    gLog << CmdArgsHelp::argsHelp();
}

void Main::setupRefreshingThread()
{
	gLog << tr("Starting refreshing thread.");
	gRefresher->setDelayBetweenResends(gConfig.doomseeker.querySpeed().delayBetweenSingleServerAttempts);
	gRefresher->start();
}

bool Main::shouldLogToStderr() const
{
	if (bTestMode)
		return logVerbosity != LV_Quiet;
	if (bVersionDump)
		return logVerbosity == LV_Verbose;
	return logVerbosity != LV_Quiet;
}

//==============================================================================

#ifdef _MSC_VER
	#ifdef NDEBUG
		#define USE_WINMAIN_AS_ENTRY_POINT
	#endif
#endif

#ifdef USE_WINMAIN_AS_ENTRY_POINT
#include <windows.h>
QStringList getCommandLineArgs()
{
	CommandLineTokenizer tokenizer;
	return tokenizer.tokenize(QString::fromUtf16((const ushort*)GetCommandLineW()));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)
{
	int argc = 0;
	char** argv = NULL;

	QStringList commandLine = getCommandLineArgs();

	// At least one is ensured to be here.
	argc = commandLine.size();
	argv = new char*[argc];

	for (int i = 0; i < commandLine.size(); ++i)
	{
		const QString& parameter = commandLine[i];
		argv[i] = new char[parameter.toUtf8().size() + 1];
		strcpy(argv[i], parameter.toUtf8().constData());
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


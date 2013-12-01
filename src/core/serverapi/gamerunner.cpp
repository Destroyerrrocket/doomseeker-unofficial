//------------------------------------------------------------------------------
// gamerunner.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "plugins/engineplugin.h"
#include "serverapi/gamerunner.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "serverapi/binaries.h"
#include "configuration/doomseekerconfig.h"
#include "gui/standardserverconsole.h"
#include "apprunner.h"
#include "log.h"
#include "main.h"
#include "pathfinder.h"
#include "strings.h"
#include <QDateTime>
#include <QStringList>

class GameRunner::PrivData
{
	public:
		QString argConnect;
		QString argConnectPassword;
		QString argIwadLoading;
		QString argPort;
		QString argPwadLoading;
		QString argDemoPlayback;
		QString argDemoRecord;
		QString argServerLaunch;

		CommandLineInfo* currentCmdLine;
		const HostInfo* currentHostInfo;
		const Server* server;
};

GameRunner::GameRunner(const Server* server)
{
	d = new PrivData();
	d->argConnect = "-connect";
	d->argIwadLoading = "-iwad";
	d->argPort = "-port";
	d->argPwadLoading = "-file";
	d->argDemoPlayback = "-playdemo";
	d->argDemoRecord = "-record";
	d->currentCmdLine = NULL;
	d->currentHostInfo = NULL;
	d->server = server;
}

GameRunner::~GameRunner()
{
	delete d;
}

const QString& GameRunner::argForConnect() const
{
	return d->argConnect;
}

const QString& GameRunner::argForConnectPassword() const
{
	return d->argConnectPassword;
}

const QString& GameRunner::argForIwadLoading() const
{
	return d->argIwadLoading;
}

const QString& GameRunner::argForPort() const
{
	return d->argPort;
}

const QString& GameRunner::argForPwadLoading() const
{
	return d->argPwadLoading;
}

const QString& GameRunner::argForDemoPlayback() const
{
	return d->argDemoPlayback;
}

const QString& GameRunner::argForDemoRecord() const
{
	return d->argDemoRecord;
}

const QString& GameRunner::argForServerLaunch() const
{
	return d->argServerLaunch;
}

bool GameRunner::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword, const QString &wadTargetDirectory)
{
	QString address = QString("%1:%2").arg(d->server->address().toString()).arg(d->server->port());

	// Connect
	args << argForConnect() << address;
	if(d->server->isLocked())
	{
		args << connectPassword;
	}

	// Iwad
	QString iwad = pf.findFile(d->server->iwad().toLower());
	args << argForIwadLoading() << iwad;
	iwadFound = !iwad.isEmpty();

	// Custom parameters
	IniSection config = gConfig.iniSectionForPlugin(d->server->plugin());
	QString customParameters = config["CustomParameters"];
	args << customParameters.split(" ", QString::SkipEmptyParts);

	// Password
	if (d->server->isLocked())
	{
		if (argForConnectPassword().isNull())
		{
			// TODO: Log a warning message here.
		}
		else
		{
			args << argForConnectPassword() << connectPassword;
		}
	}
	return true;
}

Message GameRunner::createHostCommandLine(const HostInfo& hostInfo, CommandLineInfo& cmdLine, HostMode mode)
{
	const QString RUN_RESULT_CAPTION = tr("createHostCommandLine");
	Message message;

	d->currentCmdLine = &cmdLine;
	d->currentHostInfo = &hostInfo;

	cmdLine.args.clear();

	message = hostAppendIwad();
	if (!message.isIgnore())
	{
		return message;
	}

	message = hostAppendPwads();
	if (!message.isIgnore())
	{
		return message;
	}

	// Port
	if(mode == GameRunner::HOST)
		cmdLine.args << argForPort() << QString::number(d->server->port());

	// CVars
	const QList<GameCVar>& cvars = hostInfo.cvars;
	foreach(const GameCVar c, cvars)
	{
		cmdLine.args << QString("+" + c.consoleCommand) << c.value();
	}

	message = hostGetBinary(mode != GameRunner::HOST);
	if (!message.isIgnore())
	{
		return message;
	}

	message = hostGetWorkingDirectory(mode != GameRunner::HOST);
	if (!message.isIgnore())
	{
		return message;
	}

	// Add the server launch parameter only if we don't want offline game
	if (mode == GameRunner::HOST)
	{
		cmdLine.args << argForServerLaunch();
	}
	// Demo play back
	else if (mode == GameRunner::DEMO)
	{
		cmdLine.args << argForDemoPlayback();
		cmdLine.args << hostInfo.demoPath;
	}

	hostDMFlags(cmdLine.args, hostInfo.dmFlags);
	hostProperties(cmdLine.args);
	cmdLine.args.append(hostInfo.customParameters);

	return message;
}

JoinError GameRunner::createJoinCommandLine(CommandLineInfo& cli, const QString &connectPassword, bool managedDemo)
{
	const QString &PLUGIN_NAME = d->server->plugin()->data()->name;
	JoinError joinError;

	// Init the JoinError type with critical error. We will change this upon
	// successful return or if wads are missing.
	joinError.type = JoinError::Critical;

	QDir& applicationDir = cli.applicationDir;
	QFileInfo& executablePath = cli.executable;
	QStringList& args = cli.args;

	//const QString errorCaption = tr("Doomseeker - error for plugin");
	args.clear();

	Binaries* binaries = d->server->binaries();

	Message message;
	QString clientBin = binaries->clientBinary(message);
	if (clientBin.isEmpty())
	{
		joinError.type = JoinError::ConfigurationError;
		joinError.error = tr("Client binary cannot be obtained for %1, please check the location given in the configuration.").arg(PLUGIN_NAME);
		if (!message.isIgnore())
		{
			joinError.error += "\n\n" + message.contents();
		}

		delete binaries;
		return joinError;
	}

	executablePath = clientBin;

	QString offlineGameBinary = binaries->offlineGameBinary(message);
	QString clientWorkingDirPath = binaries->clientWorkingDirectory(message);
	applicationDir = clientWorkingDirPath;

	delete binaries;

	if (clientWorkingDirPath.isEmpty())
	{
		joinError.type = JoinError::ConfigurationError;
		joinError.error = tr("Path to working directory for \"%1\" is empty.\nMake sure the configuration for the main binary is set properly.").arg(PLUGIN_NAME);
		return joinError;
	}
	else if (!applicationDir.exists())
	{
		joinError.type = JoinError::ConfigurationError;
		joinError.error = tr("%1\n cannot be used as working directory for game:\n%2\nExecutable: %3").arg(clientWorkingDirPath, PLUGIN_NAME, clientBin);
		return joinError;
	}

	PathFinder pathFinder;
	// Add the offline game directory so results are more consistent
	// addPrioritySearchDir prepends to the list so we'll want to add the real
	// priority directory second.
	pathFinder.addPrioritySearchDir(offlineGameBinary);
	pathFinder.addPrioritySearchDir(clientBin);

	QStringList missingPwads;
	bool iwadFound = false;

	if(!connectParameters(cli.args, pathFinder, iwadFound, connectPassword, DoomseekerConfig::config().wadseeker.targetDirectory))
	{
		joinError.type = JoinError::Terminate;
		return joinError;
	}

	// Record
	QString demoName;
	if(gConfig.doomseeker.bRecordDemo)
	{
		args << argForDemoRecord();

		// Generate demo name.
		// port-iwad-date-wad
		if(managedDemo)
			demoName = Main::dataPaths->demosDirectoryPath() + QDir::separator();
		demoName += QString("%1_%2").
			arg(d->server->engineName()).
			arg(QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ss"));
		if(!d->server->plugin()->data()->demoExtensionAutomatic)
			demoName += QString(".%1").arg(d->server->plugin()->data()->demoExtension);
		args << demoName;
	}

	for (int i = 0; i < d->server->numWads(); ++i)
	{
		QString pwad = pathFinder.findFile(d->server->wad(i).name);
		if (pwad.isEmpty() && !d->server->wad(i).optional)
		{
			missingPwads << d->server->wad(i).name;
		}
		else
		{
			cli.args << argForPwadLoading();
			cli.args << pwad;
		}
	}

	if (!iwadFound || !missingPwads.isEmpty())
	{
		if (!iwadFound)
		{
			joinError.missingIwad = d->server->iwad();
		}
		joinError.missingWads = missingPwads;
		joinError.type = JoinError::MissingWads;
		return joinError;
	}

	joinError.type = JoinError::NoError;

	// No errors?
	// Nothing should be stopping us from joining so dump the meta information
	// if we are doing a managed demo
	if(!demoName.isEmpty() && managedDemo)
	{
		QString metaFileName;
		// If the extension is automatic we need to add it here
		if(d->server->plugin()->data()->demoExtensionAutomatic)
			metaFileName = QString("%1.%2.ini").arg(demoName).arg(d->server->plugin()->data()->demoExtension);
		else
			metaFileName = demoName + ".ini";

		Ini metaFile(metaFileName);
		IniSection metaSection = metaFile.createSection("meta");

		// Get a list of wads for demo name:
		QStringList wadList;
		for (int i = 0; i < d->server->numWads(); ++i)
		{
			// Also be sure to escape any underscores.
			wadList << d->server->wad(i).name.toLower();
		}

		metaSection.createSetting("iwad", d->server->iwad().toLower());
		metaSection.createSetting("pwads", wadList.join(";"));
	}

	return joinError;
}

Message GameRunner::host(const HostInfo& hostInfo, HostMode mode)
{
	CommandLineInfo cmdLine;

	Message message = createHostCommandLine(hostInfo, cmdLine, mode);
	if (!message.isIgnore())
	{
		return message;
	}

#ifdef Q_OS_WIN32
	const bool WRAP_IN_SSS_CONSOLE = false;
#else
	const bool WRAP_IN_SSS_CONSOLE = mode == HOST;
#endif

	return runExecutable(cmdLine, WRAP_IN_SSS_CONSOLE);
}

Message GameRunner::hostAppendIwad()
{
	const QString RESULT_CAPTION = tr("Doomseeker - host - appending IWAD");
	const QString& iwadPath = d->currentHostInfo->iwadPath;

	Message message;

	if (iwadPath.isEmpty())
	{
		message = Message::customError(tr("Iwad is not set"));
		return message;
	}

	QFileInfo fi(iwadPath);

	if (!fi.isFile())
	{
		QString error = tr("Iwad Path error:\n\"%1\" doesn't exist or is a directory!").arg(iwadPath);
		message = Message::customError(error);
		return message;
	}

	d->currentCmdLine->args << argForIwadLoading() << iwadPath;
	return message;
}

Message GameRunner::hostAppendPwads()
{
	const QString RESULT_CAPTION = tr("Doomseeker - host - appending PWADs");
	const QStringList& pwadsPaths = d->currentHostInfo->pwadsPaths;

	Message message;

	if (!pwadsPaths.isEmpty())
	{
		QStringList& args = d->currentCmdLine->args;
		foreach(const QString pwad, pwadsPaths)
		{
			args << argForPwadLoading();

			QFileInfo fi(pwad);
			if (!fi.isFile())
			{
				QString error = tr("Pwad path error:\n\"%1\" doesn't exist or is a directory!").arg(pwad);
				message = Message::customError(error);
				return message;
			}
			args << pwad;
		}
	}

	return message;
}

Message GameRunner::hostGetBinary(bool bOfflinePlay)
{
	const QString RESULT_CAPTION = tr("Doomseeker - host - getting executable");
	QString executablePath = d->currentHostInfo->executablePath;

	if (executablePath.isEmpty())
	{
		Binaries* binaries = d->server->binaries();

		Message message;

		// Select binary depending on bOfflinePlay flag:
		if (bOfflinePlay)
		{
			executablePath = binaries->offlineGameBinary(message);
		}
		else
		{
			executablePath = binaries->serverBinary(message);
		}

		delete binaries;

		if (executablePath.isEmpty())
		{
			return message;
		}
	}

	QFileInfo fi(executablePath);

	if (!fi.isFile() && !fi.isBundle())
	{
		Message message;
		QString error = tr("%1\n doesn't exist or is not a file.").arg(fi.filePath());
		message = Message::customError(error);
		return message;
	}

	d->currentCmdLine->executable = executablePath;
	return Message();
}

Message GameRunner::hostGetWorkingDirectory(bool bOfflinePlay)
{
	const QString RESULT_CAPTION = tr("Doomseeker - host - getting working directory");
	QString error;
	QString serverWorkingDirPath;

	Message message;
	
	// First, we should try to extract the working dir from plugin.
	// [Zalewa]:
	// A plugin may insist on doing that for a reason that is currently
	// unknown to me. Let's try to predict every possible situation.
	QString workingDirFromPlugin;
	Binaries* binaries = d->server->binaries();
	if (bOfflinePlay)
	{
		workingDirFromPlugin = binaries->offlineGameWorkingDirectory(message);
	}
	else
	{
		workingDirFromPlugin = binaries->serverWorkingDirectory(message);
	}
	
	// Check if all went well on the plugin side.
	if (!message.isIgnore())
	{
		// Something's gone wrong. Report the error.
		return message;
	}
	
	if (workingDirFromPlugin.isEmpty())
	{
		// Assume that working directory is the same as executable's directory.
		// Path to executable should be known at this point.
		QFileInfo fileInfo(d->currentCmdLine->executable);

		serverWorkingDirPath = fileInfo.absolutePath();
	}
	else
	{
		// Plugin returned the directory. Use that.
		serverWorkingDirPath = workingDirFromPlugin;
	}
	
	QDir serverWorkingDir(serverWorkingDirPath);

	if (serverWorkingDirPath.isEmpty())
	{
		QString error = tr("Path to working directory is empty.\nMake sure the configuration for the executable file is set properly.");
		message = Message::customError(error);
		return message;
	}
	else if (!serverWorkingDir.exists())
	{
		QString error = tr("%1\n doesn't exist or is not a directory.").arg(serverWorkingDirPath);
		message = Message::customError(error);
		return message;
	}

	d->currentCmdLine->applicationDir = serverWorkingDir;
	return message;
}

Message GameRunner::runExecutable(const CommandLineInfo& cli, bool bWrapInStandardServerConsole)
{
	if (!bWrapInStandardServerConsole)
	{
		return AppRunner::runExecutable(cli);
	}
	else
	{
		gLog << tr("Starting (working dir %1): %2").arg(cli.applicationDir.absolutePath()).arg(cli.executable.absoluteFilePath());
		QStringList args = cli.args;
		// Is this needed for something? Zandronum needs the quotes for console
		// variables.
		//AppRunner::cleanArguments(args);
		new StandardServerConsole(d->server, cli.executable.absoluteFilePath(), args);
	}

	return Message();
}

void GameRunner::setArgForConnect(const QString& arg)
{
	d->argConnect = arg;
}

void GameRunner::setArgForConnectPassword(const QString& arg)
{
	d->argConnectPassword = arg;
}

void GameRunner::setArgForIwadLoading(const QString& arg)
{
	d->argIwadLoading = arg;
}

void GameRunner::setArgForPort(const QString& arg)
{
	d->argPort = arg;
}

void GameRunner::setArgForPwadLoading(const QString& arg)
{
	d->argPwadLoading = arg;
}

void GameRunner::setArgForDemoPlayback(const QString& arg)
{
	d->argDemoPlayback = arg;
}

void GameRunner::setArgForDemoRecord(const QString& arg)
{
	d->argDemoRecord = arg;
}

void GameRunner::setArgForServerLaunch(const QString& arg)
{
	d->argServerLaunch = arg;
}

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
#include "serverapi/gamerunner.h"
#include "serverapi/server.h"
#include "serverapi/binaries.h"
#include "gui/standardserverconsole.h"
#include "apprunner.h"
#include "log.h"
#include "main.h"
#include "pathfinder.h"
#include "strings.h"
#include <QStringList>

GameRunner::GameRunner(const Server* server)
: currentCmdLine(NULL), currentHostInfo(NULL), server(server)
{
}

void GameRunner::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword)
{
	QString address = QString("%1:%2").arg(server->address().toString()).arg(server->port());

	// Connect
	args << argForConnect() << address;
	if(server->isLocked())
	{
		args << connectPassword;
	}

	// Iwad
	QString iwad = pf.findFile(server->iwadName().toLower());
	args << argForIwadLoading() << iwad;
	iwadFound = !iwad.isEmpty();

	// Custom parameters
	QString customParametersKey = configKeyCustomParameters();
	QString customParameters = Main::config->setting(customParametersKey)->string();
	args << customParameters.split(" ", QString::SkipEmptyParts);

	// Password
	if (server->isLocked())
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
}

MessageResult GameRunner::createHostCommandLine(const HostInfo& hostInfo, CommandLineInfo& cmdLine, bool bOfflinePlay)
{
	const QString RUN_RESULT_CAPTION = tr("createHostCommandLine");
	MessageResult result(false);

	currentCmdLine = &cmdLine;
	currentHostInfo = &hostInfo;

	cmdLine.args.clear();

	result = hostAppendIwad();
	if (result.isError)
	{
		return result;
	}

	result = hostAppendPwads();
	if (result.isError)
	{
		return result;
	}

	// Port
	cmdLine.args << argForPort() << QString::number(server->port());

	// CVars
	const QList<GameCVar>& cvars = hostInfo.cvars;
	foreach(const GameCVar c, cvars)
	{
		cmdLine.args << QString("+" + c.consoleCommand) << c.value();
	}

	result = hostGetBinary(bOfflinePlay);
	if (result.isError)
	{
		return result;
	}

	result = hostGetWorkingDirectory(bOfflinePlay);
	if (result.isError)
	{
		return result;
	}

	// Add the server launch parameter only if we don't want offline game
	if (!bOfflinePlay)
	{
		cmdLine.args << argForServerLaunch();
	}

	hostDMFlags(cmdLine.args, server->gameFlags());
	hostProperties(cmdLine.args);
	cmdLine.args.append(hostInfo.customParameters);

	return MessageResult(false);
}

JoinError GameRunner::createJoinCommandLine(CommandLineInfo& cli, const QString &connectPassword)
{
	JoinError jError;

	// Init the JoinError type with critical error. We will change this upon
	// successful return or if wads are missing.
	jError.type = JoinError::Critical;

	QDir& applicationDir = cli.applicationDir;
	QFileInfo& executablePath = cli.executable;
	QStringList& args = cli.args;

	const QString errorCaption = tr("Doomseeker - error");
	args.clear();

	Binaries* binaries = server->binaries();

	QString clientBin = binaries->clientBinary(jError.error);
	if (clientBin.isEmpty())
	{
		delete binaries;
		return jError;
	}

	executablePath = clientBin;

	QString error;
	QString clientWorkingDirPath = binaries->clientWorkingDirectory(error);
	applicationDir = clientWorkingDirPath;

	delete binaries;

	if (clientWorkingDirPath.isEmpty())
	{
		jError.error = tr("Path to working directory is empty.\nMake sure the configuration for the main binary is set properly.");
		return jError;
	}
	else if (!applicationDir.exists())
	{
		jError.error = tr("%1\n cannot be used as working directory for:\n%2").arg(clientWorkingDirPath, clientBin);
		return jError;
	}

	PathFinder pf(Main::config);
	QStringList missingPwads;
	bool iwadFound = false;

	connectParameters(cli.args, pf, iwadFound, connectPassword);

	for (int i = 0; i < server->numWads(); ++i)
	{
		QString pwad = pf.findFile(server->wad(i));
		if (pwad.isEmpty())
		{
			missingPwads << server->wad(i);
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
			jError.missingIwad = server->iwadName();
		}
		jError.missingWads = missingPwads;
		jError.type = JoinError::MissingWads;
		return jError;
	}

	jError.type = JoinError::NoError;
	return jError;
}

MessageResult GameRunner::host(const HostInfo& hostInfo, bool bOfflinePlay)
{
	CommandLineInfo cmdLine;

	MessageResult result = createHostCommandLine(hostInfo, cmdLine, bOfflinePlay);
	if (result.isError)
	{
		return result;
	}

#ifdef Q_OS_WIN32
	const bool WRAP_IN_SSS_CONSOLE = false;
#else
	const bool WRAP_IN_SSS_CONSOLE = !bOfflinePlay;
#endif

	return runExecutable(cmdLine, WRAP_IN_SSS_CONSOLE);
}

MessageResult GameRunner::hostAppendIwad()
{
	const QString RESULT_CAPTION = tr("Doomseeker - host - appending IWAD");
	const QString& iwadPath = currentHostInfo->iwadPath;
	if (iwadPath.isEmpty())
	{
		return MessageResult(true, RESULT_CAPTION, tr("Iwad is not set"));
	}

	QFileInfo fi(iwadPath);

	if (!fi.isFile())
	{
		QString error = tr("Iwad Path error:\n\"%1\" doesn't exist or is a directory!").arg(iwadPath);
		return MessageResult(true, RESULT_CAPTION, error);
	}

	currentCmdLine->args << argForIwadLoading() << iwadPath;
	return MessageResult(false);
}

MessageResult GameRunner::hostAppendPwads()
{
	const QString RESULT_CAPTION = tr("Doomseeker - host - appending PWADs");
	const QStringList& pwadsPaths = currentHostInfo->pwadsPaths;
	if (!pwadsPaths.isEmpty())
	{
		QStringList& args = currentCmdLine->args;
		foreach(const QString pwad, pwadsPaths)
		{
			args << argForPwadLoading();

			QFileInfo fi(pwad);
			if (!fi.isFile())
			{
				QString error = tr("Pwad path error:\n\"%1\" doesn't exist or is a directory!").arg(pwad);
				return MessageResult(true, RESULT_CAPTION, error);
			}
			args << pwad;
		}
	}

	return MessageResult(false);
}

MessageResult GameRunner::hostGetBinary(bool bOfflinePlay)
{
	const QString RESULT_CAPTION = tr("Doomseeker - host - getting executable");
	QString executablePath = currentHostInfo->executablePath;

	if (executablePath.isEmpty())
	{
		Binaries* binaries = server->binaries();

		QString error;

		// Select binary depending on bOfflinePlay flag:
		if (bOfflinePlay)
		{
			executablePath = binaries->offlineGameBinary(error);
		}
		else
		{
			executablePath = binaries->serverBinary(error);
		}

		delete binaries;

		if (executablePath.isEmpty())
		{
			return MessageResult(true, RESULT_CAPTION, error);
		}
	}

	QFileInfo fi(executablePath);

	if (!fi.isFile())
	{
		QString error = tr("%1\n doesn't exist or is not a file.").arg(fi.filePath());
		return MessageResult(true, RESULT_CAPTION, error);
	}

	currentCmdLine->executable = executablePath;
	return MessageResult(false);
}

MessageResult GameRunner::hostGetWorkingDirectory(bool bOfflinePlay)
{
	const QString RESULT_CAPTION = tr("Doomseeker - host - getting working directory");
	QString error;
	QString serverWorkingDirPath;

	Binaries* binaries = server->binaries();
	// Select working dir based on bOfflinePlay flag:
	if (bOfflinePlay)
	{
		serverWorkingDirPath = binaries->offlineGameWorkingDirectory(error);
	}
	else
	{
		serverWorkingDirPath = binaries->serverWorkingDirectory(error);
	}

	delete binaries;
	QDir applicationDir = serverWorkingDirPath;

	if (serverWorkingDirPath.isEmpty())
	{
		QString error = tr("Path to working directory is empty.\nMake sure the configuration for the main binary is set properly.");
		return MessageResult(true, RESULT_CAPTION, error);
	}
	else if (!applicationDir.exists())
	{
		QString error = tr("%1\n doesn't exist or is not a directory.").arg(serverWorkingDirPath);
		return MessageResult(true, RESULT_CAPTION, error);
	}

	currentCmdLine->applicationDir = applicationDir;
	return MessageResult(false);
}

MessageResult GameRunner::runExecutable(const CommandLineInfo& cli, bool bWrapInStandardServerConsole)
{
	if (!bWrapInStandardServerConsole)
	{
		return AppRunner::runExecutable(cli);
	}
	else
	{
		gLog << tr("Starting (working dir %1): %2 %3").arg(cli.applicationDir.canonicalPath()).arg(cli.executable.canonicalFilePath()).arg(cli.args.join(" "));
		QStringList args = cli.args;
		AppRunner::cleanArguments(args);
		new StandardServerConsole(server, cli.executable.canonicalFilePath(), args);
	}

	return MessageResult(false, tr("runExecutable"));
}

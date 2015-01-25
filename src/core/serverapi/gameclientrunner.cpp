//------------------------------------------------------------------------------
// gameclientrunner.cpp
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
#include "gameclientrunner.h"

#include "configuration/doomseekerconfig.h"
#include "ini/inisection.h"
#include "ini/inivariable.h"
#include "pathfinder/pathfinder.h"
#include "pathfinder/wadpathfinder.h"
#include "plugins/engineplugin.h"
#include "serverapi/exefile.h"
#include "serverapi/gameexeretriever.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "apprunner.h"
#include "commandlinetokenizer.h"
#include "log.h"
#include <QDir>
#include <QScopedPointer>
#include <QStringList>

class ServerConnectParams::PrivData
{
	public:
		QString connectPassword;
		QString demoName;
		QString inGamePassword;
};

ServerConnectParams::ServerConnectParams()
{
	d = new PrivData();
}

ServerConnectParams::ServerConnectParams(const ServerConnectParams& other)
{
	d = new PrivData();
	*d = *other.d;
}

ServerConnectParams& ServerConnectParams::operator=(const ServerConnectParams& other)
{
	if (this != &other)
	{
		*d = *other.d;
	}
	return *this;
}

ServerConnectParams::~ServerConnectParams()
{
	delete d;
}

const QString& ServerConnectParams::connectPassword() const
{
	return d->connectPassword;
}

const QString& ServerConnectParams::demoName() const
{
	return d->demoName;
}

const QString& ServerConnectParams::inGamePassword() const
{
	return d->inGamePassword;
}

void ServerConnectParams::setConnectPassword(const QString& val)
{
	d->connectPassword = val;
}

void ServerConnectParams::setDemoName(const QString& val)
{
	d->demoName = val;
}

void ServerConnectParams::setInGamePassword(const QString& val)
{
	d->inGamePassword = val;
}
///////////////////////////////////////////////////////////////////////////////
#define BAIL_ON_ERROR(method) \
{ \
	method; \
	if (isFatalError()) \
	{ \
		return; \
	} \
}


class GameClientRunner::PrivData
{
	public:
		QString argConnect;
		QString argConnectPassword;
		QString argDehLoading;
		QString argInGamePassword;
		QString argIwadLoading;
		QString argPort;
		QString argPwadLoading;
		QString argDemoRecord;

		QStringList args;
		mutable QString cachedIwadPath;
		ServerConnectParams connectParams;
		CommandLineInfo* cli;
		JoinError joinError;
		QStringList missingPwads;
		PathFinder pathFinder;
		ServerPtr server;

		void (GameClientRunner::*addExtra)();
		void (GameClientRunner::*addIwad)();
		void (GameClientRunner::*createCommandLineArguments)();
};

POLYMORPHIC_DEFINE(void, GameClientRunner, addExtra, (), ());
POLYMORPHIC_DEFINE(void, GameClientRunner, addIwad, (), ());
POLYMORPHIC_DEFINE(void, GameClientRunner, createCommandLineArguments, (), ());

GameClientRunner::GameClientRunner(ServerPtr server)
{
	d = new PrivData();
	set_addExtra(&GameClientRunner::addExtra_default);
	set_addIwad(&GameClientRunner::addIwad_default);
	set_createCommandLineArguments(&GameClientRunner::createCommandLineArguments_default);
	d->argConnect = "-connect";
	d->argIwadLoading = "-iwad";
	d->argPort = "-port";
	d->argPwadLoading = "-file";
	d->argDemoRecord = "-record";
	d->cli = NULL;
	d->server = server;
}

GameClientRunner::~GameClientRunner()
{
	delete d;
}

void GameClientRunner::addConnectCommand()
{
	QString address = QString("%1:%2").arg(d->server->address().toString()).arg(d->server->port());
	args() << argForConnect() << address;
}

void GameClientRunner::addCustomParameters()
{
	IniSection config = gConfig.iniSectionForPlugin(d->server->plugin());
	QString customParameters = config["CustomParameters"];
	CommandLineTokenizer tokenizer;
	args() << tokenizer.tokenize(customParameters);
}

void GameClientRunner::addDemoRecordCommand()
{
	args() << argForDemoRecord() << demoName();
}

void GameClientRunner::addGamePaths()
{
	GamePaths paths = gamePaths();
	if (!paths.isValid())
	{
		// gamePaths() sets JoinError.
		return;
	}

	QDir applicationDir = paths.workingDir;
	if (paths.workingDir.isEmpty())
	{
		d->joinError.setType(JoinError::ConfigurationError);
		d->joinError.setError(tr("Path to working directory for game \"%1\" is empty.\n\n"
			"Make sure the configuration for the client executable is set properly.")
			.arg(pluginName()));
		return;
	}
	else if (!applicationDir.exists())
	{
		d->joinError.setType(JoinError::ConfigurationError);
		d->joinError.setError(tr("%1\n\nThis directory cannot be used as working "
			"directory for game: %2\n\nExecutable: %3")
			.arg(paths.workingDir, pluginName(), paths.clientExe));
		return;
	}

	d->cli->executable = paths.clientExe;
	d->cli->applicationDir = applicationDir;
}

void GameClientRunner::addInGamePassword()
{
	if (!argForInGamePassword().isNull())
	{
		args() << argForInGamePassword() << inGamePassword();
	}
	else
	{
		gLog << tr("BUG: Plugin doesn't specify argument for in-game "
			"password, but the server requires such password.");
	}
}

void GameClientRunner::addExtra_default()
{
}

void GameClientRunner::addIwad_default()
{
	args() << argForIwadLoading() << iwadPath();
}

void GameClientRunner::addWads()
{
	addIwad();
	addPwads();

	if (!isIwadFound() || !d->missingPwads.isEmpty())
	{
		if (!isIwadFound())
		{
			d->joinError.setMissingIwad(d->server->iwad());
		}
		d->joinError.setMissingWads(d->missingPwads);
		d->joinError.setType(JoinError::MissingWads);
	}
}

void GameClientRunner::addPassword()
{
	if (!argForConnectPassword().isNull())
	{
		args() << argForConnectPassword() << connectPassword();
	}
	else
	{
		gLog << tr("BUG: Plugin doesn't specify argument for connect "
			"password, but the server is passworded.");
	}
}

void GameClientRunner::addPwads()
{
	for (int i = 0; i < d->server->numWads(); ++i)
	{
		QString pwad = findWad(d->server->wad(i).name());
		if (pwad.isEmpty() && !d->server->wad(i).isOptional())
		{
			markPwadAsMissing(d->server->wad(i).name());
		}
		else if (!pwad.isEmpty()) // Don't -file missing optional wads
		{
			if (pwad.toLower().endsWith(".deh"))
			{
				args() << argForDehLoading() << pwad;
			}
			else
			{
				args() << argForPwadLoading() << pwad;
			}
		}
	}
}

QStringList& GameClientRunner::args()
{
	return d->cli->args;
}

const QString& GameClientRunner::argForConnect() const
{
	return d->argConnect;
}

const QString& GameClientRunner::argForConnectPassword() const
{
	return d->argConnectPassword;
}

const QString& GameClientRunner::argForDehLoading() const
{
	return d->argDehLoading;
}

const QString& GameClientRunner::argForInGamePassword() const
{
	return d->argInGamePassword;
}

const QString& GameClientRunner::argForIwadLoading() const
{
	return d->argIwadLoading;
}

const QString& GameClientRunner::argForPort() const
{
	return d->argPort;
}

const QString& GameClientRunner::argForPwadLoading() const
{
	return d->argPwadLoading;
}

const QString& GameClientRunner::argForDemoRecord() const
{
	return d->argDemoRecord;
}

bool GameClientRunner::canDownloadWadsInGame() const
{
	return d->server->plugin()->data()->inGameFileDownloads;
}

const QString& GameClientRunner::connectPassword() const
{
	return d->connectParams.connectPassword();
}

void GameClientRunner::createCommandLineArguments_default()
{
	BAIL_ON_ERROR(addGamePaths());
	BAIL_ON_ERROR(addConnectCommand());
	if (d->server->isLocked())
	{
		BAIL_ON_ERROR(addPassword());
	}
	if (d->server->isLockedInGame())
	{
		BAIL_ON_ERROR(addInGamePassword());
	}
	if (!demoName().isEmpty())
	{
		BAIL_ON_ERROR(addDemoRecordCommand());
	}
	BAIL_ON_ERROR(addWads());
	BAIL_ON_ERROR(addExtra());
	BAIL_ON_ERROR(addCustomParameters());
}

JoinError GameClientRunner::createJoinCommandLine(CommandLineInfo& cli,
	const ServerConnectParams& params)
{
	d->cli = &cli;
	d->cli->args.clear();
	d->connectParams = params;
	d->joinError = JoinError();

	setupPathFinder();
	if (d->joinError.isError())
	{
		return d->joinError;
	}
	createCommandLineArguments();

	return d->joinError;
}

const QString& GameClientRunner::demoName() const
{
	return d->connectParams.demoName();
}

QString GameClientRunner::findIwad() const
{
	return findWad(d->server->iwad().toLower());
}

QString GameClientRunner::findWad(const QString &wad) const
{
	return WadPathFinder(d->pathFinder).find(wad).path();
}

GameClientRunner::GamePaths GameClientRunner::gamePaths()
{
	Message msg;
	GamePaths result;

	QScopedPointer<ExeFile> exeFile(d->server->clientExe());
	result.clientExe = exeFile->pathToExe(msg);
	if (result.clientExe.isEmpty())
	{
		if (msg.type() == Message::Type::GAME_NOT_FOUND_BUT_CAN_BE_INSTALLED)
		{
			d->joinError.setType(JoinError::CanAutomaticallyInstallGame);
			if (msg.contents().isEmpty())
			{
				d->joinError.setError(msg.contents());
			}
			else
			{
				d->joinError.setError(tr("Game can be installed by Doomseeker"));
			}
		}
		else
		{
			d->joinError.setType(JoinError::ConfigurationError);
			QString error = tr("Client binary cannot be obtained for %1, please "
				"check the location given in the configuration.").arg(pluginName());
			if (!msg.isIgnore())
			{
				error += "\n\n" + msg.contents();
			}
			d->joinError.setError(error);
		}
		return GamePaths();
	}
	result.workingDir = exeFile->workingDirectory(msg);

	GameExeRetriever exeRetriever = GameExeRetriever(*d->server->plugin()->gameExe());
	result.offlineExe = pathToOfflineExe(msg);

	return result;
}

const QString& GameClientRunner::inGamePassword() const
{
	return d->connectParams.inGamePassword();
}

bool GameClientRunner::isFatalError() const
{
	if (d->joinError.isError())
	{
		if (d->joinError.isMissingWadsError() && canDownloadWadsInGame())
		{
			return false;
		}
		return true;
	}
	return false;
}

bool GameClientRunner::isIwadFound() const
{
	return !d->cachedIwadPath.isEmpty();
}

const QString& GameClientRunner::iwadPath() const
{
	if (!isIwadFound())
	{
		d->cachedIwadPath = findIwad();
	}
	return d->cachedIwadPath;
}

void GameClientRunner::markPwadAsMissing(const QString& pwadName)
{
	d->missingPwads << pwadName;
}

PathFinder& GameClientRunner::pathFinder()
{
	return d->pathFinder;
}

QString GameClientRunner::pathToOfflineExe(Message &msg)
{
	GameExeRetriever exeRetriever = GameExeRetriever(*d->server->plugin()->gameExe());
	return exeRetriever.pathToOfflineExe(msg);
}

const QString& GameClientRunner::pluginName() const
{
	return d->server->plugin()->data()->name;
}

ServerConnectParams& GameClientRunner::serverConnectParams()
{
	return d->connectParams;
}

void GameClientRunner::setArgForConnect(const QString& arg)
{
	d->argConnect = arg;
}

void GameClientRunner::setArgForConnectPassword(const QString& arg)
{
	d->argConnectPassword = arg;
}

void GameClientRunner::setArgForDehLoading(const QString& arg)
{
	d->argDehLoading = arg;
}

void GameClientRunner::setArgForInGamePassword(const QString& arg)
{
	d->argInGamePassword = arg;
}

void GameClientRunner::setArgForIwadLoading(const QString& arg)
{
	d->argIwadLoading = arg;
}

void GameClientRunner::setArgForPort(const QString& arg)
{
	d->argPort = arg;
}

void GameClientRunner::setArgForPwadLoading(const QString& arg)
{
	d->argPwadLoading = arg;
}

void GameClientRunner::setArgForDemoRecord(const QString& arg)
{
	d->argDemoRecord = arg;
}

JoinError GameClientRunner::joinError() const
{
	return d->joinError;
}

void GameClientRunner::setJoinError(const JoinError& e)
{
	d->joinError = e;
}

void GameClientRunner::setupPathFinder()
{
	d->pathFinder = d->server->wadPathFinder();
}

QString GameClientRunner::wadTargetDirectory() const
{
	return DoomseekerConfig::config().wadseeker.targetDirectory;
}

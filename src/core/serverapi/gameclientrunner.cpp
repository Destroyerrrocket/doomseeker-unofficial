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
#include "serverapi/gameclientrunner.h"
#include "pathfinder/pathfinder.h"
#include "plugins/engineplugin.h"
#include "serverapi/exefile.h"
#include "serverapi/gameexeretriever.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "configuration/doomseekerconfig.h"
#include "gui/standardserverconsole.h"
#include "apprunner.h"
#include "log.h"
#include "main.h"
#include "strings.h"
#include <QDateTime>
#include <QScopedPointer>
#include <QStringList>

class ServerConnectParams::PrivData
{
	public:
		QString connectPassword;
		QString demoName;
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

void ServerConnectParams::setConnectPassword(const QString& val)
{
	d->connectPassword = val;
}

void ServerConnectParams::setDemoName(const QString& val)
{
	d->demoName = val;
}
///////////////////////////////////////////////////////////////////////////////
#define BAIL_ON_ERROR(method) \
{ \
	method; \
	if (d->joinError.isError()) \
	{ \
		return; \
	} \
}


class GameClientRunner::PrivData
{
	public:
		QString argConnect;
		QString argConnectPassword;
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
		Server* server;

		void (GameClientRunner::*addExtra)();
		void (GameClientRunner::*addIwad)();
		void (GameClientRunner::*createCommandLineArguments) ();
		void (GameClientRunner::*setupPathFinder) ();
};

POLYMORPHIC_DEFINE(void, GameClientRunner, addExtra, (), ());
POLYMORPHIC_DEFINE(void, GameClientRunner, addIwad, (), ());
POLYMORPHIC_DEFINE(void, GameClientRunner, createCommandLineArguments, (), ());
POLYMORPHIC_DEFINE(void, GameClientRunner, setupPathFinder, (), ());

GameClientRunner::GameClientRunner(Server* server)
{
	d = new PrivData();
	set_addExtra(&GameClientRunner::addExtra_default);
	set_addIwad(&GameClientRunner::addIwad_default);
	set_createCommandLineArguments(&GameClientRunner::createCommandLineArguments_default);
	set_setupPathFinder(&GameClientRunner::setupPathFinder_default);
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
	args() << customParameters.split(" ", QString::SkipEmptyParts);
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
		d->joinError.type = JoinError::ConfigurationError;
		d->joinError.error = tr("Path to working directory for game \"%1\" is empty.\n\n"
			"Make sure the configuration for the client executable is set properly.")
			.arg(pluginName());
		return;
	}
	else if (!applicationDir.exists())
	{
		d->joinError.type = JoinError::ConfigurationError;
		d->joinError.error = tr("%1\n\nThis directory cannot be used as working "
			"directory for game: %2\n\nExecutable: %3")
			.arg(paths.workingDir, pluginName(), paths.clientExe);
		return;
	}

	d->cli->executable = paths.clientExe;
	d->cli->applicationDir = applicationDir;
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
			d->joinError.missingIwad = d->server->iwad();
		}
		d->joinError.missingWads = d->missingPwads;
		d->joinError.type = JoinError::MissingWads;
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
		QString pwad = d->pathFinder.findFile(d->server->wad(i).name);
		if (pwad.isEmpty() && !d->server->wad(i).optional)
		{
			markPwadAsMissing(d->server->wad(i).name);
		}
		else
		{
			args() << argForPwadLoading() << pwad;
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

const QString& GameClientRunner::connectPassword() const
{
	return d->connectParams.connectPassword();
}

void GameClientRunner::createCommandLineArguments_default()
{
	BAIL_ON_ERROR(addGamePaths());
	BAIL_ON_ERROR(addConnectCommand());
	BAIL_ON_ERROR(addCustomParameters());
	if (d->server->isLocked())
	{
		BAIL_ON_ERROR(addPassword());
	}
	if (!demoName().isEmpty())
	{
		BAIL_ON_ERROR(addDemoRecordCommand());
	}
	BAIL_ON_ERROR(addWads());
	BAIL_ON_ERROR(addExtra());
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
	return d->pathFinder.findFile(d->server->iwad().toLower());
}

GameClientRunner::GamePaths GameClientRunner::gamePaths()
{
	Message msg;
	GamePaths result;
	GameExeRetriever exeRetriever = GameExeRetriever(*d->server->plugin()->gameExe());
	QScopedPointer<ExeFile> exeFile(d->server->clientExe());
	result.clientExe = exeFile->pathToExe(msg);
	result.offlineExe = exeRetriever.pathToOfflineExe(msg);
	result.workingDir = exeFile->workingDirectory(msg);

	if (!result.isValid())
	{
		d->joinError.type = JoinError::ConfigurationError;
		d->joinError.error = tr("Client binary cannot be obtained for %1, please "
			"check the location given in the configuration.").arg(pluginName());
		if (!msg.isIgnore())
		{
			d->joinError.error += "\n\n" + msg.contents();
		}
	}

	return result;
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

void GameClientRunner::setJoinError(const JoinError& e)
{
	d->joinError = e;
}

void GameClientRunner::setupPathFinder_default()
{
	GamePaths paths = gamePaths();
	// Add the offline game directory so results are more consistent
	// addPrioritySearchDir prepends to the list so we'll want to add the real
	// priority directory second.
	d->pathFinder.addPrioritySearchDir(paths.offlineExe);
	d->pathFinder.addPrioritySearchDir(paths.clientExe);
}

QString GameClientRunner::wadTargetDirectory() const
{
	return DoomseekerConfig::config().wadseeker.targetDirectory;
}
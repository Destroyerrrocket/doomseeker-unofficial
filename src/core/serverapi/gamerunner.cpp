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
#include "pathfinder.h"
#include "strings.h"
#include <QDateTime>
#include <QScopedPointer>
#include <QStringList>

class ServerConnectParams::PrivData
{
	public:
		QString connectPassword;
		QString iwadPath;
		QString wadTargetDirectory;
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

const QString& ServerConnectParams::iwadPath() const
{
	return d->iwadPath;
}

void ServerConnectParams::setConnectPassword(const QString& val)
{
	d->connectPassword = val;
}

void ServerConnectParams::setIwadPath(const QString& val)
{
	d->iwadPath = val;
}

void ServerConnectParams::setWadTargetDirectory(const QString& val)
{
	d->wadTargetDirectory = val;
}

const QString& ServerConnectParams::wadTargetDirectory() const
{
	return d->wadTargetDirectory;
}
///////////////////////////////////////////////////////////////////////////////
class GameRunner::PrivData
{
	public:
		QString argConnect;
		QString argConnectPassword;
		QString argIwadLoading;
		QString argPort;
		QString argPwadLoading;
		QString argDemoRecord;

		QStringList args;
		CommandLineInfo* cli;
		PathFinder pathFinder;
		Server* server;
};

GameRunner::GameRunner(Server* server)
{
	d = new PrivData();
	d->argConnect = "-connect";
	d->argIwadLoading = "-iwad";
	d->argPort = "-port";
	d->argPwadLoading = "-file";
	d->argDemoRecord = "-record";
	d->cli = NULL;
	d->server = server;
}

GameRunner::~GameRunner()
{
	delete d;
}

void GameRunner::addPwads(CommandLineInfo& cli, QStringList& missingPwads)
{
	for (int i = 0; i < d->server->numWads(); ++i)
	{
		QString pwad = d->pathFinder.findFile(d->server->wad(i).name);
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
}

QStringList& GameRunner::args()
{
	return d->cli->args;
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

const QString& GameRunner::argForDemoRecord() const
{
	return d->argDemoRecord;
}

bool GameRunner::connectParameters(ServerConnectParams& params)
{
	// Connect
	QString address = QString("%1:%2").arg(d->server->address().toString()).arg(d->server->port());
	args() << argForConnect() << address;

	// Iwad
	args() << argForIwadLoading() << params.iwadPath();

	// Custom parameters
	IniSection config = gConfig.iniSectionForPlugin(d->server->plugin());
	QString customParameters = config["CustomParameters"];
	args() << customParameters.split(" ", QString::SkipEmptyParts);

	// Password
	if (d->server->isLocked())
	{
		if (argForConnectPassword().isNull())
		{
			gLog << tr("BUG: Plugin doesn't specify argument for connect "
				"password, but the server is passworded.");
		}
		else
		{
			args() << argForConnectPassword() << params.connectPassword();
		}
	}
	return true;
}

JoinError GameRunner::createJoinCommandLine(CommandLineInfo& cli,
	const QString &connectPassword, bool managedDemo)
{
	const QString &PLUGIN_NAME = d->server->plugin()->data()->name;
	d->cli = &cli;

	setupPathFinder();

	JoinError joinError;
	// Init the JoinError type with critical error. We will change this upon
	// successful return or if wads are missing.
	joinError.type = JoinError::Critical;

	cli.args.clear();

	Message message;
	GamePaths paths = gamePaths(message);
	if (!paths.isValid())
	{
		joinError.type = JoinError::ConfigurationError;
		joinError.error = tr("Client binary cannot be obtained for %1, please "
			"check the location given in the configuration.").arg(PLUGIN_NAME);
		if (!message.isIgnore())
		{
			joinError.error += "\n\n" + message.contents();
		}
		return joinError;
	}

	cli.executable = paths.clientExe;
	cli.applicationDir = paths.workingDir;

	if (paths.workingDir.isEmpty())
	{
		joinError.type = JoinError::ConfigurationError;
		joinError.error = tr("Path to working directory for \"%1\" is empty.\n"
			"Make sure the configuration for the main binary is set properly.")
			.arg(PLUGIN_NAME);
		return joinError;
	}
	else if (!cli.applicationDir.exists())
	{
		joinError.type = JoinError::ConfigurationError;
		joinError.error = tr("%1\n cannot be used as working "
			"directory for game:\n%2\nExecutable: %3")
			.arg(paths.workingDir, PLUGIN_NAME, paths.clientExe);
		return joinError;
	}

	QStringList missingPwads;
	QString iwad = findIwad();

	ServerConnectParams params;
	params.setIwadPath(iwad);
	params.setConnectPassword(connectPassword);
	params.setWadTargetDirectory(DoomseekerConfig::config().wadseeker.targetDirectory);
	if(!connectParameters(params))
	{
		joinError.type = JoinError::Terminate;
		return joinError;
	}

	// Record
	QString demoName;
	if(gConfig.doomseeker.bRecordDemo)
	{
		cli.args << argForDemoRecord();
		cli.args << mkDemoName(managedDemo);
	}

	addPwads(cli, missingPwads);

	if (iwad.isEmpty() || !missingPwads.isEmpty())
	{
		if (iwad.isEmpty())
		{
			joinError.missingIwad = d->server->iwad();
		}
		joinError.missingWads = missingPwads;
		joinError.type = JoinError::MissingWads;
		return joinError;
	}

	// No errors?
	// Nothing should be stopping us from joining so dump the meta information
	// if we are doing a managed demo
	if(!demoName.isEmpty() && managedDemo)
	{
		saveDemoMetaData(demoName);
	}

	joinError.type = JoinError::NoError;
	return joinError;
}

QString GameRunner::findIwad()
{
	return d->pathFinder.findFile(d->server->iwad().toLower());
}

GameRunner::GamePaths GameRunner::gamePaths(Message& msg)
{
	GamePaths result;
	GameExeRetriever exeRetriever = GameExeRetriever(*d->server->plugin()->gameExe());
	QScopedPointer<ExeFile> exeFile(d->server->clientExe());
	result.clientExe = exeFile->pathToExe(msg);
	result.offlineExe = exeRetriever.pathToOfflineExe(msg);
	result.workingDir = exeFile->workingDirectory(msg);
	return result;
}

QString GameRunner::mkDemoName(bool managedDemo) const
{
	// port-iwad-date-wad
	QString demoName;
	if(managedDemo)
	{
		demoName = Main::dataPaths->demosDirectoryPath() + QDir::separator();
	}
	demoName += QString("%1_%2").
		arg(d->server->engineName()).
		arg(QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ss"));
	if(!d->server->plugin()->data()->demoExtensionAutomatic)
	{
		demoName += QString(".%1").arg(d->server->plugin()->data()->demoExtension);
	}
	return demoName;
}

PathFinder& GameRunner::pathFinder()
{
	return d->pathFinder;
}

Message GameRunner::runExecutable(const CommandLineInfo& cli, bool bWrapInStandardServerConsole)
{
	if (!bWrapInStandardServerConsole)
	{
		return AppRunner::runExecutable(cli);
	}
	else
	{
		gLog << tr("Starting (working dir %1): %2")
			.arg(cli.applicationDir.absolutePath())
			.arg(cli.executable.absoluteFilePath());
		QStringList args = cli.args;
		// Is this needed for something? Zandronum needs the quotes for console
		// variables.
		//AppRunner::cleanArguments(args);
		new StandardServerConsole(d->server, cli.executable.absoluteFilePath(), args);
	}

	return Message();
}

void GameRunner::saveDemoMetaData(const QString& demoName)
{
	QString metaFileName;
	// If the extension is automatic we need to add it here
	if(d->server->plugin()->data()->demoExtensionAutomatic)
	{
		metaFileName = QString("%1.%2.ini").arg(demoName)
			.arg(d->server->plugin()->data()->demoExtension);
	}
	else
	{
		metaFileName = demoName + ".ini";
	}

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

void GameRunner::setArgForDemoRecord(const QString& arg)
{
	d->argDemoRecord = arg;
}

void GameRunner::setupPathFinder()
{
	Message msg;
	GamePaths paths = gamePaths(msg);
	// Add the offline game directory so results are more consistent
	// addPrioritySearchDir prepends to the list so we'll want to add the real
	// priority directory second.
	d->pathFinder.addPrioritySearchDir(paths.offlineExe);
	d->pathFinder.addPrioritySearchDir(paths.clientExe);
}
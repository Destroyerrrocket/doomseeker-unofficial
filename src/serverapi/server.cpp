//------------------------------------------------------------------------------
// server.cpp
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

#include "log.h"
#include "serverapi/server.h"
#include "main.h"
#include "strings.h"
#include "gui/standardserverconsole.h"
#include "gui/wadseekerinterface.h"
#include "serverapi/playerslist.h"
#include "serverapi/tooltipgenerator.h"
#include <QProcess>
#include <QSet>
#include <QTime>
#include <QUdpSocket>

////////////////////////////////////////////////////////////////////////////////

// I really don't know if the game modes will need to be translated but I
// wrapped them in tr() just to be safe.
const GameMode GameMode::COOPERATIVE(SGMICooperative, QObject::tr("Cooperative"), false);
const GameMode GameMode::DEATHMATCH(SGMIDeathmatch, QObject::tr("Deathmatch"), false);
const GameMode GameMode::TEAM_DEATHMATCH(SGMITeamDeathmatch, QObject::tr("Team DM"), true);
const GameMode GameMode::CAPTURE_THE_FLAG(SGMICTF, QObject::tr("CTF"), true);
const GameMode GameMode::UNKNOWN(SGMIUnknown, QObject::tr("Unknown"), false);

GameMode::GameMode(int index, const QString &name, bool teamgame) : gameModeIndex(index), modeName(name), teamgame(teamgame)
{
}

////////////////////////////////////////////////////////////////////////////////
const int SkillLevel::numSkillLevels = 5;

const QString SkillLevel::names[] =
{
	QObject::tr("1 - I'm too young to die."),
	QObject::tr("2 - Hey, not too rough."),
	QObject::tr("3 - Hurt me plenty."),
	QObject::tr("4 - Ultra-Violence."),
	QObject::tr("5 - NIGHTMARE!")
};

////////////////////////////////////////////////////////////////////////////////

QString Server::teamNames[] =
{
	"Blue",
	"Red",
	"Green",
	"Gold"
};

Server::Server(const QHostAddress &address, unsigned short port) : QObject(),
	currentGameMode(GameMode::COOPERATIVE), currentPing(999), locked(false),
	maxClients(0), maxPlayers(0), serverName(tr("<< ERROR >>")),
	serverScoreLimit(0), serverTimeLeft(0), serverTimeLimit(0),
	// private
	bIsRefreshing(false), serverAddress(address), serverPort(port)
{
	broadcastToLAN = false;
	broadcastToMaster = false;
	mapRandomRotation = false;
	bDelete = false;
	bKnown = false;
	custom = false;
	skill = 3;
	for(int i = 0;i < MAX_TEAMS;i++)
	{
		scores[i] = 0;
	}

	players = new PlayersList();

	connect(this, SIGNAL( updated(Server*, int) ), this, SLOT( setResponse(Server*, int) ));
}

Server::Server(const Server &other) : QObject(), currentGameMode(GameMode::COOPERATIVE)
{
	(*this) = other;
	connect(this, SIGNAL( updated(Server*, int) ), this, SLOT( setResponse(Server*, int) ));
}

Server::~Server()
{
	delete players;
	clearDMFlags();
}

void Server::cleanArguments(QStringList& args) const
{
	#ifdef Q_OS_WIN32
	QStringList::iterator it;
	for (it = args.begin(); it != args.end(); ++it)
	{
		if (it->contains(" "))
		{
			Strings::trim(*it, "\"");
		}
	}
	#endif
}

void Server::clearDMFlags()
{
	DMFlagsIt it;
	for (it = dmFlags.begin(); it != dmFlags.end(); ++it)
	{
		delete (*it);
	}
	dmFlags.clear();
}

QString Server::clientWorkingDirectory() const
{
	QString dummy;
	QFileInfo fi(clientBinary(dummy));
	return fi.absolutePath();
}

void Server::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword) const
{
	// Connect
	args << "-connect" << QString(address().toString() + ":" + QString::number(port()));
	if(isLocked())
		args << connectPassword;

	// Iwad
	QString iwad = pf.findWad(iwadName().toLower());
	args << argForIwadLoading() << iwad;
	iwadFound = !iwad.isEmpty();
}

bool Server::createHostCommandLine(const HostInfo& hostInfo, CommandLineInfo& cli, bool bOfflinePlay, QString& error) const
{
	QDir& applicationDir = cli.applicationDir;
	QFileInfo& executablePath = cli.executable;
	QStringList& args = cli.args;

	const QString errorCaption = tr("Doomseeker - error");
	args.clear();

	// First some wad path checks, add wad paths to the args if check passes:
	const QString& iwadPath = hostInfo.iwadPath;
	if (iwadPath.isEmpty())
	{
		error = tr("Iwad is not set");
		return false;
	}

	QFileInfo fi(iwadPath);

	if (!fi.isFile())
	{
		error = tr("Iwad Path error:\n\"%1\" doesn't exist or is a directory!").arg(iwadPath);
		return false;
	}

	args << argForIwadLoading() << iwadPath;

	const QStringList& pwadsPaths = hostInfo.pwadsPaths;
	if (!pwadsPaths.isEmpty())
	{
		foreach(const QString s, pwadsPaths)
		{
			args << argForPwadLoading();

			fi.setFile(s);
			if (!fi.isFile())
			{
				error = tr("Pwad path error:\n\"%1\" doesn't exist or is a directory!").arg(s);
				return false;
			}
			args << s;
		}
	}
	// Checks done.

	// Port
	args << argForPort() << QString::number(serverPort);

	// CVars
	const QList<GameCVar>& cvars = hostInfo.cvars;
	foreach(const GameCVar c, cvars)
	{
		args << QString("+" + c.consoleCommand) << c.value();
	}

	const QString& serverExecutablePath = hostInfo.executablePath;
	if (serverExecutablePath.isEmpty())
	{
		// Select binary depending on bOfflinePlay flag:
		QString serverBin;
		if (bOfflinePlay)
		{
			serverBin = offlineGameBinary(error);
		}
		else
		{
			serverBin = serverBinary(error);
		}

		if (serverBin.isEmpty())
		{
			return false;
		}
		executablePath = serverBin;
	}
	else
	{
		executablePath = serverExecutablePath;
	}

	if (!executablePath.isFile())
	{
		error = tr("%1\n doesn't exist or is not a file.").arg(executablePath.filePath());
		return false;
	}

	QString serverWorkingDirPath;
	// Select working dir based on bOfflinePlay flag:
	if (bOfflinePlay)
	{
		serverWorkingDirPath = offlineGameWorkingDirectory();
	}
	else
	{
		serverWorkingDirPath = serverWorkingDirectory();
	}

	applicationDir = serverWorkingDirectory();

	if (serverWorkingDirPath.isEmpty())
	{
		error = tr("Path to working directory is empty.\nMake sure the configuration for the main binary is set properly.");
		return false;
	}
	else if (!applicationDir.exists())
	{
		error = tr("%1\n cannot be used as working directory for:\n%2").arg(serverWorkingDirPath, executablePath.filePath());
		return false;
	}

	// Add the server launch parameter only if we don't want offline game
	if (!bOfflinePlay)
	{
		args << argForServerLaunch();
	}

	hostDMFlags(args, dmFlags);
	hostProperties(args);
	args.append(hostInfo.customParameters);

	return true;
}

JoinError Server::createJoinCommandLine(CommandLineInfo& cli, const QString &connectPassword) const
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

	QString clientBin = clientBinary(jError.error);
	if (clientBin.isEmpty())
	{
		return jError;
	}

	executablePath = clientBin;

	QString clientWorkingDirPath = clientWorkingDirectory();
	applicationDir = clientWorkingDirPath;

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

	for (int i = 0; i < numWads(); ++i)
	{
		QString pwad = pf.findWad(wad(i));
		if (pwad.isEmpty())
		{
			missingPwads << wad(i);
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
			jError.missingIwad = iwad;
		}
		jError.missingWads = missingPwads;
		jError.type = JoinError::MissingWads;
		return jError;
	}

	jError.type = JoinError::NoError;
	return jError;
}

bool Server::host(const HostInfo& hostInfo, bool bOfflinePlay, QString& error)
{
	error.clear();
	CommandLineInfo cli;

	if (!createHostCommandLine(hostInfo, cli, bOfflinePlay, error))
		return false;

#ifdef Q_OS_WIN32
	const bool WRAP_IN_SSS_CONSOLE = false;
#else
	const bool WRAP_IN_SSS_CONSOLE = !bOfflinePlay;
#endif

	return runExecutable(cli, WRAP_IN_SSS_CONSOLE, error);
}

bool Server::isWebsiteURLSafe() const
{
	return !website().startsWith("file://", Qt::CaseInsensitive);
}

JoinError Server::join(const QString &connectPassword) const
{
	CommandLineInfo cli;

	JoinError jError = createJoinCommandLine(cli, connectPassword);
	if (jError.type != JoinError::NoError)
	{
		return jError;
	}

	if (!runExecutable(cli, false, jError.error))
	{
		jError.type = JoinError::Critical;
	}

	return jError;
}

int Server::numFreeClientSlots() const
{
	int returnValue = maxClients - players->numClients();
	return (returnValue < 0) ? 0 : returnValue;
}

int Server::numFreeJoinSlots() const
{
	int returnValue = maxPlayers - players->numClients();
	return (returnValue < 0) ? 0 : returnValue;
}

int Server::numFreeSpectatorSlots() const
{
	int returnValue = numFreeClientSlots() - numFreeJoinSlots();
	return (returnValue < 0) ? 0 : returnValue;
}

QString Server::offlineGameWorkingDirectory() const
{
	QString dummy;
	QFileInfo fi(offlineGameBinary(dummy));
	return fi.absolutePath();
}

const Player& Server::player(int index) const
{
	return (*players)[index];
}

bool Server::refresh()
{
	if (Main::refreshingThread == NULL)
	{
		emitUpdated(RESPONSE_BAD);
		pLog << tr("CRITIAL ERROR: REFRESHING THREAD IS NULL");
		return false;
	}

	Main::refreshingThread->registerServer(this);
	return true;
}

void Server::refreshStarts()
{
	bIsRefreshing = true;

	emit begunRefreshing(this);
	triesLeft = Main::config->setting("QueryTries")->integer();
	if(triesLeft > 10) // Limit the maximum number of tries
		triesLeft = 10;
}

void Server::refreshStops()
{
	bIsRefreshing = false;
	iwad = iwad.toLower();
}

bool Server::runExecutable(const CommandLineInfo& cli, bool bWrapInStandardServerConsole, QString& error) const
{
	pLog << tr("Starting (working dir %1): %2 %3").arg(cli.applicationDir.canonicalPath()).arg(cli.executable.canonicalFilePath()).arg(cli.args.join(" "));
	QStringList args = cli.args;
	cleanArguments(args);

	if (!bWrapInStandardServerConsole)
	{
		int result;

		#ifdef Q_WS_MAC
		if( cli.executable.isBundle() )
		{
			result = QProcess::startDetached("open", QStringList() << cli.executable.canonicalFilePath() << "--args" << args, cli.applicationDir.canonicalPath());
		}
		else
		#endif
		{
			result = QProcess::startDetached(cli.executable.canonicalFilePath(), args, cli.applicationDir.canonicalPath());
		}
		if(!result)
		{
			error = tr("File: %1\ncannot be run").arg(cli.executable.canonicalFilePath());
			pLog << error;
			return false;
		}
	}
	else
	{
		new StandardServerConsole(this, cli.executable.canonicalFilePath(), args);
	}

	return true;
}

bool Server::sendRefreshQuery(QUdpSocket* socket)
{
	if(triesLeft-- == 0)
	{
		emitUpdated(Server::RESPONSE_TIMEOUT);
		refreshStops();
		return false;
	}

	QByteArray request;
	if (!sendRequest(request))
	{
		emitUpdated(Server::RESPONSE_BAD);
		refreshStops();
		return false;
	}

	time.start();

	socket->writeDatagram(request, address(), port());

	return true;
}

void Server::setResponse(Server* server, int res)
{
	response = static_cast<Response>(res);
	if (response == RESPONSE_GOOD)
	{
		bKnown = true;
	}
	else if (response == RESPONSE_BAD || response == RESPONSE_BANNED || response == RESPONSE_TIMEOUT)
	{
		bKnown = false;
	}

}

void Server::setToDelete(bool b)
{
	bDelete = b;
	if (!bIsRefreshing)
		delete this;
}

QString Server::serverWorkingDirectory() const
{
	QString dummy;
	QFileInfo fi(serverBinary(dummy));
	return fi.absolutePath();
}

QRgb Server::teamColor(int team) const
{
	switch(team)
	{
		case Player::TEAM_BLUE: return qRgb(0, 0, 255);
		case Player::TEAM_RED: return qRgb(255, 0, 0);
		case Player::TEAM_GREEN: return qRgb(0, 255, 0);
		case Player::TEAM_GOLD: return qRgb(255, 255, 0);
		default: break;
	}
	return qRgb(0, 255, 0);
}

TooltipGenerator* Server::tooltipGenerator() const
{
	return new TooltipGenerator(this);
}

void Server::operator= (const Server &other)
{
	serverAddress = other.address();
	serverPort = other.port();

	bDelete = other.bDelete;
	bKnown = other.isKnown();
	currentGameMode = other.gameMode();
	currentPing = other.ping();
	custom = other.isCustom();
	locked = other.isLocked();
	maxClients = other.maximumClients();
	maxPlayers = other.maximumPlayers();
	serverName = other.name();
	serverScoreLimit = other.scoreLimit();
}

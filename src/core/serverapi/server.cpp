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
#include "configuration/doomseekerconfig.h"
#include "plugins/engineplugin.h"
#include "serverapi/server.h"
#include "main.h"
#include "strings.h"
#include "gui/standardserverconsole.h"
#include "gui/wadseekerinterface.h"
#include "refresher/refresher.h"
#include "serverapi/binaries.h"
#include "serverapi/gamerunner.h"
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
	if(gConfig.doomseeker.bLookupHosts)
		lookupHost();

	bSecureServer = false;
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

Binaries *Server::binaries() const
{
	return new Binaries(plugin());
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

QString Server::engineName() const
{
	if (plugin() != NULL)
	{
		return plugin()->data()->name;
	}
	else
	{
		return tr("Undefined");
	}
}

GameRunner *Server::gameRunner() const
{
	return new GameRunner(this);
}

QString Server::hostName(bool forceAddress) const
{
	if(!forceAddress && gConfig.doomseeker.bLookupHosts &&
		serverHost.error() == QHostInfo::NoError && serverHost.lookupId() != -1)
	{
		return QString("%1:%2").arg(serverHost.hostName()).arg(port());
	}
	return QString("%1:%2").arg(address().toString()).arg(port());
}

const QPixmap &Server::icon() const
{
	return plugin()->icon();
}

bool Server::isEmpty() const
{
	return players->numClients() == 0;
}

bool Server::isFull() const
{
	return players->numClients() == maximumClients();
}

bool Server::isRefreshable() const
{
	return time.secsTo(QTime::currentTime()) >= plugin()->data()->refreshThreshold;
}

void Server::lookupHost()
{
	QHostInfo::lookupHost(serverAddress.toString(), this, SLOT( setHostName(QHostInfo) ));
}

int Server::numFreeClientSlots() const
{
	int returnValue = maximumClients() - players->numClients();
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

const Player& Server::player(int index) const
{
	return (*players)[index];
}

bool Server::refresh()
{
	if (Main::refreshingThread == NULL)
	{
		emitUpdated(RESPONSE_BAD);
		gLog << tr("CRITIAL ERROR: REFRESHING THREAD IS NULL");
		return false;
	}

	if(isRefreshable())
	{
		Main::refreshingThread->registerServer(this);
		return true;
	}
	return false;
}

void Server::refreshStarts()
{
	bIsRefreshing = true;

	emit begunRefreshing(this);
	triesLeft = gConfig.doomseeker.queryTries;
	if(triesLeft > 10) // Limit the maximum number of tries
	{
		triesLeft = 10;
	}
}

void Server::refreshStops()
{
	bIsRefreshing = false;
	iwad = iwad.toLower();
}

bool Server::sendRefreshQuery(QUdpSocket* socket)
{
	if(triesLeft <= 0)
	{
		emitUpdated(Server::RESPONSE_TIMEOUT);
		refreshStops();
		return false;
	}
	--triesLeft;

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

void Server::setHostName(QHostInfo host)
{
	serverHost = host;
	if(!bIsRefreshing)
		emit updated(this, lastResponse());
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

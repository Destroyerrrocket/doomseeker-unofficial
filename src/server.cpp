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

#include "server.h"
#include <QSet>

QString Player::teamNames[] = { "Blue", "Red", "Green", "Gold" };


Player::Player(const QString &name, unsigned short score, unsigned short ping, PlayerTeam team, bool spectator, bool bot) :
	playerName(name), currentScore(score), currentPing(ping), team(team), spectator(spectator), bot(bot)
{
}

QString	Player::nameFormatted() const
{
	QString ret;
	for (int i = 0; i < playerName.length(); ++i)
	{
		if (playerName[i] < 32 || playerName[i] > 126)
		{
			++i;
			continue;
		}

		char c = playerName[i].toAscii();

		switch (playerName[i].toAscii())
		{
			case '<':
				ret += "&lt;";
				break;

			case '>':
				ret += "&gt;";
				break;

			default:
				ret += playerName[i];
				break;
		}
	}

	return ret;
}

QString Player::nameColorTagsStripped() const
{
	QString ret;
	for (int i = 0; i < playerName.length(); ++i)
	{
		if (playerName[i] < 32 || playerName[i] > 126)
		{
			++i;
			continue;
		}

		ret += playerName[i];
		break;
	}
	return ret;
}

QString Player::teamName(int team)
{
	if (team >= MAX_TEAMS)
		return "";

	return teamNames[team];
}

////////////////////////////////////////////////////////////////////////////////

// I really don't know if the game modes will need to be translated but I
// wrapped them in tr() just to be safe.
const GameMode GameMode::COOPERATIVE(QObject::tr("Cooperative"), false);
const GameMode GameMode::DEATHMATCH(QObject::tr("Deathmatch"), false);
const GameMode GameMode::TEAM_DEATHMATCH(QObject::tr("Team DM"), true);
const GameMode GameMode::CAPTURE_THE_FLAG(QObject::tr("CTF"), true);

GameMode::GameMode(const QString &name, bool teamgame) : modeName(name), teamgame(teamgame)
{
}

////////////////////////////////////////////////////////////////////////////////

Server::Server(const QHostAddress &address, unsigned short port) : QObject(),
	serverAddress(address), serverPort(port),
	currentGameMode(GameMode::COOPERATIVE), currentPing(999), locked(false),
	maxClients(0), maxPlayers(0), serverName(tr("<< ERROR >>")),
	serverScoreLimit(0), serverTimeLeft(0), serverTimeLimit(0)
{
	for(int i = 0;i < MAX_TEAMS;i++)
		scores[i] = 0;
}

Server::Server(const Server &other) : QObject(), currentGameMode(GameMode::COOPERATIVE)
{
	(*this) = other;
}

Server::~Server()
{
}

unsigned int Server::longestPlayerName() const
{
	unsigned int x = 0;
	for (int i = 0; i < numPlayers(); ++i)
	{
		unsigned int len = players[i].nameColorTagsStripped().length();
		if (len > x)
			x = len;
	}
	return x;
}

int Server::teamPlayerCount(int team) const
{
	if (team >= MAX_TEAMS)
	{
		return -1;
	}

	int teamSize = 0;
	for (int i = 0; i < players.count(); ++i)
	{
		const Player& p = players[i];
		if (p.teamNum() == team)
		{
			++teamSize;
		}
	}
	return teamSize;
}

void Server::operator= (const Server &other)
{
	serverAddress = other.address();
	serverPort = other.port();

	currentGameMode = other.gameMode();
	currentPing = other.ping();
	locked = other.isLocked();
	maxClients = other.maximumClients();
	maxPlayers = other.maximumPlayers();
	serverName = other.name();
	serverScoreLimit = other.scoreLimit();
}

////////////////////////////////////////////////////////////////////////////////

QThreadPool ServerRefresher::threadPool;
QThreadPool	ServerRefresher::guardianThreadPool;
QMutex ServerRefresher::guardianMutex;
bool ServerRefresher::bGuardianExists = false;

void Server::refresh()
{
	ServerRefresher* r = new ServerRefresher(this);
	ServerRefresher::threadPool.start(r);
}

ServerRefresher::ServerRefresher(Server* p) : parent(p)
{
	bGuardian = false;
	if(threadPool.maxThreadCount() != 50)
		threadPool.setMaxThreadCount(50);
}

void ServerRefresher::startGuardian()
{
	guardianMutex.lock();
	if (!bGuardianExists)
	{
		bGuardianExists = true;
		bGuardian = true;
		guardianThreadPool.start(this);
	}
	guardianMutex.unlock();
}

void ServerRefresher::run()
{
	if (!bGuardian)
	{
		parent->doRefresh();
	}
	else
	{
		threadPool.waitForDone();
		emit allServersRefreshed();
		bGuardianExists = false;
	}
}

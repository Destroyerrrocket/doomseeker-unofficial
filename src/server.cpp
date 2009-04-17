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

// \c = '\034'
#define	ESCAPE_COLOR	'\034'

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
		// cut out bad characters
		if ((playerName[i] < 32 || playerName[i] > 126) && playerName[i] != ESCAPE_COLOR)
			continue;

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

	return colorizeString(ret);
}

QString Player::nameColorTagsStripped() const
{
	QString ret;
	for (int i = 0; i < playerName.length(); ++i)
	{
		if (playerName[i] < 32 || playerName[i] > 126)
		{
			// Lets only remove the following character on \c.
			// Removing the control characters is still a good idea though.
			if(playerName[i] == ESCAPE_COLOR)
				++i;
			continue;
		}

		ret += playerName[i];
	}
	return ret;
}

QString Player::teamName(int team)
{
	if (team >= MAX_TEAMS)
		return "";

	return teamNames[team];
}

const char Player::colorChart[20][7] =
{
	"FF91A4", //a
	"D2B48C", //b
	"C0C0C0", //c
	"32CD32", //d
	"918151", //e
	"F4C430", //f
	"E32636", //g
	"0000FF", //h
	"FF8C00", //i
	"EEEEEE", //j
	"FFD700", //k
	"E34234", //l
	"000000", //m
	"4169E1", //n
	"FFDEAD", //o
	"465945", //p
	"228b22", //q
	"800000", //r
	"704214", //s
	"A020F0", //t
};
QString Player::colorizeString(const QString &str, int current)
{
	QString ret;
	bool colored = false;
	for(int i = 0;i < str.length();i++)
	{
		if(str[i] == ESCAPE_COLOR)
		{
			i++;
			if(i >= str.length())
				break;
			QChar colorChar = str[i].toLower();
			int color = colorChar.toAscii() - 97;

			// special cases
			if(colorChar == '+')
				color = current == 19 ? 0 : current+1; // + is the current plus one, wrap if needed.
			else if(colorChar == '-' || colored)
			{
				if(colored)
				{
					ret += "</span>";
				}
				continue;
			}

			if(color >= 0 && color < 20)
			{
				ret += QString("<span style=\"color: #") + colorChart[color] + "\">";
			}
			continue;
		}
		ret += str[i];
	}
	if(colored)
		ret += "</span>";
	return ret;
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
	if (bRunning)
		return;

	startRunning();
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
		parent->stopRunning();
	}
	else
	{
		threadPool.waitForDone();
		emit allServersRefreshed();
		bGuardianExists = false;
	}
}

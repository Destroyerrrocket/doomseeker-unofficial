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
#include "main.h"
#include <QProcess>
#include <QSet>
#include <QTime>
#include <QUdpSocket>

// \c = '\034'
#define	ESCAPE_COLOR	'\034'

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

QString Player::teamNames[] =
{
	"Blue",
	"Red",
	"Green",
	"Gold"
};

const char Player::colorChart[20][7] =
{
	"FF91A4", //a
	"D2B48C", //b
	"808080", //c
	"32CD32", //d
	"918151", //e
	"F4C430", //f
	"E32636", //g
	"0000FF", //h
	"FF8C00", //i
	"C0C0C0", //j
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

Server::Server(const QHostAddress &address, unsigned short port) : QObject(),
	serverAddress(address), serverPort(port),
	currentGameMode(GameMode::COOPERATIVE), currentPing(999), locked(false),
	maxClients(0), maxPlayers(0), serverName(tr("<< ERROR >>")),
	serverScoreLimit(0), serverTimeLeft(0), serverTimeLimit(0)
{
	bKnown = false;
	bRunning = false;
	for(int i = 0;i < MAX_TEAMS;i++)
		scores[i] = 0;

	connect(this, SIGNAL( updated(Server*, int) ), this, SLOT( setResponse(Server*, int) ));
}

Server::Server(const Server &other) : QObject(), currentGameMode(GameMode::COOPERATIVE)
{
	(*this) = other;
	connect(this, SIGNAL( updated(Server*, int) ), this, SLOT( setResponse(Server* server, int response) ));
}

Server::~Server()
{
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

QList<ServerInfo>* Server::serverInfo() const
{
	QList<ServerInfo>* list = new QList<ServerInfo>();

	ServerInfo siName = { this->name(), "<div style='white-space: pre'>" + this->name() + "</div>" };

	QString addr = this->address().toString() + ":" + QString::number(this->port());
	ServerInfo siAddress = { addr, addr };

	list->append(siName);
	list->append(siAddress);

	additionalServerInfo(list);
	return list;
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

void Server::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound) const
{
	// Connect
	args << "+connect" << QString(address().toString() + ":" + QString::number(port()));

	// Iwad
	QString iwad = pf.findWad(iwadName().toLower());
	args << "-iwad" << iwad;
	iwadFound = !iwad.isEmpty();
}

////////////////////////////////////////////////////////////////////////////////

QThreadPool ServerRefresher::threadPool;
QMutex ServerRefresher::guardianMutex;
bool ServerRefresher::bGuardianExists = false;

void Server::doRefresh()
{
	// Connect to the server
	QUdpSocket socket;

	socket.connectToHost(address(), port());

	if(!socket.waitForConnected(1000))
	{
		printf("%s\n", socket.errorString().toAscii().data());
		emitUpdated(Server::RESPONSE_BAD);
		return;
	}

	QByteArray request;
	if(!sendRequest(request))
		return;

	// start timer and write.
	QTime time = QTime::currentTime();
	socket.write(request);
	time.start();
	if(!socket.waitForReadyRead(5000))
	{
		emitUpdated(Server::RESPONSE_TIMEOUT);
		return;
	}

	// Read
	QByteArray data = socket.readAll();
	currentPing = time.elapsed();
	if(!readRequest(data))
		return;

	socket.close();

	emitUpdated(Server::RESPONSE_GOOD);
}

void Server::refresh()
{
	if (bRunning)
		return;

	startRunning();
	ServerRefresher* r = new ServerRefresher(this);
	ServerRefresher::threadPool.start(r);
}

void Server::finalizeRefreshing()
{
	iwad = iwad.toLower();
}

ServerRefresher::ServerRefresher(Server* p) : parent(p)
{
	bGuardian = false;
	if(threadPool.maxThreadCount() != 50)
	{
		threadPool.setMaxThreadCount(50);
	}
}

void ServerRefresher::startGuardian()
{
	guardianMutex.lock();
	if (!bGuardianExists)
	{
		bGuardianExists = true;
		bGuardian = true;
		QThreadPool::globalInstance()->start(this);
	}
	guardianMutex.unlock();
}

void ServerRefresher::run()
{
	if (!bGuardian)
	{
		// If the program is no longer running then do nothing.
		if(Main::running)
		{
			parent->doRefresh();
			parent->finalizeRefreshing();
			parent->stopRunning();
		}
	}
	else
	{
		threadPool.waitForDone();
		emit allServersRefreshed();
		bGuardianExists = false;
	}
}

//------------------------------------------------------------------------------
// zdaemonserver.cpp
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#include <QBuffer>
#include <QDataStream>
#include <QTime>

#include "zdaemongameinfo.h"
#include "zdaemongamerunner.h"
#include "zdaemonengineplugin.h"
#include "zdaemonserver.h"
#include "datastreamoperatorwrapper.h"
#include "main.h"
#include "serverapi/playerslist.h"

TeamInfo::TeamInfo(const QString &name, const QColor &color, unsigned int score) :
	teamName(name), teamColor(color), teamScore(score)
{
}

////////////////////////////////////////////////////////////////////////////////

#define SERVER_CHALLENGE 0xA3,0xDB,0x0B,0x00
#define SERVER_PROTOCOL 0xFC,0xFD,0xFE,0xFF
#define SERVER_VERSION 0x03,0x00,0x00,0x00
#define SERVER_RESPONSE 0x54D6D6

ZDaemonServer::ZDaemonServer(const QHostAddress &address, unsigned short port)
: Server(address, port)
{
	currentGameMode = (*ZDaemonGameInfo::gameModes())[ZDaemonGameInfo::MODE_DEATHMATCH];
}

GameRunner* ZDaemonServer::gameRunner()
{
	return new ZDaemonGameRunner(this);
}

unsigned int ZDaemonServer::millisecondTime()
{
	const QTime time = QTime::currentTime();
	return time.hour()*360000 + time.minute()*60000 + time.second()*1000 + time.msec();
}

const EnginePlugin* ZDaemonServer::plugin() const
{
	return ZDaemonEnginePlugin::staticInstance();
}

Server::Response ZDaemonServer::readRequest(QByteArray &data)
{
	QBuffer ioBuffer(&data);
	ioBuffer.open(QIODevice::ReadOnly);
	QDataStream inStream(&ioBuffer);
	inStream.setByteOrder(QDataStream::LittleEndian);
	DataStreamOperatorWrapper in(&inStream);
	
	quint32 magicNumber = in.readQUInt32();
	if(magicNumber != SERVER_RESPONSE)
		return RESPONSE_BAD;

	int protoVersion = in.readQUInt32();
	int time = in.readQUInt32();
	
	in.skipRawData(3);

	serverName = in.readRawUntilByte('\0');

	players->clear();
	int numClients = in.readQUInt8();
	maxClients = in.readQUInt8();

	mapName = in.readRawUntilByte('\0');

	wads.clear();
	int numwads = in.readQUInt8();
	for(int i = 0;i < numwads;i++)
	{
		QString wadFile = in.readRawUntilByte('\0');
		char optional = in.readQUInt8();
		if(protoVersion == 1)
		{
			in.readRawUntilByte('\0');
		}
		else
		{
			QString tmp;
			do
			{
				in.skipRawData(16);
				tmp = in.readRawUntilByte('\0');
			}
			while(tmp[0] != '\0');
		}

		wads << PWad(wadFile, optional != 0);
	}

	int gameMode = in.readQUInt8();
	if(gameMode >= 0 && gameMode < ZDaemonGameInfo::gameModes()->size())
		currentGameMode = (*ZDaemonGameInfo::gameModes())[gameMode];
	QString gameName = in.readRawUntilByte('\0');
	iwad = in.readRawUntilByte('\0');
	if(protoVersion == 1)
	{
		in.readRawUntilByte('\0');
	}
	else
	{
		QString tmp;
		do
		{
			in.skipRawData(16);
			tmp = in.readRawUntilByte('\0');
		}
		while(tmp[0] != '\0');
	}

	skill = in.readQUInt8();

	webSite = in.readRawUntilByte('\0');
	email = in.readRawUntilByte('\0');

	// [BL] OK we might need to do something about how much can be copy and
	//      pasted between plugins...
	clearDMFlags();
	const DMFlags& zdaemonFlags = *ZDaemonGameInfo::dmFlags();

	// Read each dmflags section separately.
	for (int i = 0; i < 2; ++i)
	{
		unsigned int dmflags = in.readQUInt32();

		const DMFlagsSection& zdaemonFlagsSection = *zdaemonFlags[i];
		DMFlagsSection* dmFlagsSection = new DMFlagsSection();
		dmFlagsSection->name = zdaemonFlagsSection.name;

		// Iterate through every known flag to check whether it should be
		// inserted into the structure of this server.
		for (int j = 0; j < zdaemonFlagsSection.flags.count(); ++j)
		{
			if ( (dmflags & (1 << zdaemonFlagsSection.flags[j].value)) != 0)
			{
				dmFlagsSection->flags << zdaemonFlagsSection.flags[j];
			}
		}

		dmFlags << dmFlagsSection;
	}

	locked = in.readQUInt8();
	int acl = in.readQUInt8();
	//int numPlayers = in.readQUInt8();
	in.skipRawData(1);
	maxPlayers = in.readQUInt8();
	serverTimeLimit = in.readQInt16();
	serverTimeLeft = in.readQInt16();
	serverScoreLimit = in.readQInt16();
	if(protoVersion == 1)
		in.skipRawData(11);
	else
		in.skipRawData(14);

	serverVersion = in.readRawUntilByte('\0');
	in.skipRawData(4);

	for(int i = 0;i < numClients;i++)
	{
		QString name = in.readRawUntilByte('\0');

		int score = in.readQUInt16();
		in.skipRawData(2);
		int ping = in.readQUInt16();
		int time = in.readQUInt16();
		bool isBot = in.readQUInt8() != 0;
		bool isSpectator = in.readQUInt8() != 0;
		int team = in.readQUInt8();
		if(team >= 4 || !currentGameMode.isTeamGame())
			team = Player::TEAM_NONE;
		Player player(name, score, ping, static_cast<Player::PlayerTeam> (team), isSpectator, isBot);
		*players << player;
		in.skipRawData(3);
	}

	int numTeams = in.readQUInt8();
	if(numTeams >= 2)
	{
		// The skipRawData(1) below may need to be above the if statement, 
		// actually.
		in.skipRawData(1);
		serverScoreLimit = in.readQInt16();
		in.skipRawData(3);
		for(int i = 0;i < ZD_MAX_TEAMS;i++)
		{
			QByteArray tmp = in.readRawUntilByte('\0');
			teamInfo[i].setName(tr(tmp.constData()));
			teamInfo[i].setColor(in.readQUInt32());
			teamInfo[i].setScore(in.readQUInt16());
		}
	}

	return RESPONSE_GOOD;
}

bool ZDaemonServer::sendRequest(QByteArray &data)
{
	// This construction and cast to (char*) removes warnings from MSVC.
	const unsigned char challenge[16] = { SERVER_CHALLENGE, SERVER_PROTOCOL, SERVER_VERSION, WRITEINT32_DIRECT(millisecondTime()) };
	
	const QByteArray challengeByteArray((char*)challenge, 16);
	data.append(challengeByteArray);
	return true;
}

QRgb ZDaemonServer::teamColor(int team) const
{
	if(team >= 0 && team < ZD_MAX_TEAMS)
		return teamInfo[team].color().rgb();
	return Server::teamColor(team);
}

QString ZDaemonServer::teamName(int team) const
{
	if(team >= 0 && team < ZD_MAX_TEAMS)
		return teamInfo[team].name();
	return "NO TEAM";
}

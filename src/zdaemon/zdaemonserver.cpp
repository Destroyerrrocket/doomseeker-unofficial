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

#include <QTime>

#include "zdaemon/zdaemonbinaries.h"
#include "zdaemon/zdaemongameinfo.h"
#include "zdaemon/zdaemongamerunner.h"
#include "zdaemon/zdaemonmain.h"
#include "zdaemon/zdaemonserver.h"
#include "main.h"
#include "serverapi/playerslist.h"

TeamInfo::TeamInfo(const QString &name, const QColor &color, unsigned int score) :
	teamName(name), teamColor(color), teamScore(score)
{
}

////////////////////////////////////////////////////////////////////////////////

const // clear warnings
#include "zdaemon/zdaemon.xpm"

#define SERVER_CHALLENGE 0xA3,0xDB,0x0B,0x00
#define SERVER_PROTOCOL 0xFC,0xFD,0xFE,0xFF
#define SERVER_VERSION 0x03,0x00,0x00,0x00
#define SERVER_RESPONSE 0x54D6D6

const QPixmap *ZDaemonServer::ICON = NULL;

ZDaemonServer::ZDaemonServer(const QHostAddress &address, unsigned short port)
: Server(address, port)
{
	currentGameMode = (*ZDaemonGameInfo::gameModes())[ZDaemonGameInfo::MODE_DEATHMATCH];
}

Binaries* ZDaemonServer::binaries() const
{
	return new ZDaemonBinaries();
}

GameRunner* ZDaemonServer::gameRunner() const
{
	return new ZDaemonGameRunner(this);
}

const QPixmap &ZDaemonServer::icon() const
{
	if(ICON == NULL)
		ICON = new QPixmap(zdaemon_xpm);
	return *ICON;
}

unsigned int ZDaemonServer::millisecondTime()
{
	const QTime time = QTime::currentTime();
	return time.hour()*360000 + time.minute()*60000 + time.second()*1000 + time.msec();
}

const PluginInfo* ZDaemonServer::plugin() const
{
	return ZDaemonMain::get();
}

Server::Response ZDaemonServer::readRequest(QByteArray &data)
{
	const char* in = data.data();

	if(READINT32(&in[0]) != SERVER_RESPONSE)
		return RESPONSE_BAD;

	int protoVersion = READINT32(&in[4]);
	int time = READINT32(&in[8]);

	int pos = 15;
	serverName = QString(&in[pos]);
	pos += serverName.length() + 1;

	players->clear();
	int numClients = in[pos++];
	maxClients = in[pos++];

	mapName = QString(&in[pos]);
	pos += mapName.length() + 1;

	int numwads = in[pos++];
	for(int i = 0;i < numwads;i++)
	{
		QString wadFile(&in[pos]);
		pos += wadFile.length()+1;
		char optional = in[pos++];
		if(protoVersion == 1)
		{
			QString tmp(&in[pos]);
			pos += tmp.length()+1;
		}
		else
		{
			QString tmp;
			do
			{
				tmp = QString(&in[pos+16]);
				pos += tmp.length()+17;
			}
			while(tmp[0] != '\0');
		}

		wads << PWad(wadFile, optional != 0);
	}

	int gameMode = in[pos++];
	if(gameMode >= 0 && gameMode < ZDaemonGameInfo::gameModes()->size())
		currentGameMode = (*ZDaemonGameInfo::gameModes())[gameMode];
	QString gameName(&in[pos]);
	pos += gameName.length()+1;
	iwad = QString(&in[pos]);
	pos += iwad.length()+1;
	if(protoVersion == 1)
	{
		QString tmp(&in[pos]);
		pos += tmp.length()+1;
	}
	else
	{
		QString tmp;
		do
		{
			tmp = QString(&in[pos+16]);
			pos += tmp.length()+17;
		}
		while(tmp[0] != '\0');
	}

	skill = in[pos++];

	webSite = QString(&in[pos]);
	pos += webSite.length()+1;

	email = QString(&in[pos]);
	pos += email.length()+1;

	// [BL] OK we might need to do something about how much can be copy and
	//      pasted between plugins...
	clearDMFlags();
	const DMFlags& zdaemonFlags = *ZDaemonGameInfo::dmFlags();

	// Read each dmflags section separately.
	for (int i = 0; i < 2; ++i)
	{
		unsigned int dmflags = READINT32(&in[pos]);
		pos += 4;

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

	locked = in[pos++];
	int acl = in[pos++];
	//int numPlayers = in[pos++];
	pos++;
	maxPlayers = in[pos++];
	serverTimeLimit = READINT16(&in[pos]);
	serverTimeLeft = READINT16(&in[pos+2]);
	serverScoreLimit = READINT16(&in[pos+4]);
	if(protoVersion == 1)
		pos += 17;
	else
		pos += 20;

	serverVersion = QString(&in[pos]);
	pos += serverVersion.length()+5;

	for(int i = 0;i < numClients;i++)
	{
		QString name(&in[pos]);
		pos += name.length()+1;

		int score = READINT16(&in[pos]);
		int ping = READINT16(&in[pos+4]);
		int time = READINT16(&in[pos+6]);
		bool isBot = in[pos+8] != 0;
		bool isSpectator = in[pos+9] != 0;
		int team = in[pos+10];
		if(team >= 4 || !currentGameMode.isTeamGame())
			team = Player::TEAM_NONE;
		Player player(name, score, ping, static_cast<Player::PlayerTeam> (team), isSpectator, isBot);
		*players << player;
		pos += 13;
	}

	int numTeams = READINT8(&in[pos++]);
	if(numTeams >= 2)
	{
		serverScoreLimit = READINT16(&in[pos+1]);
		pos += 6;
		for(int i = 0;i < ZD_MAX_TEAMS;i++)
		{
			teamInfo[i].setName(tr(&in[pos]));
			pos += teamInfo[i].name().length() + 1;
			teamInfo[i].setColor(READINT32(&in[pos]));
			teamInfo[i].setScore(READINT16(&in[pos+4]));
			pos += 6;
		}
	}

	return RESPONSE_GOOD;
}

bool ZDaemonServer::sendRequest(QByteArray &data)
{
	const char challenge[16] = { SERVER_CHALLENGE, SERVER_PROTOCOL, SERVER_VERSION, WRITEINT32_DIRECT(millisecondTime()) };
	const QByteArray chall(challenge, 16);
	data.append(chall);
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

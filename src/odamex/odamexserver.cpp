//------------------------------------------------------------------------------
// odamexserver.cpp
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

#include "odamexbinaries.h"
#include "odamexgameinfo.h"
#include "odamexgamerunner.h"
#include "odamexmain.h"
#include "odamexserver.h"
#include "main.h"
#include "serverapi/playerslist.h"

const // clear warnings
#include "odamex.xpm"

#define SERVER_CHALLENGE	0xA3,0xDB,0x0B,0x00
#define SERVER_GOOD			5560020

#define SPECTATOR_INFO		0x01020304
#define EXTRA_INFO			0x01020305

const QPixmap *OdamexServer::ICON = NULL;

OdamexServer::OdamexServer(const QHostAddress &address, unsigned short port) : Server(address, port),
	protocol(0)
{
}

Binaries* OdamexServer::binaries() const
{
	return new OdamexBinaries();
}

const QPixmap &OdamexServer::icon() const
{
	if(ICON == NULL)
		ICON = new QPixmap(odamex_xpm);
	return *ICON;
}

GameRunner* OdamexServer::gameRunner() const
{
	return new OdamexGameRunner(this);
}

const PluginInfo* OdamexServer::plugin() const
{
	return OdamexMain::get();
}

Server::Response OdamexServer::readRequest(QByteArray &data)
{
	fflush(stderr);
	fflush(stdout);
	const char* in = data.data();

	// Check the response code
	int response = READINT32(&in[0]);
	if(response != SERVER_GOOD)
	{
		return RESPONSE_BAD;
	}

	int pos = 8;

	// Server name
	serverName = QString(&in[pos]);
	pos += serverName.length() + 1;

	// Players
	int numPlayers = READINT8(&in[pos++]);
	maxClients = maxPlayers = READINT8(&in[pos++]);

	// Map
	mapName = QString(&in[pos]);
	pos += mapName.length() + 1;

	// Wads
	wads.clear();
	int wadCount = READINT8(&in[pos++]);
	if(wadCount > 0)
	{
		iwad = QString(&in[pos]);
		pos += iwad.length() + 1;
		for(int i = 1;i < wadCount;i++)
		{
			QString wad(&in[pos]);
			wads << wad;
			pos += wad.length() + 1;
		}
	}

	// Game mode
	const QList<GameMode>& gameModes = *plugin()->pInterface->gameModes();

	int mode = READINT8(&in[pos++]);
	skill = READINT8(&in[pos++]);
	int teamplay = READINT8(&in[pos++]);
	int ctf = READINT8(&in[pos++]);
	if(ctf == 1)
	{
		currentGameMode = gameModes[OdamexGameInfo::MODE_CAPTURE_THE_FLAG];
	}
	else if(teamplay == 1)
	{
		currentGameMode = gameModes[OdamexGameInfo::MODE_TEAM_DEATHMATCH];
	}
	else
	{
		currentGameMode = gameModes[mode];
	}
	// Players
	players->clear();
	for(int i = 0;i < numPlayers;i++)
	{
		QString name(&in[pos]);
		pos += name.length() + 1;
		short score = READINT16(&in[pos]);
		int ping = READINT32(&in[pos+2]);
		int team = READINT8(&in[pos+6]);
		pos += 7;

		Player player(name, score, ping, static_cast<Player::PlayerTeam> (teamplay ? team : Player::TEAM_NONE));
		(*players) << player;
	}

	// PWAD md5
	for(int i = 0;i < wads.size() + 1;i++)
	{
		QString md5 = QString(&in[pos]);
		pos += md5.length() + 1;
	}

	// Website
	webSite = QString(&in[pos]);
	pos += webSite.length() + 1;

	// team scores
	if(currentGameMode.isTeamGame())
	{
		serverScoreLimit = READINT32(&in[pos]);
		pos += 4;
		for(int i = 0;i < 3;i++)
		{
			if(READINT8(&in[pos++]) == 1)
			{
				scores[i] = READINT32(&in[pos]);
				pos += 4;
			}
		}
	}

	// protocol version
	protocol = READINT16(&in[pos]);
	pos += 2;

	// email
	email = QString(&in[pos]);
	pos += email.length() + 1;

	// time and frag limit
	serverTimeLimit = READINT16(&in[pos]);
	serverTimeLeft = READINT16(&in[pos+2]);
	if(!currentGameMode.isTeamGame())
		serverScoreLimit = READINT16(&in[pos+4]);
	pos += 6;

	// flags
	const DMFlags& dmFlagsOdamex = *plugin()->pInterface->allDMFlags();

	clearDMFlags();
	DMFlagsSection* dmFlagsSec = new DMFlagsSection();
	dmFlagsSec->name = dmFlagsOdamex[0]->name;
	dmFlags << dmFlagsSec;
	for(int i = 0;i < 14;i++)
	{
		if(READINT8(&in[pos++]) == 1)
		{
			dmFlagsSec->flags << dmFlagsOdamex[0]->flags[i];
		}
	}

	// Players 2
	pos += numPlayers*6;

	// [BL] Whoever decided this was a good idea needs to be shot.
	if(READINT32(&in[pos]) == SPECTATOR_INFO)
	{
		maxPlayers -= maxPlayers - READINT16(&in[pos+4]);
		pos += 6;

		for(int i = 0;i < numPlayers;i++)
		{
			Player& oldPlayer = (*players)[i];
			Player player(oldPlayer.name(), oldPlayer.score(), oldPlayer.ping(), oldPlayer.teamNum(), READINT8(&in[pos++]) == 1);
			players->replace(i, player);
		}
	}

	if(READINT32(&in[pos]) == EXTRA_INFO)
	{
		pos += 4;
		locked = READINT16(&in[pos]) == 1;
		pos += 2;
		int version = READINT32(&in[pos]);
		int version_major = version/256;
		int version_minor = (version % 256)/10;
		int version_patch = (version % 256)%10;
		serverVersion = QString("%1.%2.%3").arg(version_major).arg(version_minor).arg(version_patch);
		pos += 4;

		dehPatches.clear();
		int numPatches = READINT8(&in[pos++]);
		for(int i = 0;i < numPatches;i++)
		{
			QString patch(&in[pos]);
			dehPatches << patch;
			wads << patch; // So that it can be seeked.
			pos += patch.length()+1;
		}
	}

	return RESPONSE_GOOD;
}

bool OdamexServer::sendRequest(QByteArray &data)
{
	const char challenge[4] = {SERVER_CHALLENGE};
	const QByteArray chall(challenge, 4);
	data.append(chall);
	return true;
}

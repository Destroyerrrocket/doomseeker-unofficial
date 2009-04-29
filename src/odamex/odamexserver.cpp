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

#include "odamex/odamexserver.h"

#define SERVER_CHALLENGE	0xA3,0xDB,0x0B,0x00
#define SERVER_GOOD			5560020

#define SPECTATOR_INFO		0x01020304
#define EXTRA_INFO			0x01020305

const GameMode OdamexServer::GAME_MODES[NUM_ODAMEX_GAME_MODES] =
{
	GameMode::COOPERATIVE,
	GameMode::DEATHMATCH,
	GameMode(tr("Deathmatch 2.0"), false),
	GameMode::TEAM_DEATHMATCH,
	GameMode::CAPTURE_THE_FLAG
};

const QString OdamexServer::DMFLAGS[13] =
{
	tr("Items respawn"),
	tr("Weapons stay"),
	tr("Friendly fire"),
	tr("Allow exit"),
	tr("Infinite ammo"),
	tr("No monsters"),
	tr("Monsters respawn"),
	tr("Fast monsters"),
	tr("Jumping allowed"),
	tr("Freelook allowed"),
	tr("Wad can be downloaded"),
	tr("Server resets on empty"),
	tr("Kill anyone who tries to leave the level")
};

OdamexServer::OdamexServer(const QHostAddress &address, unsigned short port) : Server(address, port),
	protocol(0), skill(0), version(0)
{
}

bool OdamexServer::sendRequest(QByteArray &data)
{
	const char challenge[4] = {SERVER_CHALLENGE};
	data.append(challenge, 4);
	return true;
}

bool OdamexServer::readRequest(QByteArray &data)
{
	const char* in = data.data();

	// Check the response code
	int response = READINT32(&in[0]);
	if(response != SERVER_GOOD)
		emit updated(this, RESPONSE_BAD);

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
	int mode = READINT8(&in[pos++]);
	skill = READINT8(&in[pos++]);
	int teamplay = READINT8(&in[pos++]);
	int ctf = READINT8(&in[pos++]);
	if(ctf == 1)
		currentGameMode = GAME_MODES[MODE_CAPTURE_THE_FLAG];
	else if(teamplay == 1)
		currentGameMode = GAME_MODES[MODE_TEAM_DEATHMATCH];
	else
		currentGameMode = GAME_MODES[mode];

	// Players
	for(int i = 0;i < numPlayers;i++)
	{
		QString name(&in[pos]);
		pos += name.length() + 1;
		short score = READINT16(&in[pos]);
		int ping = READINT32(&in[pos+2]);
		int team = READINT8(&in[pos+6]);
		pos += 7;

		Player player(name, score, ping, static_cast<Player::PlayerTeam> (team));
		players << player;
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
	for(int i = 0;i < 13;i++)
	{
		if(READINT8(&in[pos++]) == 1)
			dmFlags << DMFLAGS[i];
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
			Player player(players[i].name(), players[i].score(), players[i].ping(), players[i].teamNum(), READINT8(&in[pos++]) == 1);
			players.replace(i, player);
		}
	}

	if(READINT32(&in[pos]) == EXTRA_INFO)
	{
		pos += 4;
		locked = READINT8(&in[pos++]) == 1;
		version = READINT32(&in[pos]);
	}

	emit updated(this, RESPONSE_GOOD);
	return true;
}

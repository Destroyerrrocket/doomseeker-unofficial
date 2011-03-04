//------------------------------------------------------------------------------
// chocolatedoomserver.cpp
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

#include "chocolatedoomserver.h"
#include "chocolatedoombinaries.h"
#include "chocolatedoomgamerunner.h"
#include "chocolatedoommain.h"
#include "global.h"
#include "main.h"
#include "serverapi/playerslist.h"

const // clear warnings
#include "chocolatedoom.xpm"

#define NET_PACKET_TYPE_QUERY			0,13
#define NET_PACKET_TYPE_QUERY_RESPONSE	14

const QPixmap *ChocolateDoomServer::ICON = NULL;

ChocolateDoomServer::ChocolateDoomServer(const QHostAddress &address, unsigned short port) : Server(address, port),
	serverState(0), game(0), gameMission(0)
{
}

Binaries* ChocolateDoomServer::binaries() const
{
	return new ChocolateDoomBinaries();
}

GameRunner*	ChocolateDoomServer::gameRunner() const
{
	return new ChocolateDoomGameRunner(this);
}

const QPixmap &ChocolateDoomServer::icon() const
{
	if(ICON == NULL)
		ICON = new QPixmap(chocolatedoom_xpm);
	return *ICON;
}

const PluginInfo* ChocolateDoomServer::plugin() const
{
	return ChocolateDoomMain::get();
}

Server::Response ChocolateDoomServer::readRequest(QByteArray &data)
{
	static const char* playerNames[4] =
	{
		"Green",
		"Indigo",
		"Brown",
		"Red"
	};

	const char* in = data.data();

	// Check the response code
	int response = READINT8(&in[1]);
	if(response != NET_PACKET_TYPE_QUERY_RESPONSE)
	{
		return RESPONSE_BAD;
	}

	serverVersion = QString(&in[2]);
	int pos = 2 + serverVersion.length()+1;

	serverState = READINT8(&in[pos++]);
	unsigned int numPlayers = READINT8(&in[pos++]);
	players->clear();
	for(unsigned int i = 0;i < numPlayers;i++)
	{
		*players << Player(playerNames[i < 4 ? i : 4], 0, 0, static_cast<Player::PlayerTeam> (Player::TEAM_NONE), false, false);
	}
	maxClients = maxPlayers = READINT8(&in[pos++]);
	game = READINT8(&in[pos++]);
	gameMission = READINT8(&in[pos++]);
	serverName = QString(&in[pos]);

	switch(game)
	{
		default:
		case 0: //shareware
			iwad = "doom1.wad";
			break;
		case 1: //registered
		case 3: //retail
			iwad = "doom.wad";
			break;
		case 2: //commercial
			switch(gameMission)
			{
				default:
				case 1:
					iwad = "doom2.wad";
					break;
				case 2:
					iwad = "tnt.wad";
					break;
				case 3:
					iwad = "plutonia.wad";
					break;
			}
			break;
	}
	return RESPONSE_GOOD;
}

bool ChocolateDoomServer::sendRequest(QByteArray &data)
{
	char challenge[2] = { NET_PACKET_TYPE_QUERY };
	const QByteArray packet(challenge, 2);
	data.append(packet);
	return true;
}

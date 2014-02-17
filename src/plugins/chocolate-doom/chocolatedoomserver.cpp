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

#include "chocolatedoomgamehost.h"
#include "chocolatedoomgamerunner.h"
#include "chocolatedoomengineplugin.h"
#include "global.h"
#include "serverapi/playerslist.h"

#define NET_PACKET_TYPE_QUERY			0,13
#define NET_PACKET_TYPE_QUERY_RESPONSE	14

ChocolateDoomServer::ChocolateDoomServer(const QHostAddress &address, unsigned short port)
: Server(address, port), serverState(0), game(0), gameMission(0)
{
	set_createSendRequest(&ChocolateDoomServer::createSendRequest);
	set_readRequest(&ChocolateDoomServer::readRequest);
}

GameClientRunner* ChocolateDoomServer::gameRunner()
{
	return new ChocolateDoomGameClientRunner(
		self().toStrongRef().staticCast<ChocolateDoomServer>());
}

EnginePlugin* ChocolateDoomServer::plugin() const
{
	return ChocolateDoomEnginePlugin::staticInstance();
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

	setGameVersion(QString(&in[2]));
	int pos = 2 + gameVersion().length()+1;

	serverState = READINT8(&in[pos++]);
	unsigned int numPlayers = READINT8(&in[pos++]);
	clearPlayersList();
	for(unsigned int i = 0;i < numPlayers;i++)
	{
		addPlayer(Player(playerNames[i < 4 ? i : 4], 0, 0, static_cast<Player::PlayerTeam> (Player::TEAM_NONE), false, false));
	}
	int clients = READINT8(&in[pos++]);
	setMaxClients(clients);
	setMaxPlayers(clients);
	game = READINT8(&in[pos++]);
	gameMission = READINT8(&in[pos++]);
	setName(QString(&in[pos]));

	switch(game)
	{
		default:
		case 0: //shareware
			setIwad("doom1.wad");
			break;
		case 1: //registered
		case 3: //retail
			setIwad("doom.wad");
			break;
		case 2: //commercial
			switch(gameMission)
			{
				default:
				case 1:
					setIwad("doom2.wad");
					break;
				case 2:
					setIwad("tnt.wad");
					break;
				case 3:
					setIwad("plutonia.wad");
					break;
			}
			break;
	}
	return RESPONSE_GOOD;
}

QByteArray ChocolateDoomServer::createSendRequest()
{
	char challenge[2] = { NET_PACKET_TYPE_QUERY };
	QByteArray packet(challenge, 2);
	return packet;
}

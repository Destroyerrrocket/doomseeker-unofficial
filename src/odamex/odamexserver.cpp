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

/// Macro that checks the readRequest() validity.
#define CHECK_POS if (pos >= dataLength) \
		{ \
			return RESPONSE_BAD; \
		}

#define SERVER_CHALLENGE	0x02,0x10,0x01,0xAD, 0x32,0x00,0x00,0x00, 0x02,0x00,0x00,0x00, 0,0,0,0

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
	const char* in = data.data();
	unsigned int dataLength = (unsigned)data.length();

	// Check the response code
	int response = READINT32(&in[0]);
	if((response&0xFFF00000) != 0xAD000000)
		return RESPONSE_BAD;

	unsigned int version = READINT32(&in[4]);
	short version_major = version/256;
	short version_minor = (version % 256)/10;
	short version_patch = (version % 256)%10;
	serverVersion = QString("%1.%2.%3").arg(version_major).arg(version_minor).arg(version_patch);

	unsigned int protocolVersion = READINT32(&in[8]);
	unsigned int pos = 12;
	if(protocolVersion >= 2)
	{
		pos += 8;
	}
	
	CHECK_POS;
	
	serverVersion += QString(" r%1").arg(READINT32(&in[pos]));
	pos += 4;
	
	CHECK_POS;

	short cvarCount = READINT8(&in[pos++]);
	while(cvarCount-- > 0)
	{
		QString cvarName(&in[pos]);
		pos += cvarName.length()+1;
		CHECK_POS;
		
		QString cvarValue(&in[pos]);
		pos += cvarValue.length()+1;
		CHECK_POS;

		if(cvarName == "sv_email")
			email = cvarValue;
		else if(cvarName == "sv_hostname")
			serverName = cvarValue;
		else if(cvarName == "sv_maxplayers")
			maxPlayers = cvarValue.toUInt();
		else if(cvarName == "sv_maxclients")
			maxClients = cvarValue.toUInt();
		else if(cvarName == "sv_scorelimit")
			serverScoreLimit = cvarValue.toUInt();
		else if(cvarName == "sv_gametype")
		{
			unsigned int mode = cvarValue.toUInt();
			if(mode < (unsigned)plugin()->pInterface->gameModes()->size())
				currentGameMode = (*plugin()->pInterface->gameModes())[cvarValue.toUInt()];
		}
		else if(cvarName == "sv_website")
			webSite = cvarValue;
	}

	QString passwordHash(&in[pos]);
	pos += passwordHash.length()+1;
	CHECK_POS;

	mapName = QString(&in[pos]);
	pos += mapName.length()+1;
	CHECK_POS;

	serverTimeLeft = READINT16(&in[pos]);
	pos += 2;

	short teamCount = READINT8(&in[pos++]);
	while(teamCount-- > 0)
	{
		CHECK_POS;
	
		QString teamName(&in[pos]);
		pos += teamName.length()+1;
		pos += 6;
	}

	dehPatches.clear();
	short patchCount = READINT8(&in[pos++]);
	while(patchCount-- > 0)
	{
		CHECK_POS;
		
		QString patch(&in[pos]);
		pos += patch.length()+1;
		dehPatches << patch;
	}

	wads.clear();
	short wadCount = READINT8(&in[pos++]);
	for(short i = 0;i < wadCount;i++)
	{
		CHECK_POS;
	
		QString wad(&in[pos]);
		if(i >= 2)
			wads << wad;
		else if(i == 1)
			iwad = wad;
		pos += wad.length()+1;
		
		CHECK_POS;

		QString hash(&in[pos]);
		pos += hash.length()+1;
	}

	players->clear();
	short playerCount = READINT8(&in[pos++]);
	while(playerCount-- > 0)
	{
		CHECK_POS;
	
		QString playerName(&in[pos]);
		pos += playerName.length()+1;
		Player player(playerName, READINT16(&in[pos+6]), READINT16(&in[pos+1]), static_cast<Player::PlayerTeam> (READINT8(&in[pos])), READINT8(&in[pos+5]));
		pos += 12;
		players->append(player);
	}

	return RESPONSE_GOOD;
}

bool OdamexServer::sendRequest(QByteArray &data)
{
	// This construction and cast to (char*) removes warnings from MSVC.
	const unsigned char challenge[] = {SERVER_CHALLENGE};
	
	const QByteArray challengeByteArray((char*)challenge, 16);
	data.append(challengeByteArray);
	return true;
}

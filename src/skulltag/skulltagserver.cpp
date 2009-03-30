//------------------------------------------------------------------------------
// skulltagserver.cpp
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

#include <QUdpSocket>
#include <QTime>

#include "huffman/huffman.h"
#include "skulltag/skulltagserver.h"
#include "global.h"

#define SERVER_CHALLENGE	0xC7,0x00,0x00,0x00
#define SERVER_GOOD			5660023
#define SERVER_BANNED		5660025
#define SERVER_BAD			5660024

#define ST_MAX_TEAMS		4

SkulltagServer::SkulltagServer(const QHostAddress &address, unsigned short port) : Server(address, port)
{
}

void SkulltagServer::refresh()
{
	// Connect to the server
	QUdpSocket socket;
	socket.connectToHost(address(), port());
	if(!socket.waitForConnected(1000))
	{
		printf("%s\n", socket.errorString().toAscii().data());
		return;
	}

	// Send launcher challenge.
	QTime time = QTime::currentTime();
	int query = SQF_STANDARDQUERY;
	const char challenge[12] = {SERVER_CHALLENGE,WRITEINT32_DIRECT(query),0x00,0x00,0x00,0x00};
	char challengeOut[16];
	int out = 16;
	g_Huffman.encode(challenge, challengeOut, 12, &out);
	socket.write(challengeOut, out);

	// Start the timer and wait
	time.start();
	if(!socket.waitForReadyRead(1000))
		return;

	// Decompress the response.
	QByteArray data = socket.readAll();
	const char* in = data.data();
	char packetOut[2000];
	out = 2000;
	g_Huffman.decode(in, packetOut, data.size(), &out);

	// Check the response code
	int response = READINT32(&packetOut[0]);
	currentPing = time.elapsed();
	if(response == SERVER_BANNED)
	{
		emit banned(this);
		return;
	}
	else if(response != SERVER_GOOD)
		return;

	QString version(&packetOut[12]);
	int pos = 12 + version.length() + 1;

	// now read the data.
	SkulltagQueryFlags flags = static_cast<SkulltagQueryFlags> (READINT32(&packetOut[pos]));
	pos += 4;
	if((flags & SQF_NAME) == SQF_NAME)
	{
		serverName = QString(&packetOut[pos]);
		pos += serverName.length() + 1;
	}
	if((flags & SQF_URL) == SQF_URL)
	{
		webSite = QString(&packetOut[pos]);
		pos += webSite.length() + 1;
	}
	if((flags & SQF_EMAIL) == SQF_EMAIL)
	{
		email = QString(&packetOut[pos]);
		pos += email.length() + 1;
	}
	if((flags & SQF_MAPNAME) == SQF_MAPNAME)
	{
		mapName = QString(&packetOut[pos]);
		pos += mapName.length() + 1;
	}
	if((flags & SQF_MAXCLIENTS) == SQF_MAXCLIENTS)
		maxClients = READINT8(&packetOut[pos++]);
	if((flags & SQF_MAXPLAYERS) == SQF_MAXPLAYERS)
		maxPlayers = READINT8(&packetOut[pos++]);
	if((flags & SQF_PWADS) == SQF_PWADS)
	{
		int numPwads = READINT8(&packetOut[pos++]);
		for(int i = 0;i < numPwads;i++)
		{
			QString wad(&packetOut[pos]);
			pos += wad.length() + 1;
			wads << wad;
		}
	}
	if((flags & SQF_GAMETYPE) == SQF_GAMETYPE)
	{
		//byte: mode
		//byte: instagib?
		//byte: buckshot?
		pos += 3;
	}
	if((flags & SQF_GAMENAME) == SQF_GAMENAME)
	{
		//Useless String
		pos += strlen(&packetOut[pos]) + 1;
	}
	if((flags & SQF_IWAD) == SQF_IWAD)
	{
		iwad = QString(&packetOut[pos]);
		pos += iwad.length() + 1;
	}
	if((flags & SQF_FORCEPASSWORD) == SQF_FORCEPASSWORD)
	{
		if(READINT8(&packetOut[pos++]) != 0)
			locked = true;
	}
	if((flags & SQF_FORCEJOINPASSWORD) == SQF_FORCEJOINPASSWORD)
	{
		if(READINT8(&packetOut[pos++]) != 0)
			locked = true;
	}
	if((flags & SQF_GAMESKILL) == SQF_GAMESKILL)
		skill = READINT8(&packetOut[pos++]);
	if((flags & SQF_BOTSKILL) == SQF_BOTSKILL)
		botSkill = READINT8(&packetOut[pos++]);
	if((flags & SQF_DMFLAGS) == SQF_DMFLAGS)
	{
		// three long ints that represent dmflags, dmflags2, and compatflags respectively.
		pos += 12;
	}
	if((flags & SQF_LIMITS) == SQF_LIMITS)
	{
		// Shorts for fraglimit, timelimit, time left, duellimit, pointlimit, winlimit
		pos += 12;
	}
	if((flags & SQF_TEAMDAMAGE) == SQF_TEAMDAMAGE)
	{
		// float
		pos += 4;
	}
	if((flags & SQF_TEAMSCORES) == SQF_TEAMSCORES)
	{
		for(int i = 0;i < ST_MAX_TEAMS;i++)
		{
			scores[i] = READINT16(&packetOut[pos]);
			pos += 2;
		}
	}
	if((flags & SQF_NUMPLAYERS) == SQF_NUMPLAYERS)
	{
		int numPlayers = READINT8(&packetOut[pos++]);
		if((flags & SQF_PLAYERDATA) == SQF_PLAYERDATA)
		{
			QString name(&packetOut[pos]);
			pos += name.length() + 1;
			int score = READINT16(&packetOut[pos]);
			int ping = READINT16(&packetOut[pos+2]);
			bool spectating = READINT8(&packetOut[pos+4]) != 0;
			bool bot = READINT8(&packetOut[pos+5]) != 0;
			int team = READINT8(&packetOut[pos+6]);
			int time = READINT8(&packetOut[pos+7]);
			pos += 8;

			Player player(name, score, ping, static_cast<Player::PlayerTeam> (team), spectating, bot);
			players << player;
		}
	}

	socket.close();

	emit updated(this);
}

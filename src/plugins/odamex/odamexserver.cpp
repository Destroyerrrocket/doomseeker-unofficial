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
#include "odamexserver.h"

#include <QBuffer>
#include <QDataStream>

#include "odamexgamehost.h"
#include "odamexgameinfo.h"
#include "odamexgamerunner.h"
#include "odamexengineplugin.h"
#include "datastreamoperatorwrapper.h"
#include "plugins/engineplugin.h"
#include "serverapi/playerslist.h"

/// Macro that checks the readRequest() validity.
#define CHECK_POS if (!in.hasRemaining()) \
		{ \
			return RESPONSE_BAD; \
		}

#define CHECK_POS_OFFSET(offset) if (in.remaining() < (offset)) \
		{ \
			return RESPONSE_BAD; \
		}

#define SERVER_CHALLENGE	0x02,0x10,0x01,0xAD,0x40,0,0,0,5,0,0,0,0,0,0,0

#define SPECTATOR_INFO		0x01020304
#define EXTRA_INFO			0x01020305

OdamexServer::OdamexServer(const QHostAddress &address, unsigned short port)
: Server(address, port), protocol(0)
{
	set_readRequest(&OdamexServer::readRequest);
	set_createSendRequest(&OdamexServer::createSendRequest);
}

GameClientRunner* OdamexServer::gameRunner()
{
	return new OdamexGameClientRunner(
		self().toStrongRef().staticCast<OdamexServer>());
}

EnginePlugin* OdamexServer::plugin() const
{
	return OdamexEnginePlugin::staticInstance();
}

Server::Response OdamexServer::readRequest(const QByteArray &data)
{
	QBuffer ioBuffer;
	ioBuffer.setData(data);
	ioBuffer.open(QIODevice::ReadOnly);
	QDataStream inStream(&ioBuffer);
	inStream.setByteOrder(QDataStream::LittleEndian);
	DataStreamOperatorWrapper in(&inStream);

	// Check the response code
	int response = in.readQInt32();
	if((response&0xFFF00000) != 0xAD000000)
		return RESPONSE_BAD;

	unsigned int version = in.readQUInt32();
	short version_major = version/256;
	short version_minor = (version % 256)/10;
	short version_patch = (version % 256)%10;
	QString strVersion = QString("%1.%2.%3").arg(version_major).arg(version_minor).arg(version_patch);

	unsigned int protocolVersion = in.readQUInt32();
	if(protocolVersion >= 1)
	{
		in.skipRawData(8);
	}

	CHECK_POS;

	strVersion += QString(" r%1").arg(in.readQUInt32());
	setGameVersion(strVersion);

	CHECK_POS;

	short cvarCount = in.readQUInt8();
	while(cvarCount-- > 0)
	{
		QString cvarName = in.readRawUntilByte('\0');
		CHECK_POS;

		enum CVarType
		{
			CVAR_BOOL = 1,
			CVAR_INT8,
			CVAR_INT16,
			CVAR_INT32,
			CVAR_FLOAT,
			CVAR_STRING
		};
		CVarType type = CVAR_STRING;
		if(protocolVersion >= 5)
			type = static_cast<CVarType>(in.readQUInt8());

		QString cvarValue;
		switch(type)
		{
			case CVAR_BOOL:
				cvarValue = "1";
				break;
			case CVAR_INT8:
				cvarValue = QString("%1").arg(in.readQInt8());
				break;
			case CVAR_INT16:
				cvarValue = QString("%1").arg(in.readQInt16());
				break;
			case CVAR_INT32:
				cvarValue = QString("%1").arg(in.readQInt32());
				break;
			default:
				cvarValue = in.readRawUntilByte('\0');
				break;
		}
		CHECK_POS;

		if(cvarName == "sv_email")
			setEmail(cvarValue);
		else if(cvarName == "sv_hostname")
			setName(cvarValue);
		else if(cvarName == "sv_maxplayers")
			setMaxPlayers(cvarValue.toUInt());
		else if(cvarName == "sv_maxclients")
			setMaxClients(cvarValue.toUInt());
		else if(cvarName == "sv_scorelimit")
			setScoreLimit(cvarValue.toUInt());
		else if(cvarName == "sv_gametype")
		{
			unsigned int mode = cvarValue.toUInt();
			if(mode < (unsigned)plugin()->data()->gameModes->size())
			{
				setGameMode((*plugin()->data()->gameModes)[cvarValue.toUInt()]);
			}
		}
		else if(cvarName == "sv_website")
			setWebSite(cvarValue);
	}

	if(protocolVersion >= 4)
	{
		short hashLength = in.readQUInt8();
		in.skipRawData(hashLength);
		setLocked(hashLength > 0);
	}
	else
	{
		QString passwordHash = in.readRawUntilByte('\0');
		setLocked(!passwordHash.isEmpty());
	}
	CHECK_POS;

	setMap(in.readRawUntilByte('\0'));
	CHECK_POS;

	setTimeLeft(in.readQUInt16());

	if(protocolVersion < 5 || gameMode().isTeamGame())
	{
		short teamCount = in.readQUInt8();
		while(teamCount-- > 0)
		{
			CHECK_POS;

			QString teamName = in.readRawUntilByte('\0');
			in.skipRawData(6);
		}
	}

	dehPatches.clear();
	short patchCount = in.readQUInt8();
	while(patchCount-- > 0)
	{
		CHECK_POS;

		QString patch = in.readRawUntilByte('\0');
		dehPatches << patch;
	}

	clearWads();
	short wadCount = in.readQUInt8();
	for(short i = 0;i < wadCount;i++)
	{
		CHECK_POS;

		QString wad = in.readRawUntilByte('\0');
		if(i >= 2)
			addWad(wad);
		else if(i == 1)
			setIwad(wad);

		CHECK_POS;
		if(protocolVersion >= 4)
		{
			short hashLength = in.readQUInt8();
			in.skipRawData(hashLength);
		}
		else
			in.readRawUntilByte('\0');
	}

	clearPlayersList();
	short playerCount = in.readQUInt8();
	while(playerCount-- > 0)
	{
		CHECK_POS;

		QString playerName = in.readRawUntilByte('\0');

		if(protocolVersion >= 2)
		{
			CHECK_POS_OFFSET(4);
			// Player color
			in.skipRawData(4);
		}

		unsigned short teamIndex = Player::TEAM_NONE;
		if(protocolVersion < 5 || gameMode().isTeamGame())
		{
			CHECK_POS_OFFSET(1);
			teamIndex = in.readQUInt8();
		}
		CHECK_POS_OFFSET(11);
		unsigned short ping = in.readQUInt16();
		in.skipRawData(2);
		bool spectator = in.readQUInt8();
		unsigned short score = in.readQUInt16();
		in.skipRawData(4);

		Player::PlayerTeam team = gameMode().isTeamGame() ? static_cast<Player::PlayerTeam> (teamIndex) : Player::TEAM_NONE;
		Player player(playerName, score, ping, team, spectator);
		addPlayer(player);
	}

	if (maxPlayers() == 2 && gameMode().index() == GameMode::SGM_Deathmatch)
	{
		setGameMode(OdamexGameInfo::gameModeDuel());
	}

	return RESPONSE_GOOD;
}

QByteArray OdamexServer::createSendRequest()
{
	// This construction and cast to (char*) removes warnings from MSVC.
	const unsigned char challenge[] = {SERVER_CHALLENGE};

	QByteArray challengeByteArray((char*)challenge, 16);
	return challengeByteArray;
}

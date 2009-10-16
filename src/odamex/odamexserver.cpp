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
#include "main.h"

const // clear warnings
#include "odamex/odamex.xpm"

#define SERVER_CHALLENGE	0xA3,0xDB,0x0B,0x00
#define SERVER_GOOD			5560020

#define SPECTATOR_INFO		0x01020304
#define EXTRA_INFO			0x01020305

const QPixmap *OdamexServer::ICON = NULL;

const GameMode OdamexServer::GAME_MODES[NUM_ODAMEX_GAME_MODES] =
{
	GameMode::COOPERATIVE,
	GameMode::DEATHMATCH,
	GameMode(MODE_DEATHMATCH2, tr("Deathmatch 2.0"), false),
	GameMode::TEAM_DEATHMATCH,
	GameMode::CAPTURE_THE_FLAG
};

const DMFlagsSection OdamexServer::DM_FLAGS =
{
	tr("DMFlags"),
	13,
	{
		{ tr("Items respawn"),									0 },
		{ tr("Weapons stay"),									1 },
		{ tr("Friendly fire"),									2 },
		{ tr("Allow exit"),										3 },
		{ tr("Infinite ammo"),									4 },
		{ tr("No monsters"),									5 },
		{ tr("Monsters respawn"),								6 },
		{ tr("Fast monsters"),									7 },
		{ tr("Jumping allowed"),								8 },
		{ tr("Freelook allowed"),								9 },
		{ tr("Wad can be downloaded"),							10 },
		{ tr("Server resets on empty"),							11 },
		{ tr("Kill anyone who tries to leave the level"),		12 }
	}
};

OdamexServer::OdamexServer(const QHostAddress &address, unsigned short port) : Server(address, port),
	protocol(0)
{
}

QString	OdamexServer::clientBinary(QString& error) const
{
	SettingsData* setting = Main::config->setting("OdamexBinaryPath");

	if (setting->string().isEmpty())
	{
		error = tr("No executable specified for Odamex");
		return QString();
	}

	QFileInfo fi(setting->string());

	if (!fi.exists() || fi.isDir())
	{
		error = tr("%1\n is a directory or doesn't exist.").arg(setting->string());
		return QString();
	}

	return setting->string();
}

void OdamexServer::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword) const
{
	Server::connectParameters(args, pf, iwadFound, connectPassword);

	args << Main::config->setting("OdamexCustomParameters")->string().split(" ", QString::SkipEmptyParts);

	if(iwadFound)
	{
		// Waddir - Work around for an Odamex bug.
		args << "-waddir";
		QString waddir = pf.findWad(iwadName().toLower());
		waddir.truncate(waddir.length() - iwadName().length());
		for(int i = 0;i < numWads();i++)
		{
			QString pwaddir = pf.findWad(wad(i).toLower());
			pwaddir.truncate(pwaddir.length() - wad(i).length());
			waddir += ":" + pwaddir;
		}
		args << waddir;
	}
}

const QPixmap &OdamexServer::icon() const
{
	if(ICON == NULL)
		ICON = new QPixmap(odamex_xpm);
	return *ICON;
}

bool OdamexServer::readRequest(QByteArray &data)
{
	fflush(stderr);
	fflush(stdout);
	const char* in = data.data();

	// Check the response code
	int response = READINT32(&in[0]);
	if(response != SERVER_GOOD)
	{
		emit updated(this, RESPONSE_BAD);
		return false;
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
	int mode = READINT8(&in[pos++]);
	skill = READINT8(&in[pos++]);
	int teamplay = READINT8(&in[pos++]);
	int ctf = READINT8(&in[pos++]);
	if(ctf == 1)
	{
		currentGameMode = GAME_MODES[MODE_CAPTURE_THE_FLAG];
	}
	else if(teamplay == 1)
	{
		currentGameMode = GAME_MODES[MODE_TEAM_DEATHMATCH];
	}
	else
	{
		currentGameMode = GAME_MODES[mode];
	}
	// Players
	players.clear();
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
	clearDMFlags();
	DMFlagsSection* dmFlagsSec = new DMFlagsSection();
	dmFlagsSec->size = 0;
	dmFlagsSec->name = DM_FLAGS.name;
	dmFlags << dmFlagsSec;
	for(int i = 0;i < 13;i++)
	{
		if(READINT8(&in[pos++]) == 1)
		{
			dmFlagsSec->flags[dmFlagsSec->size++] = DM_FLAGS.flags[i];
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
			Player player(players[i].name(), players[i].score(), players[i].ping(), players[i].teamNum(), READINT8(&in[pos++]) == 1);
			players.replace(i, player);
		}
	}

	if(READINT32(&in[pos]) == EXTRA_INFO)
	{
		pos += 4;
		locked = READINT8(&in[pos++]) == 1;
		int version = READINT32(&in[pos]);
		int version_major = version/256;
		int version_minor = (version % 256)/10;
		int version_patch = (version % 256)%10;
		serverVersion = QString("%1.%2.%3").arg(version_major).arg(version_minor).arg(version_patch);
	}

	return true;
}

bool OdamexServer::sendRequest(QByteArray &data)
{
	const char challenge[4] = {SERVER_CHALLENGE};
	const QByteArray chall(challenge, 4);
	data.append(chall);
	return true;
}

QString OdamexServer::serverBinary(QString& error) const
{
	SettingsData* setting = Main::config->setting("OdamexServerBinaryPath");

	if (setting->string().isEmpty())
	{
		error = tr("No server executable specified for Odamex");
		return QString();
	}

	QFileInfo fi(setting->string());

	if (!fi.exists() || fi.isDir())
	{
		error = tr("%1\nis a directory or doesn't exist.").arg(setting->string());
		return QString();
	}

	return setting->string();
}

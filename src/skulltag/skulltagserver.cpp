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

#include "huffman/huffman.h"
#include "skulltag/skulltagserver.h"
#include "global.h"
#include "main.h"
#include "md5/md5.h"

#include <QMessageBox>

const // clear warnings
#include "skulltag/skulltag.xpm"

#ifdef Q_OS_WIN32
#define ST_BINARY_NAME "skulltag.exe"
#define SCRIPT_FILE_EXTENSION ".bat"
#else
#define ST_BINARY_NAME "skulltag"
#define SCRIPT_FILE_EXTENSION ".sh"
#endif

#define SERVER_CHALLENGE	0xC7,0x00,0x00,0x00
#define SERVER_GOOD			5660023
#define SERVER_BANNED		5660025
#define SERVER_WAIT			5660024

#define RCON_PROTOCOL_VERSION	3

TeamInfo::TeamInfo(const QString &name, const QColor &color, unsigned int score) :
	teamName(name), teamColor(color), teamScore(score)
{
}

////////////////////////////////////////////////////////////////////////////////

const DMFlagsSection SkulltagServer::DM_FLAGS[NUM_DMFLAG_SECTIONS] =
{
	{
		tr("DMFlags"),
		30,
		{
			{ tr("Do not spawn health items (DM)"),					0 },
			{ tr("Do not spawn powerups (DM)"),						1 },
			{ tr("Weapons remain after pickup (DM)"),				2 },
			{ tr("Falling damage (ZDoom/Strife)"),					3 },
			{ tr("Falling damage (Hexen/Strife)"),					4 },
			{ tr("Stay on same map when someone exits (DM)"),		6 },
			{ tr("Spawn players as far as possible (DM)"),			7 },
			{ tr("Automatically respawn dead players (DM)"),		8 },
			{ tr("Don't spawn armor (DM)"),							9 },
			{ tr("Kill anyone who tries to exit the level (DM)"),	10 },
			{ tr("Infinite ammo"),									11 },
			{ tr("No monsters"),									12 },
			{ tr("Monsters respawn"),								13 },
			{ tr("Items other than invuln. and invis. respawn"),	14 },
			{ tr("Fast monsters"),									15 },
			{ tr("No jumping"),										16 },
			{ tr("No freelook"),									17 },
			{ tr("Respawn invulnerability and invisibility"),		18 },
			{ tr("Arbitrator FOV"),									19 },
			{ tr("No multiplayer weapons in cooperative"),			20 },
			{ tr("No crouching"),									21 },
			{ tr("Lose all old inventory on respawn (COOP)"),		22 },
			{ tr("Lose keys on respawn (COOP)"),					23 },
			{ tr("Lose weapons on respawn (COOP)"),					24 },
			{ tr("Lose armor on respawn (COOP)"),					25 },
			{ tr("Lose powerups on respawn (COOP)"),				26 },
			{ tr("Lose ammo on respawn (COOP)"),					27 },
			{ tr("Lose half your ammo on respawn (COOP)"),			28 },
			{ tr("Jumping allowed"),								29 },
			{ tr("Crouching allowed"),								30 }
		}
	},

	{
		tr("DMFlags2"),
		27,
		{
			{ tr("Drop weapons upon death"), 							1 },
			{ tr("Don't spawn runes"),									2 },
			{ tr("Instantly return flags (ST/CTF)"),					3 },
			{ tr("Don't allow players to switch teams"),				4 },
			{ tr("Players are automatically assigned teams"),			5 },
			{ tr("Double the amount of ammo given"),					6 },
			{ tr("Players slowly lose health over 100% like Quake"),	7 },
			{ tr("Allow BFG freeaiming"),								8 },
			{ tr("Barrels respawn"),									9 },
			{ tr("No respawn protection"),								10 },
			{ tr("All players start with a shotgun"),					11 },
			{ tr("Players respawn where they died (COOP)"),				12 },
			{ tr("Players keep teams after map change"),				13 },
			{ tr("Don't clear frags after each level"),					14 },
			{ tr("Player can't respawn"),								15 },
			{ tr("Lose a frag when killed"),							16 },
			{ tr("Infinite inventory"),									17 },
			{ tr("No rocket jumping"),									19 },
			{ tr("Award damage not kills"),								20 },
			{ tr("Force drawing alpha"),								21 },
			{ tr("All monsters must be killed before exiting"),			22 },
			{ tr("Players can't see the automap"),						23 },
			{ tr("Allies can't be seen on the automap"),				24 },
			{ tr("You can't spy allies"),								25 },
			{ tr("Players can use chase cam"),							26 },
			{ tr("Players can suicide"),								27 },
			{ tr("Players can not use autoaim"),						28 }
		}
	},

	{
		tr("Compat. flags"),
		30,
		{
			{ tr("Use Doom's shortest texture behavior"),						0 },
			{ tr("Don't fix loop index for stair building"),					1 },
			{ tr("Pain elemental is limited to 20 lost souls"),					2 },
			{ tr("Pickups are only heard locally"),								3 },
			{ tr("Infinitly tall actors"),										4 },
			{ tr("Limit actors to only one sound"),								5 },
			{ tr("Enable wallrunning"),											6 },
			{ tr("Dropped items spawn on floor"),								7 },
			{ tr("Special lines block use line"),								8 },
			{ tr("Disable BOOM local door light effect"),						9 },
			{ tr("Raven's scrollers use their original speed"),					10 },
			{ tr("Use sector based sound target code"),							11 },
			{ tr("Limit dehacked MaxHealth to health bonus"),					12 },
			{ tr("Trace ignores lines with the same sector on both sides"),		13 },
			{ tr("Monsters can not move when hanging over a drop off"),			14 },
			{ tr("Scrolling sectors are additive like Boom"),					15 },
			{ tr("Monsters can see semi-invisible players"),					16 },
			{ tr("Limited movement in the air"),								17 },
			{ tr("Allow map01 \"plasma bump\" bug"),							18 },
			{ tr("Allow instant respawn after death"),							19 },
			{ tr("Disable taunting"),											20 },
			{ tr("Use doom2.exe's original sound curve"),						21 },
			{ tr("Use original doom2 intermission music"),						22 },
			{ tr("Disable stealth monsters"),									23 },
			{ tr("Disable crosshair"),											25 },
			{ tr("Force weapon switch"),										26 },
			{ tr("Instantly moving floors are not silent"),						28 },
			{ tr("Sector sounds use original method for sound orgin"),			29 },
			{ tr("Use original Doom heights for clipping against projetiles"),	30 },
			{ tr("Monsters can't be pushed over drop offs"),					31 }
		}
	}
};

const GameMode SkulltagServer::GAME_MODES[NUM_SKULLTAG_GAME_MODES] =
{
	GameMode::COOPERATIVE,
	GameMode(GAMEMODE_SURVIVAL, tr("Survival"), false),
	GameMode(GAMEMODE_INVASION, tr("Invasion"), false),
	GameMode::DEATHMATCH,
	GameMode::TEAM_DEATHMATCH,
	GameMode(GAMEMODE_DUEL, tr("Duel"), false),
	GameMode(GAMEMODE_TERMINATOR, tr("Terminator"), false),
	GameMode(GAMEMODE_LASTMANSTANDING, tr("LMS"), false),
	GameMode(GAMEMODE_TEAMLMS, tr("Team LMS"), true),
	GameMode(GAMEMODE_POSSESSION, tr("Possession"), false),
	GameMode(GAMEMODE_TEAMPOSSESSION, tr("Team Poss"), true),
	GameMode(GAMEMODE_TEAMGAME, tr("Team Game"), true),
	GameMode::CAPTURE_THE_FLAG,
	GameMode(GAMEMODE_ONEFLAGCTF, tr("One Flag CTF"), true),
	GameMode(GAMEMODE_SKULLTAG, tr("Skulltag"), true),
	GameMode(GAMEMODE_DOMINATION, tr("Domination"), true)
};

const GameCVar SkulltagServer::GAME_MODIFIERS[NUM_SKULLTAG_GAME_MODIFIERS] =
{
	GameCVar("Buckshot", "buckshot"),
	GameCVar("Instagib", "instagib")
};

SkulltagServer::SkulltagServer(const QHostAddress &address, unsigned short port) : Server(address, port),
	botSkill(0), buckshot(false), duelLimit(0), fragLimit(0), instagib(false),
	numTeams(2), pointLimit(0), teamDamage(0.0f), winLimit(0), testingServer(false)
{
	teamInfo[0] = TeamInfo(tr("Blue"), QColor(0, 0, 255), 0);
	teamInfo[1] = TeamInfo(tr("Red"), QColor(255, 0, 0), 0);
	teamInfo[2] = TeamInfo(tr("Green"), QColor(0, 255, 0), 0);
	teamInfo[3] = TeamInfo(tr("Gold"), QColor(255, 255, 0), 0);
}

QString SkulltagServer::clientBinary(QString& error) const
{
	SettingsData* setting;
	if (!this->testingServer || !Main::config->setting("SkulltagEnableTesting")->boolean())
	{
		setting = Main::config->setting("SkulltagBinaryPath");

		if (setting->string().isEmpty())
		{
			error = tr("No executable specified for Skulltag");
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
	else
	{
		// This is common code for both Unix and Windows:
		setting = Main::config->setting("SkulltagTestingPath");
		QString path = setting->string();
		if (path.isEmpty())
		{
			error = tr("No testing directory specified for Skulltag");
			return QString();
		}

		if (path[path.length() - 1] != '/' && path[path.length() - 1] != '\\' )
			path += '/';

		path += version();

		QFileInfo fi(path);
		if (!fi.exists())
		{
			error = tr("%1\ndoesn't exist.\nYou need to install new testing binaries.").arg(path);
			QString messageBoxContent = tr("%1\n\nDo you want Doomseeker to create %2 directory and copy all your .ini files from your base directory?\n\nNote: You will still have to manualy install the binaries.").arg(error, version());

			if (QMessageBox::question(Main::mainWindow, tr("Doomseeker - missing testing binaries"), messageBoxContent, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			{
				// setting->string() should still contain base dir
				// for testing binaries
				QDir dir(setting->string());
				if (!dir.mkdir(version()))
				{
					error = tr("Unable to create directory:\n%1").arg(path);
					return QString();
				}

				// Now copy all .ini's. On Linux .ini's are kept in ~/.skulltag so this will
				// do nothing, but on Windows this should work like magic.
				QDir baseBinaryDir(clientWorkingDirectory());
				QStringList nameFilters;
				nameFilters << "*.ini";
				QStringList iniFiles = baseBinaryDir.entryList(nameFilters, QDir::Files);
				foreach(QString str, iniFiles)
				{
					QString sourcePath = baseBinaryDir.canonicalPath() + '/' + str;
					QString targetPath = path + '/' + str;
					QFile file(sourcePath);
					file.copy(targetPath);
				}

				QMessageBox::information(Main::mainWindow, tr("Doomseeker"), tr("Please install now version \"%1\" into:\n%2").arg(version(), path));
				error = QString();
			}
			return QString();
		}

		if (!fi.isDir())
		{
			error = tr("%1\nexists but is NOT a directory.\nCannot proceed.").arg(path);
			return QString();
		}

		QString binPath = path + '/' + ST_BINARY_NAME;
		qDebug() << binPath;
		fi = QFileInfo(binPath);
		if (!fi.exists() || fi.isDir())
		{
			error = tr("%1\ndoesn't contain Skulltag executable").arg(path);
			return QString();
		}

		// Everything checked out, so proceed to create (if necessary) and return path to the script file.
		QString retPath;
		if (!spawnTestingBatchFile(path, retPath, error))
		{
			// error is already specified inside spawnTestingBatchFile()Ŝ
			return QString();
		}

		return retPath;
	}
}

QString SkulltagServer::clientWorkingDirectory() const
{
	SettingsData* setting = Main::config->setting("SkulltagBinaryPath");
	QFileInfo fi(setting->string());
	return fi.canonicalPath();
}

void SkulltagServer::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword) const
{
	Server::connectParameters(args, pf, iwadFound, connectPassword);

	args << Main::config->setting("SkulltagCustomParameters")->string().split(" ", QString::SkipEmptyParts);;

	if(isLocked())
		args << "+cl_password" << connectPassword;
}

void SkulltagServer::hostDMFlags(QStringList& args, const DMFlags& dmFlags) const
{
	const QString argNames[] = { "+dmflags", "+dmflags2", "+compatflags" };
	for (int i = 0; i < qMin(dmFlags.size(), 3); ++i)
	{
		args << argNames[i];
		unsigned val = 0;
		const DMFlagsSection* sec = dmFlags[i];

		for (int i = 0; i < sec->size; ++i)
		{
			val |= 1 << sec->flags[i].value;
		}

		args << QString::number(val);
	}
}

void SkulltagServer::hostProperties(QStringList& args) const
{
	args << "+alwaysapplydmflags" << QString::number(1);
	args << "-host";
	args << "-skill" << QString::number(skill + 1); // from 1 to 5

	QString gameModeStr;
	switch(currentGameMode.modeIndex())
	{
		case GameMode::SGMICooperative:			gameModeStr = "+cooperative"; break;
		case GameMode::SGMICTF:					gameModeStr = "+ctf"; break;
		case GameMode::SGMIDeathmatch:			gameModeStr = "+deathmatch"; break;
		case GameMode::SGMITeamDeathmatch:		gameModeStr = "+teamplay"; break;
		case GAMEMODE_DOMINATION:				gameModeStr = "+domination"; break;
		case GAMEMODE_DUEL:						gameModeStr = "+duel"; break;
		case GAMEMODE_INVASION:					gameModeStr = "+invasion"; break;
		case GAMEMODE_LASTMANSTANDING:			gameModeStr = "+lastmanstanding"; break;
		case GAMEMODE_ONEFLAGCTF:				gameModeStr = "+oneflagctf"; break;
		case GAMEMODE_POSSESSION:				gameModeStr = "+possession"; break;
		case GAMEMODE_SKULLTAG:					gameModeStr = "+skulltag"; break;
		case GAMEMODE_SURVIVAL:					gameModeStr = "+survival"; break;
		case GAMEMODE_TEAMGAME:					gameModeStr = "+teamgame"; break;
		case GAMEMODE_TEAMLMS:					gameModeStr = "+teamlms"; break;
		case GAMEMODE_TEAMPOSSESSION:			gameModeStr = "+teampossession"; break;
		case GAMEMODE_TERMINATOR:				gameModeStr = "+terminator"; break;
	}
	args << gameModeStr << "1";

	args << "+sv_hostemail" << "\"" + email + "\"";

	if (!mapName.isEmpty())
	{
		args << "+map" << mapName;
	}

	if (!mapList.isEmpty())
	{
		foreach (QString s, mapList)
		{
			args << "+addmap" << s;
		}
	}

	args << "+sv_maprotation" << QString::number(static_cast<int>(mapRandomRotation));

	QString md = motd;
	args << "+sv_motd" << "\"" + md.replace("\n", "\\n") + "\"";

	args << "+sv_hostname" << "\"" + name() + "\"";

	args << "+sv_website" << "\"" + webSite + "\"";

	args << "+sv_password" << "\"" + passwordConnect + "\"";
	args << "+sv_forcepassword" << QString::number(static_cast<int>(!passwordConnect.isEmpty()));

	args << "+sv_joinpassword" << "\"" + passwordJoin + "\"";
	args << "+sv_forcejoinpassword" << QString::number(static_cast<int>(!passwordJoin.isEmpty()));

	args << "+sv_rconpassword" << "\"" + passwordRCon + "\"";

	args << "+sv_broadcast" << QString::number(static_cast<int>(broadcastToLAN));
	args << "+sv_updatemaster" << QString::number(static_cast<int>(broadcastToMaster));
	args << "+sv_maxclients" << QString::number(maxClients);
	args << "+sv_maxplayers" << QString::number(maxPlayers);
}

QPixmap SkulltagServer::icon() const
{
	return QPixmap(skulltag_xpm);
}

bool SkulltagServer::readRequest(QByteArray &data)
{
	// Decompress the response.
	const char* in = data.data();
	char packetOut[2000];
	int out = 2000;
	g_Huffman.decode(in, packetOut, data.size(), &out);

	// Check the response code
	int response = READINT32(&packetOut[0]);
	if(response == SERVER_BANNED)
	{
		emit updated(this, RESPONSE_BANNED);
		return false;
	}
	else if(response == SERVER_WAIT)
	{
		emit updated(this, RESPONSE_WAIT);
		return false;
	}
	else if (response != SERVER_GOOD)
	{
		emit updated(this, RESPONSE_BAD);
		return false;
	}

	serverVersion = QString(&packetOut[8]);
	int pos = 8 + serverVersion.length() + 1;

	// now read the data.
	SkulltagGameMode mode = GAMEMODE_COOPERATIVE;
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
		wads.clear(); // clear any previous list we may have had.
		for(int i = 0;i < numPwads;i++)
		{
			QString wad(&packetOut[pos]);
			pos += wad.length() + 1;
			wads << wad;
		}
	}
	if((flags & SQF_GAMETYPE) == SQF_GAMETYPE)
	{
		mode = static_cast<SkulltagGameMode> (READINT8(&packetOut[pos++]));
		currentGameMode = GAME_MODES[mode];
		instagib = READINT8(&packetOut[pos++]) != 0;
		buckshot = READINT8(&packetOut[pos++]) != 0;
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
		locked = (READINT8(&packetOut[pos++]) != 0);
	}
	else
	{
		locked = false;
	}

	if((flags & SQF_FORCEJOINPASSWORD) == SQF_FORCEJOINPASSWORD)
	{
		pos++;
	}

	if((flags & SQF_GAMESKILL) == SQF_GAMESKILL)
		skill = READINT8(&packetOut[pos++]);
	if((flags & SQF_BOTSKILL) == SQF_BOTSKILL)
		botSkill = READINT8(&packetOut[pos++]);

	if((flags & SQF_DMFLAGS) == SQF_DMFLAGS)
	{
		clearDMFlags();
		for (int i = 0; i < NUM_DMFLAG_SECTIONS; ++i)
		{
			unsigned int dmflags = READINT32(&packetOut[pos]);
			pos += 4;

			DMFlagsSection *dmFlagsSec = new DMFlagsSection();
			dmFlagsSec->name = DM_FLAGS[i].name;
			dmFlagsSec->size = 0;

			for (int j = 0; j < DM_FLAGS[i].size; ++j)
			{
				if ( (dmflags & (1 << DM_FLAGS[i].flags[j].value)) != 0)
				{
					dmFlagsSec->flags[dmFlagsSec->size++] = DM_FLAGS[i].flags[j];
				}
			}

			dmFlags << dmFlagsSec;
		}
	}

	if((flags & SQF_LIMITS) == SQF_LIMITS)
	{
		fragLimit = READINT16(&packetOut[pos]);

		// Read timelimit and timeleft,
		// note that if timelimit == 0 then no info
		// about timeleft is sent
		serverTimeLimit = READINT16(&packetOut[pos+2]);
		pos += 4;
		if (serverTimeLimit != 0)
		{
			serverTimeLeft = READINT16(&packetOut[pos]);
			pos += 2;
		}

		duelLimit = READINT16(&packetOut[pos]);
		pointLimit = READINT16(&packetOut[pos+2]);
		winLimit = READINT16(&packetOut[pos+4]);
		switch(mode)
		{
			default:
				serverScoreLimit = fragLimit;
				break;
			case GAMEMODE_LASTMANSTANDING:
			case GAMEMODE_TEAMLMS:
				serverScoreLimit = winLimit;
				break;
			case GAMEMODE_POSSESSION:
			case GAMEMODE_TEAMPOSSESSION:
			case GAMEMODE_TEAMGAME:
			case GAMEMODE_CTF:
			case GAMEMODE_ONEFLAGCTF:
			case GAMEMODE_SKULLTAG:
			case GAMEMODE_DOMINATION:
				serverScoreLimit = pointLimit;
				break;
		}
		pos += 6;
	}
	else
	{
		// Nullify vars if there is no info
		fragLimit = 0;
		serverTimeLimit = 0;
		duelLimit = 0;
		pointLimit = 0;
		winLimit = 0;
		serverScoreLimit = 0;
	}

	if((flags & SQF_TEAMDAMAGE) == SQF_TEAMDAMAGE)
	{
		teamDamage = QByteArray(&packetOut[pos], 4).toFloat();
		pos += 4;
	}
	if((flags & SQF_TEAMSCORES) == SQF_TEAMSCORES)
	{
		// DEPRECATED flag
		for(int i = 0;i < 2;i++)
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
			players.clear(); // Erase previous players (if any)
			for(int i = 0;i < numPlayers;i++)
			{
				// team isn't sent in non team modes.
				bool teammode = currentGameMode.isTeamGame();

				QString name(&packetOut[pos]);
				pos += name.length() + 1;
				int score = READINT16(&packetOut[pos]);
				int ping = READINT16(&packetOut[pos+2]);
				bool spectating = READINT8(&packetOut[pos+4]) != 0;
				bool bot = READINT8(&packetOut[pos+5]) != 0;
				int team = teammode ? READINT8(&packetOut[pos+6]) : Player::TEAM_NONE;
				int time = READINT8(&packetOut[pos+(teammode ? 7 : 6)]);
				pos += teammode ? 8 : 7;

				Player player(name, score, ping, static_cast<Player::PlayerTeam> (team), spectating, bot);
				players << player;
			}
		}
	}
	if((flags & SQF_TEAMINFO_NUMBER) == SQF_TEAMINFO_NUMBER)
		numTeams = READINT8(&packetOut[pos++]);
	if((flags & SQF_TEAMINFO_NAME) == SQF_TEAMINFO_NAME)
	{
		for(int i = 0;i < numTeams && i < ST_MAX_TEAMS;i++)
		{
			teamInfo[i].setName(tr(&packetOut[pos]));
			pos += teamInfo[i].name().length() + 1;
		}
	}
	if((flags & SQF_TEAMINFO_COLOR) == SQF_TEAMINFO_COLOR)
	{
		// NOTE: This may not be correct
		for(int i = 0;i < numTeams && i < ST_MAX_TEAMS;i++)
		{
			teamInfo[i].setColor(QColor(READINT32(&packetOut[pos])));
			pos += 4;
		}
	}
	if((flags & SQF_TEAMINFO_SCORE) == SQF_TEAMINFO_SCORE)
	{
		for(int i = 0;i < numTeams && i < ST_MAX_TEAMS;i++)
		{
			teamInfo[i].setScore(READINT16(&packetOut[pos]));
			if(i < MAX_TEAMS) // Transfer to super class score array if possible.
				scores[i] = teamInfo[i].score();
			pos += 2;
		}
	}

	// Due to a bug in 0.97d3 we need to add additional checks here.
	// 0.97d3 servers also respond with SQF_TESTING_SERVER flag set
	// if it was previously sent to them
	if (pos < out && version().compare("0.97d3") != 0 && (flags & SQF_TESTING_SERVER) == SQF_TESTING_SERVER)
	{
		testingServer = static_cast<bool>(READINT8(&packetOut[pos]));
		++pos;

		// '\0' is read if testingServer == false
		testingArchive = &packetOut[pos];
		pos += testingArchive.length() + 1;
	}
	else
	{
		testingServer = false;
		testingArchive = QString();
	}

	return true;
}

bool SkulltagServer::sendRequest(QByteArray &data)
{
	// Send launcher challenge.
	int query = SQF_STANDARDQUERY;
	const char challenge[12] = {SERVER_CHALLENGE,WRITEINT32_DIRECT(query),0x00,0x00,0x00,0x00};
	char challengeOut[16];
	int out = 16;
	g_Huffman.encode(challenge, challengeOut, 12, &out);
	const QByteArray chall(challengeOut, out);
	data.append(chall);
	return true;
}

QString SkulltagServer::serverBinary(QString& error) const
{
	#ifdef Q_OS_WIN32
		return clientBinary(error);
	#else
		SettingsData* setting = Main::config->setting("SkulltagServerBinaryPath");

		if (setting->string().isEmpty())
		{
			error = tr("No server executable specified for Skulltag");
			return QString();
		}

		QFileInfo fi(setting->string());

		if (!fi.exists() || fi.isDir())
		{
			error = tr("%1\nis a directory or doesn't exist.").arg(setting->string());
			return QString();
		}

		return setting->string();
	#endif
}

bool SkulltagServer::spawnTestingBatchFile(const QString& versionDir, QString& fullPathToFile, QString& error) const
{
	QString binaryPath = versionDir + '/' + ST_BINARY_NAME;
	// This will create an actual path to file, because there is no '/' at the end
	// of scriptFilepath.
	fullPathToFile = versionDir + SCRIPT_FILE_EXTENSION;
	QFileInfo fi(fullPathToFile);
	QFile file(fullPathToFile);
	if (fi.isDir())
	{
		error = tr("%1\n should be a script file but is a directory!").arg(fullPathToFile);
		return false;
	}

	if (fi.exists())
	{
		printf("File Permissions: %X\n", (unsigned int)file.permissions());
		if ((file.permissions() & QFile::ExeUser) == 0)
		{
			error = tr("You don't have permissions to execute file: %1\n").arg(fullPathToFile);
			return false;
		}
		return true;
	}

	QString content;
	#ifdef Q_OS_WIN32
	// Create Windows batch file
	// Extract drive letter:
	QString driveLetter;
	QString workDir = clientWorkingDirectory();
	for (int i = 0; i < workDir.length(); ++i)
	{
		if (workDir[i] == ':')
		{
			driveLetter = workDir.left(i);
		}
	}

	if (!driveLetter.isEmpty())
	{
		content += driveLetter + ":\r\n";
	}

	content += "cd " + clientWorkingDirectory().replace('/', '\\') + "\r\n";
	content += binaryPath.replace('/', '\\') + " %*"; // %* deals with all the parameters
	#else
	// Create Unix script file
	content  = "#!/bin/bash\n";
	content += "cd " + clientWorkingDirectory() + "\n";
	content += "export LANG=C\n"; // without this Skulltag won't run on my system (Zalewa)
	content += binaryPath + " $*"; // $* deals with all the parameters
	#endif

	if (!file.open(QIODevice::WriteOnly))
	{
		error = tr("Couldn't open batch file \"%1\" for writing").arg(fullPathToFile);
		return false;
	}

	if (file.write(content.toAscii()) < 0)
	{
		error = tr("Error while writing batch file \"%1\"").arg(fullPathToFile);
		file.close();
		return false;
	}

	file.close();

	if (!file.setPermissions(file.permissions() | QFile::ExeUser))
	{
		error = tr("Cannot set permissions for file:\n%1").arg(fullPathToFile);
		return false;
	}

	return true;
}

QRgb SkulltagServer::teamColor(int team) const
{
	if(team >= ST_MAX_TEAMS || team < 0)
		return Server::teamColor(team);

	return teamInfo[team].color().rgb();
}

QString	SkulltagServer::teamName(int team) const
{
	if (team == 255)
		return "NO TEAM";

	return team >= 0 && team < ST_MAX_TEAMS ? teamInfo[team].name() : "";
}

////////////////////////////////////////////////////////////////////////////////

SkulltagRConProtocol::SkulltagRConProtocol(Server *server) : RConProtocol(server)
{
}

RConProtocol *SkulltagRConProtocol::connectToServer(Server *server)
{
	SkulltagRConProtocol *protocol = new SkulltagRConProtocol(server);

	const char beginConnection[2] = { CLRC_BEGINCONNECTION, RCON_PROTOCOL_VERSION };
	char encodedConnection[4];
	int encodedSize = 4;
	g_Huffman.encode(beginConnection, encodedConnection, 2, &encodedSize);

	// Try to connect, up to 3 times
	protocol->connected = false;
	for(unsigned int attempts = 0;attempts < 3;attempts++)
	{
		protocol->socket.writeDatagram(encodedConnection, encodedSize, server->address(), server->port());
		if(protocol->socket.waitForReadyRead(3000))
		{
			int size = protocol->socket.pendingDatagramSize();
			char* data = new char[size];
			protocol->socket.readDatagram(data, size);
			char packet[64];
			int decodedSize = 64;
			g_Huffman.decode(data, packet, size, &decodedSize);
			delete[] data;
			switch(packet[0])
			{
				case SVRC_BANNED:
					QMessageBox::critical(NULL, tr("Banned"), tr("You have been banned from this server."));
					break;
				default:
				case SVRC_OLDPROTOCOL:
					QMessageBox::critical(NULL, tr("Incompatible Protocol"), tr("The protocol appears to be outdated."));
					break;
				case SVRC_SALT:
					protocol->connected = true;
					protocol->salt = QString(&packet[1]);
					return protocol;
			}
			break;
		}
	}
	delete protocol;
	return NULL;
}

void SkulltagRConProtocol::disconnectFromServer()
{
	const char disconnectPacket[1] = { CLRC_DISCONNECT };
	char encodedDisconnect[4];
	int encodedSize = 4;
	g_Huffman.encode(disconnectPacket, encodedDisconnect, 1, &encodedSize);
	socket.writeDatagram(encodedDisconnect, encodedSize, server->address(), server->port());
	connected = false;
	emit disconnected();
}

void SkulltagRConProtocol::sendCommand(const QString &cmd)
{
	char packet[4096];
	packet[0] = CLRC_COMMAND;
	packet[cmd.length()+1] = 0;
	memcpy(packet+1, cmd.toAscii().constData(), cmd.length());
	char encodedPacket[4097];
	int encodedSize = 4097;
	g_Huffman.encode(packet, encodedPacket, cmd.length()+2, &encodedSize);
	socket.writeDatagram(encodedPacket, encodedSize, server->address(), server->port());
}

void SkulltagRConProtocol::sendPassword(const QString &password)
{
	// Calculate the MD5 of the salt + password
	// Since the md5 implementation is written in C we need to do some
	// conversions in order to use the functions with our C++ code.
	// Hence the mess.
	QString hashPassword = salt + password;
	MD5_CTX context;
	MD5Init(&context);
	unsigned char tmp[128];
	memcpy(tmp, hashPassword.toAscii().constData(), hashPassword.length());
	MD5Update(&context, tmp, hashPassword.length());
	unsigned char out[16];
	MD5Final(out, &context);
	QString md5;
	for(int i = 0;i < 16;i++)
		md5 += QString("%1").arg(out[i], 2, 16, QChar('0'));

	// Create the packet
	char passwordPacket[34];
	passwordPacket[0] = CLRC_PASSWORD;
	memcpy(passwordPacket+1, md5.toAscii().data(), md5.length());
	passwordPacket[33] = 0;
	char encodedPassword[50];
	int encodedLength = 50;
	g_Huffman.encode(passwordPacket, encodedPassword, 34, &encodedLength);

	for(unsigned int i = 0;i < 3;i++)
	{
		socket.writeDatagram(encodedPassword, encodedLength, server->address(), server->port());

		if(socket.waitForReadyRead(3000))
		{
			int size = socket.pendingDatagramSize();
			char* data = new char[size];
			socket.readDatagram(data, size);
			char packet[4096];
			int decodedSize = 4096;
			g_Huffman.decode(data, packet, size, &decodedSize);
			delete[] data;
			switch(packet[0])
			{
				default:
				case SVRC_INVALIDPASSWORD:
					connected = false;
					QMessageBox::critical(NULL, tr("Invalid Password"), tr("The password you entered appears to be invalid."));
					disconnectFromServer();
					break;
				case SVRC_LOGGEDIN:
					start();
					serverProtocolVersion = packet[1];
					hostName = QString(&packet[2]);
					qDebug() << hostName;
					int numUpdates = packet[hostName.length() + 3];
					int position = 0;
					processPacket(packet + hostName.length() + 4, decodedSize - hostName.length() - 4, true, numUpdates, &position);
					position += hostName.length() + 4;
					int numStrings = packet[position++];
					while(numStrings-- > 0)
					{
						QString message(&packet[position]);
						position += message.length() + 1;
						emit messageReceived(message);
					}
					break;
			}
			break;
		}
	}
}

void SkulltagRConProtocol::run()
{
	while(connected)
	{
		if(socket.waitForReadyRead(4800)) // Try to get 2 packets per second in order to compensate for lag and packet loss
		{
			int size = socket.pendingDatagramSize();
			char* data = new char[size];
			socket.readDatagram(data, size);
			char packet[4096];
			int decodedSize = 4096;
			g_Huffman.decode(data, packet, size, &decodedSize);
			delete[] data;

			processPacket(packet, size);
		}
		else
		{
			// create a "PONG" packet
			const char pong[1] = { CLRC_PONG };
			char encodedPong[4];
			int encodedSize = 4;
			g_Huffman.encode(pong, encodedPong, 1, &encodedSize);
			socket.writeDatagram(encodedPong, encodedSize, server->address(), server->port());
		}
	}
}

void SkulltagRConProtocol::processPacket(const char *data, int length, bool initial, int maxUpdates, int *pos)
{
	if(length <= 0)
		return;

	int position = 0;
	while(position < length && maxUpdates-- != 0)
	{
		// Determine how we get to the update.
		int update = 0;
		if(initial)
			update = SVRC_UPDATE;
		else
			update = data[position++];

		switch(update)
		{
			default:
				qDebug() << "Unknown update (" << static_cast<int> (data[position-1]) << ")";
				if(pos != NULL)
					*pos = position;
				return;
			case SVRC_MESSAGE:
			{
				QString message = QString(&data[position]);
				position += message.length() + 1;
				emit messageReceived(message);
				break;
			}
			case SVRC_UPDATE:
				switch(data[position++])
				{
					default:
						qDebug() << "Uknown streamlined update (" << static_cast<int> (data[position-1]) << ")";
						if(pos != NULL)
							*pos = position;
						return;
					case SVRCU_MAP:
					{
						QString map = QString(&data[position]);
						position += map.length() + 1;
						break;
					}
					case SVRCU_ADMINCOUNT:
					{
						int admins = data[position++];
						break;
					}
					case SVRCU_PLAYERDATA:
					{
						int players = data[position++];
						this->players.clear();
						while(players-- > 0)
						{
							QString player(&data[position]);
							position += player.length() + 1;
							this->players.append(Player(player, 0, 0));
						}
						emit playerListUpdated();
						break;
					}
				}
				break;
		}
	}
	if(pos != NULL)
		*pos = position;
}

RConProtocol *SkulltagServer::rcon()
{
	return SkulltagRConProtocol::connectToServer(this);
}

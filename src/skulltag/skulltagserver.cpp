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

#define SERVER_CHALLENGE	0xC7,0x00,0x00,0x00
#define SERVER_GOOD			5660023
#define SERVER_BANNED		5660025
#define SERVER_WAIT			5660024

TeamInfo::TeamInfo(const QString &name, const QColor &color, unsigned int score) :
	teamName(name), teamColor(color), teamScore(score)
{
}

////////////////////////////////////////////////////////////////////////////////

const QString SkulltagServer::DMFLAGS[96] =
{
	tr("Do not spawn health items (DM)"),
	tr("Do not spawn powerups (DM)"),
	tr("Weapons remain after pickup (DM)"),
	tr("Falling damage (ZDoom/Strife)"),
	tr("Falling damage (Hexen/Strife)"),
	"",
	tr("Stay on same map when someone exits (DM)"),
	tr("Spawn players as far as possible (DM)"),
	tr("Automatically respawn dead players (DM)"),
	tr("Don't spawn armor (DM)"),
	tr("Kill anyone who tries to exit the level (DM)"),
	tr("Infinite ammo"),
	tr("No monsters"),
	tr("Monsters respawn"),
	tr("Items other than invuln. and invis. respawn"),
	tr("Fast monsters"),
	tr("No jumping"),
	tr("No freelook"),
	tr("Respawn invulnerability and invisibility"),
	tr("Arbitrator FOV"),
	tr("No multiplayer weapons in cooperative"),
	tr("No crouching"),
	tr("Lose all old inventory on respawn (COOP)"),
	tr("Lose keys on respawn (COOP)"),
	tr("Lose weapons on respawn (COOP)"),
	tr("Lose armor on respawn (COOP)"),
	tr("Lose powerups on respawn (COOP)"),
	tr("Lose ammo on respawn (COOP)"),
	tr("Lose half your ammo on respawn (COOP)"),
	tr("Jumping allowed"), // Apparently this and the next are 97D compatibility dmflags
	tr("Crouching allowed"),
	"",
	"",
	tr("Drop weapons upon death"),
	tr("Don't spawn runes"),
	tr("Instantly return flags (ST/CTF)"),
	tr("Don't allow players to switch teams"),
	tr("Players are automatically assigned teams"),
	tr("Double the amount of ammo given"),
	tr("Players slowly lose health over 100% like Quake"),
	tr("Allow BFG freeaiming"),
	tr("Barrels respawn"),
	tr("Invulnerability doesn't respawn"),
	tr("All players start with a shotgun"),
	tr("Players respawn where they died (COOP)"),
	tr("Players keep teams after map change"),
	tr("Don't clear frags after each level"),
	tr("Player can't respawn"),
	tr("Lose a frag when killed"),
	tr("Infinite inventory"),
	"",
	"",
	tr("Award damage not kills"),
	tr("Force drawing alpha"),
	tr("All monsters must be killed before exiting"),
	tr("Players can see the automap"),
	tr("Allies can be seen on the automap"),
	tr("You can spy allies"),
	tr("Players can use chase cam"),
	tr("Players can not suicide"),
	tr("Players can not use autoaim"),
	"",
	"",
	"",
	tr("Use Doom's shortest texture behavior"),
	tr("Don't fix loop index for stair building"),
	tr("Pain elemental is limited to 20 lost souls"),
	tr("Pickups are only heard locally"),
	tr("Infinitly tall actors"),
	tr("Limit actors to only one sound"),
	tr("Enable wallrunning"),
	tr("Dropped items spawn on floor"),
	tr("Special lines block use line"),
	tr("Disable BOOM local door light effect"),
	tr("Raven's scrollers use their original speed"),
	tr("Use sector based sound target code"),
	tr("Limit dehacked MaxHealth to health bonus"),
	tr("Trace ignores lines with the same sector on both sides"),
	tr("Monsters can not move when hanging over a drop off"),
	tr("Scrolling sectors are additive like Boom"),
	tr("Monsters can see semi-invisible players"),
	tr("Limited movement in the air"),
	tr("Allow map01 \"plasma bump\" bug"),
	tr("Allow instant respawn after death"),
	tr("Disable taunting"),
	tr("Use doom2.exe's original sound curve"),
	tr("Use original doom2 intermission music"),
	tr("Disable stealth monsters"),
	"",
	tr("Disable crosshair"),
	tr("Force weapon switch"),
	"",
	tr("Instantly moving floors are not silent"),
	tr("Sector sounds use original method for sound orgin"),
	tr("Use original Doom heights for clipping against projetiles"),
	tr("Monsters can't be pushed over drop offs")
};

const GameMode SkulltagServer::GAME_MODES[NUM_SKULLTAG_GAME_MODES] =
{
	GameMode::COOPERATIVE,
	GameMode(tr("Survival"), false),
	GameMode(tr("Invasion"), false),
	GameMode::DEATHMATCH,
	GameMode::TEAM_DEATHMATCH,
	GameMode(tr("Duel"), false),
	GameMode(tr("Terminator"), false),
	GameMode(tr("LMS"), false),
	GameMode(tr("Team LMS"), true),
	GameMode(tr("Possession"), false),
	GameMode(tr("Team Poss"), true),
	GameMode(tr("Team Game"), true),
	GameMode::CAPTURE_THE_FLAG,
	GameMode(tr("One Flag CTF"), true),
	GameMode(tr("Skulltag"), true),
	GameMode(tr("Domination"), true)
};

SkulltagServer::SkulltagServer(const QHostAddress &address, unsigned short port) : Server(address, port),
	botSkill(0), buckshot(false), duelLimit(0), fragLimit(0), instagib(false),
	numTeams(2), pointLimit(0), skill(0), teamDamage(0.0f), winLimit(0)
{
	teamInfo[0] = TeamInfo(tr("Blue"), QColor(0, 0, 255), 0);
	teamInfo[1] = TeamInfo(tr("Red"), QColor(255, 0, 0), 0);
	teamInfo[2] = TeamInfo(tr("Green"), QColor(0, 255, 0), 0);
	teamInfo[3] = TeamInfo(tr("Gold"), QColor(255, 255, 0), 0);
}

void SkulltagServer::additionalServerInfo(QList<ServerInfo>* baseList) const
{
	if (!this->webSite.isEmpty())
	{
		QString url = "<A HREF=\"" + this->webSite + "\">" + this->webSite + "</A>";
		ServerInfo siUrl = { tr("URL: ") + url, "<div style='white-space: pre'>" + this->webSite + "</div>"};
		baseList->append(siUrl);
	}
	if (!this->email.isEmpty())
	{
		QString email = "<A HREF=\"mailto:" + this->email + "\">" + this->email + "</A>";
		ServerInfo siEmail = { tr("E-mail: ") + email, "<div style='white-space: pre'>" + this->email + "</div>"};
		baseList->append(siEmail);
	}
}

QString	SkulltagServer::gameInfoTableHTML() const
{
	const QString timelimit = tr("Timelimit");
	const QString scorelimit = tr("Scorelimit");
	const QString unlimited = tr("Unlimited");
	const QString players = tr("Players");

	// Timelimit
    QString firstTableTimelimit = "<TR><TD>" + timelimit + ":&nbsp;</TD><TD>%1 %2</TD></TR>";
    if (this->timeLimit() == 0)
    {
    	firstTableTimelimit = firstTableTimelimit.arg(unlimited, "");
    }
    else
    {
		QString strLeft = tr("(%1 left)").arg(this->timeLeft());
		firstTableTimelimit = firstTableTimelimit.arg(this->timeLimit()).arg(strLeft);
    }

	// Scorelimit
	QString firstTableScorelimit = "<TR><TD>" + scorelimit + ":&nbsp;</TD><TD>%1</TD></TR>";
	if (this->scoreLimit() == 0)
	{
		firstTableScorelimit = firstTableScorelimit.arg(unlimited);
	}
	else
	{
		firstTableScorelimit = firstTableScorelimit.arg(this->scoreLimit());
	}

	// Team score
	QString firstTableTeamscore;
	if (this->gameMode().isTeamGame())
	{
		firstTableTeamscore = "<TR><TD COLSPAN=2>%1</TD></TR>";
		QString teams;
		bool bPrependBar = false;
		for (int i = 0; i < MAX_TEAMS; ++i)
		{
			if (this->teamPlayerCount(i) != 0)
			{
				if (bPrependBar)
					teams += " | ";
				teams += teamInfo[i].name() + ": " + QString::number(this->score(i));
				bPrependBar = true;
			}
		}
		firstTableTeamscore = firstTableTeamscore.arg(teams);
	}

	// Players
	QString firstTablePlayers = "<TR><TD>" + players + ":&nbsp;</TD><TD>%1 / %2</TD></TR>";
	firstTablePlayers = firstTablePlayers.arg(this->numPlayers()).arg(this->maximumClients());

	QString firstTable = "<TABLE>";
	firstTable += firstTableTimelimit;
	firstTable += firstTableScorelimit;
	firstTable += firstTableTeamscore;
	firstTable += firstTablePlayers;
	firstTable += "</TABLE>";

	return firstTable;
}

QString	SkulltagServer::generalInfoHTML() const
{
	const QString strVersion = tr("Version");
	const QString strEmail = tr ("E-mail");
	const QString strURL = tr ("URL");
	const QString strSkill = tr ("Skill");
	QString ret;

	QString tmp = serverName;
	ret  = tmp.replace('>', "&gt;").replace('<', "&lt;") + "\n";
	ret += strVersion + ": " + version + "\n";

	if (!email.isEmpty())
	{
		ret += strEmail + ": " + email + "\n";
	}

	if (!webSite.isEmpty())
	{
		ret += strURL + ": <A HREF=\"" + webSite + "\">" + webSite + "</A>\n";
	}

	if (skill < SkillLevel::numSkillLevels)
	{
		ret += strSkill + ": " + SkillLevel::names[skill];
	}

	return ret;
}

QString SkulltagServer::playerTableHTML() const
{
	// Sort the players out first.
	QHash<int, QList<const Player*> > sortedPlayers;
	QList<const Player*> botList;
	QList<const Player*> specList;

	for (int i = 0; i < this->numPlayers(); ++i)
	{
		const Player& p = this->player(i);

		if (p.isSpectating())
		{
			specList.append(&p);
			continue;
		}

		if (this->gameMode().isTeamGame())
		{
			int team = p.teamNum();

			QHash<int, QList<const Player*> >::iterator it = sortedPlayers.find(team);
			if (it == sortedPlayers.end())
			{
				QList<const Player*> l;
				l.append(&p);
				sortedPlayers.insert(team, l);
			}
			else
			{
				it.value().append(&p);
			}
		}
		else
		{
			if (p.isBot())
			{
				botList.append(&p);
				continue;
			}

			if (sortedPlayers.count() == 0)
				sortedPlayers.insert(0, QList<const Player*>());

			sortedPlayers.find(0).value().append(&p);
		}

	}

	const QString team = tr("Team");
	const QString player = tr("Player");
	const QString score = tr("Score");
	const QString ping = tr("Ping");
	const QString status = tr("Status");

	QString plTabTeamHeader;
	QString plTabHeader = "<TR>";
	int plTabColNum = 4;
	if (this->gameMode().isTeamGame())
	{
		plTabColNum = 5;
		plTabTeamHeader = "<TD>" + team + "</TD>";
	}
	plTabHeader += plTabTeamHeader + "<TD>" + player + "</TD><TD ALIGN=right>&nbsp;" + score + "</TD><TD ALIGN=right>&nbsp;" + ping + "</TD><TD ALIGN>" + status + "</TD></TR>";
	plTabHeader += QString("<TR><TD COLSPAN=%1><HR WIDTH=100%></TD></TR>").arg(plTabColNum);

	QString plTabPlayers;
	QHash<int, QList<const Player*> >::iterator it;
	bool bAppendEmptyRowAtBeginning = false;
	for (it = sortedPlayers.begin(); it != sortedPlayers.end(); ++it)
	{
		plTabPlayers += spawnPartOfPlayerTable(it.value(), "", plTabColNum, this->gameMode().isTeamGame(), bAppendEmptyRowAtBeginning);
		if (!bAppendEmptyRowAtBeginning)
		{
			bAppendEmptyRowAtBeginning = true;
		}
	}

	bAppendEmptyRowAtBeginning = !plTabPlayers.isEmpty();
	QString plTabBots = spawnPartOfPlayerTable(botList, tr("BOT"), plTabColNum, this->gameMode().isTeamGame(), bAppendEmptyRowAtBeginning);

	bAppendEmptyRowAtBeginning = !(plTabBots.isEmpty() && plTabPlayers.isEmpty());
	QString plTabSpecs = spawnPartOfPlayerTable(specList, tr("SPECTATOR"), plTabColNum, this->gameMode().isTeamGame(), bAppendEmptyRowAtBeginning);


	QString plTab = "<table cellspacing=\"4\" style=\"background-color: #FFFFFF;color: #000000\">";
	plTab += plTabHeader;
	plTab += plTabPlayers;
	plTab += plTabBots;
	plTab += plTabSpecs;
	plTab += "</table>";
	return plTab;
}

QString SkulltagServer::spawnPartOfPlayerTable(QList<const Player*> list, QString status, int colspan, bool isTeamgame, bool bAppendEmptyRowAtBeginning) const
{
	QString ret;
	if (list.count() != 0)
	{
		if (bAppendEmptyRowAtBeginning)
		{
			ret = QString("<TR><TD COLSPAN=%1>&nbsp;</TD></TR>").arg(colspan);
		}

		for (int i = 0; i < list.count(); ++i)
		{
			const Player& p = *list[i];

			QString strPlayer = "<TR>";
			if (isTeamgame)
			{
				if (p.teamNum() < ST_MAX_TEAMS)
				{
					strPlayer += QString("<TD>%1</TD>").arg(teamInfo[p.teamNum()].name());
				}
				else
				{
					strPlayer += QString("<TD></TD>");
				}

			}
			strPlayer += "<TD>%1</TD><TD ALIGN=right>%2</TD><TD ALIGN=right>%3</TD><TD>%4</TD></TR>";
			strPlayer = strPlayer.arg(p.nameFormatted()).arg(p.score()).arg(p.ping());
			strPlayer = strPlayer.arg(status);

			ret += strPlayer;
		}
	}

	return ret;
}

bool SkulltagServer::sendRequest(QByteArray &data)
{
	// Send launcher challenge.
	int query = SQF_STANDARDQUERY;
	const char challenge[12] = {SERVER_CHALLENGE,WRITEINT32_DIRECT(query),0x00,0x00,0x00,0x00};
	char challengeOut[16];
	int out = 16;
	g_Huffman.encode(challenge, challengeOut, 12, &out);
	data.append(challengeOut, out);
	return true;
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

	version = QString(&packetOut[8]);
	int pos = 8 + version.length() + 1;

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
		unsigned int dmflags = READINT32(&packetOut[pos]);
		unsigned int dmflags2 = READINT32(&packetOut[pos+4]);
		unsigned int compatflags = READINT32(&packetOut[pos+8]);
		pos += 12;

		for(int i = 0;i < 96;i++)
		{
			// If the flag is set and we have the description for the flag,
			// add it to the list.
			if(((i < 32 && (dmflags & (1<<i)) != 0) ||
				((i >= 32 && i < 64) && (dmflags2 & (i<<(i-32))) != 0) ||
				(i >= 64 && (compatflags & (1<<(i-64))) != 0))
				&& !DMFLAGS[i].isEmpty())
			{
				dmFlags << DMFLAGS[i];
			}
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
				int team = READINT8(&packetOut[pos+6]);
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

	return true;
}

//------------------------------------------------------------------------------
// skulltaggamerunner.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "main.h"
#include "skulltaggamerunner.h"
#include "skulltaggameinfo.h"
#include "skulltagserver.h"

SkulltagGameRunner::SkulltagGameRunner(const SkulltagServer* server)
: GameRunner(server)
{
}

void SkulltagGameRunner::hostDMFlags(QStringList& args, const DMFlags& dmFlags) const
{
	const QString argNames[] = { "+dmflags", "+dmflags2", "+compatflags" };
	for (int i = 0; i < qMin(dmFlags.size(), 3); ++i)
	{
		unsigned flagsValue = 0;
		const DMFlagsSection* section = dmFlags[i];

		for (int j = 0; j < section->flags.count(); ++j)
		{
			flagsValue |= 1 << section->flags[j].value;
		}

		args << argNames[i] << QString::number(flagsValue);
	}
}

void SkulltagGameRunner::hostProperties(QStringList& args) const
{
	args << "+alwaysapplydmflags" << QString::number(1);
	args << "-skill" << QString::number(server->gameSkill() + 1); // from 1 to 5

	QString gameModeStr;
	switch(server->gameMode().modeIndex())
	{
		case GameMode::SGMICooperative:						gameModeStr = "+cooperative"; break;
		case GameMode::SGMICTF:								gameModeStr = "+ctf"; break;
		case GameMode::SGMIDeathmatch:						gameModeStr = "+deathmatch"; break;
		case GameMode::SGMITeamDeathmatch:					gameModeStr = "+teamplay"; break;
		case SkulltagGameInfo::GAMEMODE_DOMINATION:			gameModeStr = "+domination"; break;
		case SkulltagGameInfo::GAMEMODE_DUEL:				gameModeStr = "+duel"; break;
		case SkulltagGameInfo::GAMEMODE_INVASION:			gameModeStr = "+invasion"; break;
		case SkulltagGameInfo::GAMEMODE_LASTMANSTANDING:	gameModeStr = "+lastmanstanding"; break;
		case SkulltagGameInfo::GAMEMODE_ONEFLAGCTF:			gameModeStr = "+oneflagctf"; break;
		case SkulltagGameInfo::GAMEMODE_POSSESSION:			gameModeStr = "+possession"; break;
		case SkulltagGameInfo::GAMEMODE_SKULLTAG:			gameModeStr = "+skulltag"; break;
		case SkulltagGameInfo::GAMEMODE_SURVIVAL:			gameModeStr = "+survival"; break;
		case SkulltagGameInfo::GAMEMODE_TEAMGAME:			gameModeStr = "+teamgame"; break;
		case SkulltagGameInfo::GAMEMODE_TEAMLMS:			gameModeStr = "+teamlms"; break;
		case SkulltagGameInfo::GAMEMODE_TEAMPOSSESSION:		gameModeStr = "+teampossession"; break;
		case SkulltagGameInfo::GAMEMODE_TERMINATOR:			gameModeStr = "+terminator"; break;
	}
	args << gameModeStr << "1";

	args << "+sv_hostemail" << "\"" + server->eMail() + "\"";

	if (!server->map().isEmpty())
	{
		args << "+map" << server->map();
	}

	const QStringList& mapsList = server->mapsList();
	if (!mapsList.isEmpty())
	{
		foreach (QString map, mapsList)
		{
			args << "+addmap" << map;
		}
	}

	args << "+sv_maprotation" << QString::number(
		static_cast<int>(!mapsList.isEmpty()));
	args << "+sv_randommaprotation" << QString::number(
		static_cast<int>(server->randomMapRotation()) );

	QString motd = server->messageOfTheDay();
	args << "+sv_motd" << "\"" + motd.replace("\n", "\\n") + "\"";

	args << "+sv_hostname" << "\"" + server->name() + "\"";

	args << "+sv_website" << "\"" + server->website() + "\"";

	QString password = server->connectPassword();
	args << "+sv_password" << "\"" + password + "\"";
	args << "+sv_forcepassword" << QString::number(static_cast<int>(!password.isEmpty()));

	password = server->joinPassword();
	args << "+sv_joinpassword" << "\"" + password + "\"";
	args << "+sv_forcejoinpassword" << QString::number(static_cast<int>(!password.isEmpty()));

	password = server->rconPassword();
	args << "+sv_rconpassword" << "\"" + password + "\"";

	args << "+sv_broadcast" << QString::number(static_cast<int>( server->isBroadcastingToLAN() ));
	args << "+sv_updatemaster" << QString::number(static_cast<int>( server->isBroadcastingToMaster() ));
	args << "+sv_maxclients" << QString::number(server->maximumClients());
	args << "+sv_maxplayers" << QString::number(server->maximumPlayers());
}

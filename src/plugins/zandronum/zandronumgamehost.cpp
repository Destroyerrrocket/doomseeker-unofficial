//------------------------------------------------------------------------------
// zandronumgamehost.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "zandronumgamehost.h"
#include "zandronumengineplugin.h"
#include "zandronumgamerunner.h"
#include "zandronumgameinfo.h"
#include "zandronumserver.h"
#include <serverapi/gamecreateparams.h>

ZandronumGameHost::ZandronumGameHost()
: GameHost(ZandronumEnginePlugin::staticInstance())
{
	setArgForServerLaunch("-host");

	set_addDMFlags(&ZandronumGameHost::addDMFlags);
}

void ZandronumGameHost::addDMFlags()
{
	const QList<DMFlagsSection>& dmFlags = params().dmFlags();
	const QString argNames[] = { "+dmflags", "+dmflags2", "+compatflags" };
	for (int i = 0; i < qMin(dmFlags.size(), 3); ++i)
	{
		unsigned flagsValue = 0;
		const DMFlagsSection& section = dmFlags[i];

		for (int j = 0; j < section.count(); ++j)
		{
			flagsValue |= section[i].value();
		}

		args() << argNames[i] << QString::number(flagsValue);
	}
}

void ZandronumGameHost::addExtra()
{
	args() << "+alwaysapplydmflags" << QString::number(1);
	args() << "-skill" << QString::number(params().skill() + 1); // from 1 to 5

	QString gameModeStr;
	switch(params().gameMode().index())
	{
		case GameMode::SGM_Cooperative:						gameModeStr = "+cooperative"; break;
		case GameMode::SGM_CTF:								gameModeStr = "+ctf"; break;
		case GameMode::SGM_Deathmatch:						gameModeStr = "+deathmatch"; break;
		case GameMode::SGM_TeamDeathmatch:					gameModeStr = "+teamplay"; break;
		case ZandronumGameInfo::GAMEMODE_DOMINATION:		gameModeStr = "+domination"; break;
		case ZandronumGameInfo::GAMEMODE_DUEL:				gameModeStr = "+duel"; break;
		case ZandronumGameInfo::GAMEMODE_INVASION:			gameModeStr = "+invasion"; break;
		case ZandronumGameInfo::GAMEMODE_LASTMANSTANDING:	gameModeStr = "+lastmanstanding"; break;
		case ZandronumGameInfo::GAMEMODE_ONEFLAGCTF:		gameModeStr = "+oneflagctf"; break;
		case ZandronumGameInfo::GAMEMODE_POSSESSION:		gameModeStr = "+possession"; break;
		case ZandronumGameInfo::GAMEMODE_SKULLTAG:			gameModeStr = "+skulltag"; break;
		case ZandronumGameInfo::GAMEMODE_SURVIVAL:			gameModeStr = "+survival"; break;
		case ZandronumGameInfo::GAMEMODE_TEAMGAME:			gameModeStr = "+teamgame"; break;
		case ZandronumGameInfo::GAMEMODE_TEAMLMS:			gameModeStr = "+teamlms"; break;
		case ZandronumGameInfo::GAMEMODE_TEAMPOSSESSION:	gameModeStr = "+teampossession"; break;
		case ZandronumGameInfo::GAMEMODE_TERMINATOR:		gameModeStr = "+terminator"; break;
	}
	args() << gameModeStr << "1";

	args() << "+sv_hostemail" << strArg(params().email());

	if (!params().map().isEmpty())
	{
		args() << "+map" << strArg(params().map());
	}

	foreach (const QString& map, params().mapList())
	{
		args() << "+addmap" << strArg(map);
	}

	args() << "+sv_maprotation" << QString::number(
		static_cast<int>(!params().mapList().isEmpty()));
	args() << "+sv_randommaprotation" << QString::number(
		static_cast<int>(params().isRandomMapRotation()) );

	QString motd = params().motd();
	args() << "+sv_motd" << strArg(motd.replace("\n", "\\n"));

	args() << "+sv_hostname" << strArg(params().name());

	args() << "+sv_website" << strArg(params().url());

	{
		QString password = params().connectPassword();
		args() << "+sv_password" << strArg(password);
		args() << "+sv_forcepassword" << QString::number(static_cast<int>(!password.isEmpty()));
	}

	{
		QString password = params().ingamePassword();
		args() << "+sv_joinpassword" << strArg(password);
		args() << "+sv_forcejoinpassword" << QString::number(static_cast<int>(!password.isEmpty()));
	}

	args() << "+sv_rconpassword" << strArg(params().rconPassword());

	args() << "+sv_broadcast" << QString::number(static_cast<int>( params().isBroadcastToLan() ));
	args() << "+sv_updatemaster" << QString::number(static_cast<int>( params().isBroadcastToMaster() ));
	args() << "+sv_maxclients" << QString::number(params().maxTotalClientSlots());
	args() << "+sv_maxplayers" << QString::number(params().maxPlayers());
}

QString ZandronumGameHost::strArg(const QString &val)
{
	if (val.isEmpty())
	{
		// This will reset the value to nothing.
		return "\"\"";
	}
	else
	{
		return val;
	}
}

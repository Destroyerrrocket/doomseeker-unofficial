//------------------------------------------------------------------------------
// odamexgamehost.cpp
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
#include "odamexgamehost.h"

#include <serverapi/gamecreateparams.h>
#include <serverapi/serverstructs.h>
#include "odamexengineplugin.h"

OdamexGameHost::OdamexGameHost()
: GameHost(OdamexEnginePlugin::staticInstance())
{
	setArgForDemoPlayback("-netplay");
	setArgForDemoRecord("-netrecord");
}

void OdamexGameHost::addExtra()
{
	args() << "-skill" << QString::number(params().skill() + 1); // from 1 to 5

	const QStringList& mapsList = params().mapList();
	if (!mapsList.isEmpty())
	{
		foreach (QString map, mapsList)
		{
			args() << "+addmap" << map;
		}
	}
	args() << "+shufflemaplist" << QString::number( static_cast<int>(params().isRandomMapRotation()) );

	unsigned int modeNum;
	switch(params().gameMode().index())
	{
		default:
		case GameMode::SGM_Cooperative: modeNum = 0; break;
		case GameMode::SGM_Deathmatch: modeNum = 1; break;
		case GameMode::SGM_TeamDeathmatch: modeNum = 2; break;
		case GameMode::SGM_CTF: modeNum = 3; break;
	}
	args() << "+sv_gametype" << QString::number(modeNum);

	if (!params().map().isEmpty())
	{
		args() << "+map" << params().map();
	}

	args() << "+join_password" << "\"" + params().ingamePassword() + "\"";
	args() << "+rcon_password" << "\"" + params().rconPassword() + "\"";
	args() << "+sv_email" << "\"" + params().email() + "\"";
	args() << "+sv_hostname" << "\"" + params().name() + "\"";
	args() << "+sv_maxclients" << QString::number(params().maxTotalClientSlots());
	args() << "+sv_maxplayers" << QString::number(params().maxPlayers());
	args() << "+sv_website" << "\"" + params().url() + "\"";

	QString motd = params().motd();
	args() << "+sv_motd" << "\"" + motd.replace("\n", "\\n") + "\"";

	args() << "+sv_usemasters" << QString::number(static_cast<int>( params().isBroadcastToMaster() ));
}

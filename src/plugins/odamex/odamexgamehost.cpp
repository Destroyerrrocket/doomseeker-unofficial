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

#include <serverapi/serverstructs.h>
#include "odamexserver.h"

OdamexGameHost::OdamexGameHost(const OdamexServer* server)
: GameHost(server)
{
	this->server = server;
	setArgForDemoPlayback("-netplay");
	setArgForDemoRecord("-netrecord");
}

void OdamexGameHost::addExtra()
{
	args() << "-skill" << QString::number(server->skill() + 1); // from 1 to 5

	const QStringList& mapsList = server->mapList();
	if (!mapsList.isEmpty())
	{
		foreach (QString map, mapsList)
		{
			args() << "+addmap" << map;
		}
	}
	args() << "+shufflemaplist" << QString::number( static_cast<int>(server->isRandomMapRotation()) );

	unsigned int modeNum;
	switch(server->gameMode().index())
	{
		default:
		case GameMode::SGM_Cooperative: modeNum = 0; break;
		case GameMode::SGM_Deathmatch: modeNum = 1; break;
		case GameMode::SGM_TeamDeathmatch: modeNum = 2; break;
		case GameMode::SGM_CTF: modeNum = 3; break;
	}
	args() << "+sv_gametype" << QString::number(modeNum);

	if (!server->map().isEmpty())
	{
		args() << "+map" << server->map();
	}

	args() << "+join_password" << "\"" + server->joinPassword() + "\"";
	args() << "+rcon_password" << "\"" + server->rconPassword() + "\"";
	args() << "+sv_email" << "\"" + server->email() + "\"";
	args() << "+sv_hostname" << "\"" + server->name() + "\"";
	args() << "+sv_maxclients" << QString::number(server->numTotalSlots());
	args() << "+sv_maxplayers" << QString::number(server->maxPlayers());
	args() << "+sv_website" << "\"" + server->webSite() + "\"";

	QString motd = server->motd();
	args() << "+sv_motd" << "\"" + motd.replace("\n", "\\n") + "\"";

	args() << "+sv_usemasters" << QString::number(static_cast<int>( server->isBroadcastToMaster() ));
}

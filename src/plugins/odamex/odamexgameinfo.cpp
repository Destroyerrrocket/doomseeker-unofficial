//------------------------------------------------------------------------------
// odamexgameinfo.cpp
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
#include "odamexgameinfo.h"

QList<DMFlagsSection> OdamexGameInfo::dmFlagsList;
QList<GameMode> OdamexGameInfo::gameModesList;

OdamexGameInfo* OdamexGameInfo::static_constructor = new OdamexGameInfo();

OdamexGameInfo::OdamexGameInfo()
{
	initDMFlags();
	initGameModes();

	delete static_constructor;
}

void OdamexGameInfo::initDMFlags()
{
	DMFlagsSection section("DMFlags");
	section << DMFlag( tr("Items respawn"),								0 );
	section << DMFlag( tr("Weapons stay"),								1 );
	section << DMFlag( tr("Friendly fire"),								2 );
	section << DMFlag( tr("Allow exit"),								3 );
	section << DMFlag( tr("Infinite ammo"),								4 );
	section << DMFlag( tr("No monsters"),								5 );
	section << DMFlag( tr("Monsters respawn"),							6 );
	section << DMFlag( tr("Fast monsters"),								7 );
	section << DMFlag( tr("Jumping allowed"),							8 );
	section << DMFlag( tr("Freelook allowed"),							9 );
	section << DMFlag( tr("Wad can be downloaded"),						10 );
	section << DMFlag( tr("Server resets on empty"),					11 );
	section << DMFlag( tr("Clean Maps"),								12 );
	section << DMFlag( tr("Kill anyone who tries to leave the level"),	13 );

	dmFlagsList << section;
}

void OdamexGameInfo::initGameModes()
{
	gameModesList << GameMode::mkCooperative();
	gameModesList << GameMode::mkDeathmatch();
	gameModesList << GameMode::mkTeamDeathmatch();
	gameModesList << GameMode::mkCaptureTheFlag();
//	gameModesList << GameMode(MODE_DEATHMATCH2, tr("Deathmatch 2.0"), false);
}

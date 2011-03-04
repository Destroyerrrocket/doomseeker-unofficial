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

DMFlags			OdamexGameInfo::dmFlagsList;
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
	DMFlagsSection* pSection = new DMFlagsSection;
	dmFlagsList << pSection;

	DMFlagsSection& section = *pSection;
	section.name = tr("DMFlags");
	section.flags << DMFlag( tr("Items respawn"),								0 );
	section.flags << DMFlag( tr("Weapons stay"),								1 );
	section.flags << DMFlag( tr("Friendly fire"),								2 );
	section.flags << DMFlag( tr("Allow exit"),									3 );
	section.flags << DMFlag( tr("Infinite ammo"),								4 );
	section.flags << DMFlag( tr("No monsters"),									5 );
	section.flags << DMFlag( tr("Monsters respawn"),							6 );
	section.flags << DMFlag( tr("Fast monsters"),								7 );
	section.flags << DMFlag( tr("Jumping allowed"),								8 );
	section.flags << DMFlag( tr("Freelook allowed"),							9 );
	section.flags << DMFlag( tr("Wad can be downloaded"),						10 );
	section.flags << DMFlag( tr("Server resets on empty"),						11 );
	section.flags << DMFlag( tr("Clean Maps"),									12 );
	section.flags << DMFlag( tr("Kill anyone who tries to leave the level"),	13 );
}

void OdamexGameInfo::initGameModes()
{
	gameModesList << GameMode::COOPERATIVE;
	gameModesList << GameMode::DEATHMATCH;
	gameModesList << GameMode::TEAM_DEATHMATCH;
	gameModesList << GameMode::CAPTURE_THE_FLAG;
//	gameModesList << GameMode(MODE_DEATHMATCH2, tr("Deathmatch 2.0"), false);
}

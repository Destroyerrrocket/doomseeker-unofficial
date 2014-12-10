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

GameMode OdamexGameInfo::gameModeDuel()
{
	return GameMode::ffaGame(MODE_DUEL, tr("Duel"));
}

QList<DMFlagsSection> OdamexGameInfo::dmFlags()
{
	DMFlagsSection section("DMFlags");
	section << DMFlag( tr("Items respawn"), 1 << 0 );
	section << DMFlag( tr("Weapons stay"), 1 << 1 );
	section << DMFlag( tr("Friendly fire"), 1 << 2 );
	section << DMFlag( tr("Allow exit"), 1 << 3 );
	section << DMFlag( tr("Infinite ammo"), 1 << 4 );
	section << DMFlag( tr("No monsters"), 1 << 5 );
	section << DMFlag( tr("Monsters respawn"), 1 << 6 );
	section << DMFlag( tr("Fast monsters"), 1 << 7 );
	section << DMFlag( tr("Jumping allowed"), 1 << 8 );
	section << DMFlag( tr("Freelook allowed"), 1 << 9 );
	section << DMFlag( tr("Wad can be downloaded"), 1 << 10 );
	section << DMFlag( tr("Server resets on empty"), 1 << 11 );
	section << DMFlag( tr("Clean Maps"), 1 << 12 );
	section << DMFlag( tr("Kill anyone who tries to leave the level"), 1 << 13 );

	QList<DMFlagsSection> result;
	result << section;
	return result;
}

QList<GameMode> OdamexGameInfo::gameModes()
{
	QList<GameMode> gameModesList;
	gameModesList << GameMode::mkCooperative();
	gameModesList << GameMode::mkDeathmatch();
	gameModesList << GameMode::mkTeamDeathmatch();
	gameModesList << GameMode::mkCaptureTheFlag();
	return gameModesList;
}

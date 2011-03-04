//------------------------------------------------------------------------------
// zdaemongameinfo.cpp
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#include "zdaemongameinfo.h"

DMFlags			ZDaemonGameInfo::dmFlagsList;
QList<GameMode> ZDaemonGameInfo::gameModesList;

ZDaemonGameInfo* ZDaemonGameInfo::static_constructor = new ZDaemonGameInfo();

ZDaemonGameInfo::ZDaemonGameInfo()
{
	initDMFlags();
	initGameModes();

	delete static_constructor;
}

void ZDaemonGameInfo::initDMFlags()
{
	int dmflagsSections = 2;
	while(dmflagsSections--)
	{
		dmFlagsList << new DMFlagsSection();
	}

	initDMFlags1(*dmFlagsList[0]);
	initDMFlags2(*dmFlagsList[1]);
}

void ZDaemonGameInfo::initDMFlags1(DMFlagsSection& section)
{
	section.name = tr("DMFlags");
	section.flags << DMFlag( tr("Do not spawn health items (DM)"),					0 );
	section.flags << DMFlag( tr("Do not spawn powerups (DM)"), 						1 );
	section.flags << DMFlag( tr("Weapons remain after pickup (DM)"),				2 );
	section.flags << DMFlag( tr("Falling damage (ZDoom)"),							3 );
	section.flags << DMFlag( tr("Falling damage (Hexen)"),		 					4 );
	section.flags << DMFlag( tr("Stay on same map when someone exits (DM)"),		6 );
	section.flags << DMFlag( tr("Spawn players as far as possible (DM)"),			7 );
	section.flags << DMFlag( tr("Automatically respawn dead players (DM)"),			8 );
	section.flags << DMFlag( tr("Don't spawn armor (DM)"),							9 );
	section.flags << DMFlag( tr("Kill anyone who tries to exit the level (DM)"),	10 );
	section.flags << DMFlag( tr("Infinite ammo"),									11 );
	section.flags << DMFlag( tr("No monsters"),										12 );
	section.flags << DMFlag( tr("Monsters respawn"),								13 );
	section.flags << DMFlag( tr("Items other than invuln. and invis. respawn"),		14 );
	section.flags << DMFlag( tr("Fast monsters"),									15 );
	section.flags << DMFlag( tr("No jumping"),										16 );
	section.flags << DMFlag( tr("No freelook"),										17 );
	section.flags << DMFlag( tr("Respawn invulnerability and invisibility"),		18 );
	section.flags << DMFlag( tr("Infinitely tall actors"),							19 );
	section.flags << DMFlag( tr("Respawn mega items"),								21 );
	section.flags << DMFlag( tr("Allow crosshair"),									22 );
}

void ZDaemonGameInfo::initDMFlags2(DMFlagsSection& section)
{
	section.name = tr("DMFlags2");
	section.flags << DMFlag( tr("Drop weapons upon death"), 						1 );
	section.flags << DMFlag( tr("Respawn where died (COOP)"),						2 );
	section.flags << DMFlag( tr("Barrels respawn"),									3 );
	section.flags << DMFlag( tr("Player respawn protection (DM)"),					4 );
	section.flags << DMFlag( tr("Nice weapons (COOP)"),								5 );
	section.flags << DMFlag( tr("Keep keys"),										6 );
	section.flags << DMFlag( tr("2x monster strength"),								7 );
	section.flags << DMFlag( tr("2x monster damage"),								8 );
	section.flags << DMFlag( tr("Use classic pickup sounds"),						9 );
	section.flags << DMFlag( tr("Use original sound limmit"),						10 );
	section.flags << DMFlag( tr("Allow wallrunning"),								11 );
	section.flags << DMFlag( tr("Allow classic sound cutoff"),						12 );
	section.flags << DMFlag( tr("Use original style movement"),						13 );
	section.flags << DMFlag( tr("Hide player's country"),							15 );
	section.flags << DMFlag( tr("Use original weapon switching"),					16 );
	section.flags << DMFlag( tr("Allow target names"),								17 );
	section.flags << DMFlag( tr("Use original Z movement"),							18 );
	section.flags << DMFlag( tr("CTF compatibility mode"),							19 );
	section.flags << DMFlag( tr("Disable team starts"),								20 );
	section.flags << DMFlag( tr("Spawn keys in team modes"),						21 );
	section.flags << DMFlag( tr("Assign team keys"),								22 );
	section.flags << DMFlag( tr("Keys remain in team modes"),						23 );
	section.flags << DMFlag( tr("Allies can't be seen on the automap"),				24 );
	section.flags << DMFlag( tr("You can't spy allies"),							25 );
	section.flags << DMFlag( tr("Players can use chase cam"),						26 );
	section.flags << DMFlag( tr("Players can suicide"),								27 );
	section.flags << DMFlag( tr("Players can not use autoaim"),						28 );
}

void ZDaemonGameInfo::initGameModes()
{
	gameModesList << GameMode::DEATHMATCH;
	gameModesList << GameMode::TEAM_DEATHMATCH;
	gameModesList << GameMode::COOPERATIVE;
	gameModesList << GameMode::CAPTURE_THE_FLAG;
}

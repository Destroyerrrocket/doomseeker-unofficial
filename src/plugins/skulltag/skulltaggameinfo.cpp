//------------------------------------------------------------------------------
// skulltaggameinfo.cpp
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
#include "skulltaggameinfo.h"

DMFlags			SkulltagGameInfo::dmFlagsList;
QList<GameMode> SkulltagGameInfo::gameModesList;
QList<GameCVar>	SkulltagGameInfo::gameModifiersList;

SkulltagGameInfo* SkulltagGameInfo::static_constructor = new SkulltagGameInfo();

SkulltagGameInfo::SkulltagGameInfo()
{
	initDMFlags();
	initGameModes();
	initGameModifiers();

	delete static_constructor;
}

void SkulltagGameInfo::initDMFlags()
{
	int dmflagsSections = 3;
	while (dmflagsSections--)
	{
		dmFlagsList << new DMFlagsSection();
	}

	initDMFlags1(*dmFlagsList[0]);
	initDMFlags2(*dmFlagsList[1]);
	initCompatFlags(*dmFlagsList[2]);
}

void SkulltagGameInfo::initDMFlags1(DMFlagsSection& section)
{
	section.name = tr("DMFlags");
	section.flags << DMFlag( tr("Do not spawn health items (DM)"),					0 );
	section.flags << DMFlag( tr("Do not spawn powerups (DM)"), 						1 );
	section.flags << DMFlag( tr("Weapons remain after pickup (DM)"),				2 );
	section.flags << DMFlag( tr("Falling damage (ZDoom/Strife)"),					3 );
	section.flags << DMFlag( tr("Falling damage (Hexen/Strife)"), 					4 );
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
	section.flags << DMFlag( tr("Arbitrator FOV"),									19 );
	section.flags << DMFlag( tr("No multiplayer weapons in cooperative"),			20 );
	section.flags << DMFlag( tr("No crouching"),									21 );
	section.flags << DMFlag( tr("Lose all old inventory on respawn (COOP)"),		22 );
	section.flags << DMFlag( tr("Lose keys on respawn (COOP)"),						23 );
	section.flags << DMFlag( tr("Lose weapons on respawn (COOP)"),					24 );
	section.flags << DMFlag( tr("Lose armor on respawn (COOP)"),					25 );
	section.flags << DMFlag( tr("Lose powerups on respawn (COOP)"),					26 );
	section.flags << DMFlag( tr("Lose ammo on respawn (COOP)"),						27 );
	section.flags << DMFlag( tr("Lose half your ammo on respawn (COOP)"),			28 );
	section.flags << DMFlag( tr("Jumping allowed"),									29 );
	section.flags << DMFlag( tr("Crouching allowed"),								30 );
}

void SkulltagGameInfo::initDMFlags2(DMFlagsSection& section)
{
	section.name = tr("DMFlags2");
	section.flags << DMFlag( tr("Drop weapons upon death"), 						1 );
	section.flags << DMFlag( tr("Don't spawn runes"),								2 );
	section.flags << DMFlag( tr("Instantly return flags (ST/CTF)"),					3 );
	section.flags << DMFlag( tr("Don't allow players to switch teams"),				4 );
	section.flags << DMFlag( tr("Players are automatically assigned teams"),		5 );
	section.flags << DMFlag( tr("Double the amount of ammo given"),					6 );
	section.flags << DMFlag( tr("Players slowly lose health over 100% like Quake"),	7 );
	section.flags << DMFlag( tr("Allow BFG freeaiming"),							8 );
	section.flags << DMFlag( tr("Barrels respawn"),									9 );
	section.flags << DMFlag( tr("No respawn protection"),							10 );
	section.flags << DMFlag( tr("All players start with a shotgun"),				11 );
	section.flags << DMFlag( tr("Players respawn where they died (COOP)"),			12 );
	section.flags << DMFlag( tr("Players keep teams after map change"),				13 );
	section.flags << DMFlag( tr("Don't clear frags after each level"),				14 );
	section.flags << DMFlag( tr("Player can't respawn"),							15 );
	section.flags << DMFlag( tr("Lose a frag when killed"),							16 );
	section.flags << DMFlag( tr("Infinite inventory"),								17 );
	section.flags << DMFlag( tr("No rocket jumping"),								19 );
	section.flags << DMFlag( tr("Award damage not kills"),							20 );
	section.flags << DMFlag( tr("Force drawing alpha"),								21 );
	section.flags << DMFlag( tr("All monsters must be killed before exiting"),		22 );
	section.flags << DMFlag( tr("Players can't see the automap"),					23 );
	section.flags << DMFlag( tr("Allies can't be seen on the automap"),				24 );
	section.flags << DMFlag( tr("You can't spy allies"),							25 );
	section.flags << DMFlag( tr("Players can use chase cam"),						26 );
	section.flags << DMFlag( tr("Players can suicide"),								27 );
	section.flags << DMFlag( tr("Players can not use autoaim"),						28 );
}

void SkulltagGameInfo::initCompatFlags(DMFlagsSection& section)
{
	section.name = tr("Compat. flags");
	section.flags << DMFlag( tr("Use Doom's shortest texture behavior"),						0 );
	section.flags << DMFlag( tr("Don't fix loop index for stair building"),						1 );
	section.flags << DMFlag( tr("Pain elemental is limited to 20 lost souls"),					2 );
	section.flags << DMFlag( tr("Pickups are only heard locally"),								3 );
	section.flags << DMFlag( tr("Infinitly tall actors"),										4 );
	section.flags << DMFlag( tr("Limit actors to only one sound"),								5 );
	section.flags << DMFlag( tr("Enable wallrunning"),											6 );
	section.flags << DMFlag( tr("Dropped items spawn on floor"),								7 );
	section.flags << DMFlag( tr("Special lines block use line"),								8 );
	section.flags << DMFlag( tr("Disable BOOM local door light effect"),						9 );
	section.flags << DMFlag( tr("Raven's scrollers use their original speed"),					10 );
	section.flags << DMFlag( tr("Use sector based sound target code"),							11 );
	section.flags << DMFlag( tr("Limit dehacked MaxHealth to health bonus"),					12 );
	section.flags << DMFlag( tr("Trace ignores lines with the same sector on both sides"),		13 );
	section.flags << DMFlag( tr("Monsters can not move when hanging over a drop off"),			14 );
	section.flags << DMFlag( tr("Scrolling sectors are additive like Boom"),					15 );
	section.flags << DMFlag( tr("Monsters can see semi-invisible players"),						16 );
	section.flags << DMFlag( tr("Limited movement in the air"),									17 );
	section.flags << DMFlag( tr("Allow map01 \"plasma bump\" bug"),								18 );
	section.flags << DMFlag( tr("Allow instant respawn after death"),							19 );
	section.flags << DMFlag( tr("Disable taunting"),											20 );
	section.flags << DMFlag( tr("Use doom2.exe's original sound curve"),						21 );
	section.flags << DMFlag( tr("Use original doom2 intermission music"),						22 );
	section.flags << DMFlag( tr("Disable stealth monsters"),									23 );
	section.flags << DMFlag( tr("Disable crosshair"),											25 );
	section.flags << DMFlag( tr("Force weapon switch"),											26 );
	section.flags << DMFlag( tr("Instantly moving floors are not silent"),						28 );
	section.flags << DMFlag( tr("Sector sounds use original method for sound orgin"),			29 );
	section.flags << DMFlag( tr("Use original Doom heights for clipping against projetiles"),	30 );
	section.flags << DMFlag( tr("Monsters can't be pushed over drop offs"),						31 );
}

void SkulltagGameInfo::initGameModes()
{
	gameModesList << GameMode::COOPERATIVE;
	gameModesList << GameMode(GAMEMODE_SURVIVAL, tr("Survival"), false);
	gameModesList << GameMode(GAMEMODE_INVASION, tr("Invasion"), false);
	gameModesList << GameMode::DEATHMATCH;
	gameModesList << GameMode::TEAM_DEATHMATCH;
	gameModesList << GameMode(GAMEMODE_DUEL, tr("Duel"), false);
	gameModesList << GameMode(GAMEMODE_TERMINATOR, tr("Terminator"), false);
	gameModesList << GameMode(GAMEMODE_LASTMANSTANDING, tr("LMS"), false);
	gameModesList << GameMode(GAMEMODE_TEAMLMS, tr("Team LMS"), true);
	gameModesList << GameMode(GAMEMODE_POSSESSION, tr("Possession"), false);
	gameModesList << GameMode(GAMEMODE_TEAMPOSSESSION, tr("Team Poss"), true);
	gameModesList << GameMode(GAMEMODE_TEAMGAME, tr("Team Game"), true);
	gameModesList << GameMode::CAPTURE_THE_FLAG;
	gameModesList << GameMode(GAMEMODE_ONEFLAGCTF, tr("One Flag CTF"), true);
	gameModesList << GameMode(GAMEMODE_SKULLTAG, tr("Skulltag"), true);
	gameModesList << GameMode(GAMEMODE_DOMINATION, tr("Domination"), true);
	gameModesList << GameMode::UNKNOWN;
}

void SkulltagGameInfo::initGameModifiers()
{
	gameModifiersList << GameCVar("Buckshot", "buckshot");
	gameModifiersList << GameCVar("Instagib", "instagib");
}

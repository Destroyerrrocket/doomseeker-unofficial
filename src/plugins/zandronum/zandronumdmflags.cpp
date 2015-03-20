//------------------------------------------------------------------------------
// zandronumdmflags.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "zandronumdmflags.h"

#include "serverapi/serverstructs.h"

DMFlagsSection ZandronumDmflags::compatFlags()
{
	DMFlagsSection section("Compat. flags");
	section << DMFlag( tr("Use Doom's shortest texture behavior"), COMPATF_SHORTTEX);
	section << DMFlag( tr("Don't fix loop index for stair building"), COMPATF_STAIRINDEX);
	section << DMFlag( tr("Pain elemental is limited to 20 lost souls"), COMPATF_LIMITPAIN);
	section << DMFlag( tr("Pickups are only heard locally"), COMPATF_SILENTPICKUP);
	section << DMFlag( tr("Infinitly tall actors"), COMPATF_NO_PASSMOBJ);
	section << DMFlag( tr("Limit actors to only one sound"), COMPATF_MAGICSILENCE);
	section << DMFlag( tr("Enable wallrunning"), COMPATF_WALLRUN);
	section << DMFlag( tr("Dropped items spawn on floor"), COMPATF_NOTOSSDROPS);
	section << DMFlag( tr("Special lines block use line"), COMPATF_USEBLOCKING);
	section << DMFlag( tr("Disable BOOM local door light effect"), COMPATF_NODOORLIGHT);
	section << DMFlag( tr("Raven's scrollers use their original speed"), COMPATF_RAVENSCROLL);
	section << DMFlag( tr("Use sector based sound target code"), COMPATF_SOUNDTARGET);
	section << DMFlag( tr("Limit dehacked MaxHealth to health bonus"), COMPATF_DEHHEALTH);
	section << DMFlag( tr("Trace ignores lines with the same sector on both sides"), COMPATF_TRACE);
	section << DMFlag( tr("Monsters can not move when hanging over a drop off"), COMPATF_DROPOFF);
	section << DMFlag( tr("Scrolling sectors are additive like Boom"), COMPATF_BOOMSCROLL);
	section << DMFlag( tr("Monsters can see semi-invisible players"), COMPATF_INVISIBILITY);
	section << DMFlag(tr("Instantly moving floors are not silent"), COMPATF_SILENT_INSTANT_FLOORS);
	section << DMFlag(tr("Sector sounds use original method for sound origin"), COMPATF_SECTORSOUNDS);
	section << DMFlag(tr("Use original Doom heights for clipping against projectiles"), COMPATF_MISSILECLIP);
	section << DMFlag(tr("Monsters can't be pushed over dropoffs"), COMPATF_CROSSDROPOFF);
	section << DMFlag(tr("Any monster which calls BOSSDEATH counts for level specials"), COMPATF_ANYBOSSDEATH);
	section << DMFlag(tr("Minotaur's floor flame is exploded immediately when feet are clipped"), COMPATF_MINOTAUR);
	section << DMFlag(tr("Force original velocity calculations for A_Mushroom in Dehacked mods"), COMPATF_MUSHROOM);
	section << DMFlag(tr("Monsters are affected by friction and pushers/pullers"), COMPATF_MBFMONSTERMOVE);
	section << DMFlag(tr("Crushed monsters are turned into gibs, rather than replaced by gibs"), COMPATF_CORPSEGIBS);
	section << DMFlag(tr("Friendly monsters aren't blocked by monster-blocking lines"), COMPATF_NOBLOCKFRIENDS);
	section << DMFlag(tr("Invert sprite sorting order for sprites of equal distance"), COMPATF_SPRITESORT);
	section << DMFlag(tr("Hitscans use original blockmap and hit check code"), COMPATF_HITSCAN);
	section << DMFlag(tr("Find neighboring light level like like Doom"), COMPATF_LIGHT);
	section << DMFlag(tr("Draw polyobjects the old fashioned way"), COMPATF_POLYOBJ);
	return section;
}

DMFlagsSection ZandronumDmflags::zandronumCompatFlags()
{
	DMFlagsSection section("ZA Compat. flags");
	section << DMFlag(tr("Net scripts are client side"), ZACOMPATF_NETSCRIPTS_ARE_CLIENTSIDE);
	section << DMFlag(tr("Clients send full button info"), ZACOMPATF_CLIENTS_SEND_FULL_BUTTON_INFO);
	section << DMFlag(tr("Players can't use 'land' CCMD"), ZACOMPATF_NO_LAND);
	section << DMFlag(tr("Use Doom's original random number generator"), ZACOMPATF_OLD_RANDOM_GENERATOR);
	section << DMFlag(tr("Spheres have NOGRAVITY flag"), ZACOMPATF_NOGRAVITY_SPHERES);
	section << DMFlag(tr("Don't stop player scripts on disconnect"), ZACOMPATF_DONT_STOP_PLAYER_SCRIPTS_ON_DISCONNECT);
	section << DMFlag(tr("Use horizontal explosion thrust of old ZDoom versions"), ZACOMPATF_OLD_EXPLOSION_THRUST);
	section << DMFlag(tr("Non-SOLID things fall through invisible bridges"), ZACOMPATF_OLD_BRIDGE_DROPS);
	section << DMFlag(tr("Use old ZDoom jump physics"), ZACOMPATF_OLD_ZDOOM_ZMOVEMENT);
	section << DMFlag(tr("Disallow weapon change when in mid raise/lower"), ZACOMPATF_FULL_WEAPON_LOWER);
	section << DMFlag(tr("Use vanilla's autoaim tracer behavior"), ZACOMPATF_AUTOAIM);
	section << DMFlag(tr("West spawns are silent"), ZACOMPATF_SILENT_WEST_SPAWNS);
	section << DMFlag(tr("Limited movement in the air"), ZACOMPATF_LIMITED_AIRMOVEMENT);
	section << DMFlag(tr("Allow map01 \"plasma bump\" bug"), ZACOMPATF_PLASMA_BUMP_BUG);
	section << DMFlag(tr("Allow instant respawn after death"), ZACOMPATF_INSTANTRESPAWN);
	section << DMFlag(tr("Disable taunting"), ZACOMPATF_DISABLETAUNTS);
	section << DMFlag(tr("Use doom2.exe's original sound curve"), ZACOMPATF_ORIGINALSOUNDCURVE);
	section << DMFlag(tr("Use original doom2 intermission music"), ZACOMPATF_OLDINTERMISSION);
	section << DMFlag(tr("Disable stealth monsters"), ZACOMPATF_DISABLESTEALTHMONSTERS);
	section << DMFlag(tr("Radius damage has infinite height"), ZACOMPATF_OLDRADIUSDMG);
	section << DMFlag(tr("Disable crosshair"), ZACOMPATF_NO_CROSSHAIR);
	section << DMFlag(tr("Force weapon switch"), ZACOMPATF_OLD_WEAPON_SWITCH);
	return section;
}

DMFlagsSection ZandronumDmflags::dmflags()
{
	DMFlagsSection section("DMFlags");
	section << DMFlag( tr("Do not spawn health items (DM)"), DF_NO_HEALTH);
	section << DMFlag( tr("Do not spawn powerups (DM)"), DF_NO_ITEMS);
	section << DMFlag( tr("Weapons remain after pickup (DM)"), DF_WEAPONS_STAY);
	section << DMFlag( tr("Falling damage (old ZDoom)"), DF_FORCE_FALLINGZD);
	section << DMFlag( tr("Falling damage (Hexen)"), DF_FORCE_FALLINGHX);
	section << DMFlag( tr("Falling damage (Strife)"), DF_FORCE_FALLINGST);
	section << DMFlag( tr("Stay on same map when someone exits (DM)"), DF_SAME_LEVEL);
	section << DMFlag( tr("Spawn players as far as possible (DM)"), DF_SPAWN_FARTHEST);
	section << DMFlag( tr("Automatically respawn dead players (DM)"), DF_FORCE_RESPAWN);
	section << DMFlag( tr("Don't spawn armor (DM)"), DF_NO_ARMOR);
	section << DMFlag( tr("Kill anyone who tries to exit the level (DM)"), DF_NO_EXIT);
	section << DMFlag( tr("Infinite ammo"), DF_INFINITE_AMMO);
	section << DMFlag( tr("No monsters"), DF_NO_MONSTERS);
	section << DMFlag( tr("Monsters respawn"), DF_MONSTERS_RESPAWN);
	section << DMFlag( tr("Items other than invuln. and invis. respawn"), DF_ITEMS_RESPAWN);
	section << DMFlag( tr("Fast monsters"), DF_FAST_MONSTERS);
	section << DMFlag( tr("No jumping"), DF_NO_JUMP);
	section << DMFlag( tr("No freelook"), DF_NO_FREELOOK);
	section << DMFlag( tr("Respawn invulnerability and invisibility"), DF_RESPAWN_SUPER);
	section << DMFlag( tr("Arbitrator FOV"), DF_NO_FOV);
	section << DMFlag( tr("No multiplayer weapons in cooperative"), DF_NO_COOP_WEAPON_SPAWN);
	section << DMFlag( tr("No crouching"), DF_NO_CROUCH);
	section << DMFlag( tr("Lose all old inventory on respawn (COOP)"), DF_COOP_LOSE_INVENTORY);
	section << DMFlag( tr("Lose keys on respawn (COOP)"), DF_COOP_LOSE_KEYS);
	section << DMFlag( tr("Lose weapons on respawn (COOP)"), DF_COOP_LOSE_WEAPONS);
	section << DMFlag( tr("Lose armor on respawn (COOP)"), DF_COOP_LOSE_ARMOR);
	section << DMFlag( tr("Lose powerups on respawn (COOP)"), DF_COOP_LOSE_POWERUPS);
	section << DMFlag( tr("Lose ammo on respawn (COOP)"), DF_COOP_LOSE_AMMO);
	section << DMFlag( tr("Lose half your ammo on respawn (COOP)"), DF_COOP_HALVE_AMMO);
	// Why do these two exist anyway?
	section << DMFlag( tr("Jumping allowed"), DF_YES_JUMP);
	section << DMFlag( tr("Crouching allowed"), DF_YES_CROUCH);
	return section;
}

DMFlagsSection ZandronumDmflags::dmflags2()
{
	DMFlagsSection section("DMFlags2");
	section << DMFlag( tr("Drop weapons upon death"), DF2_YES_WEAPONDROP);
	section << DMFlag( tr("Don't spawn runes"), DF2_NO_RUNES);
	section << DMFlag( tr("Instantly return flags (ST/CTF)"), DF2_INSTANT_RETURN);
	section << DMFlag( tr("Don't allow players to switch teams"), DF2_NO_TEAM_SWITCH);
	section << DMFlag( tr("Players are automatically assigned teams"), DF2_NO_TEAM_SELECT);
	section << DMFlag( tr("Double the amount of ammo given"), DF2_YES_DOUBLEAMMO);
	section << DMFlag( tr("Players slowly lose health over 100% like Quake"), DF2_YES_DEGENERATION);
	section << DMFlag( tr("Allow BFG freeaiming"), DF2_YES_FREEAIMBFG);
	section << DMFlag( tr("Barrels respawn"), DF2_BARRELS_RESPAWN);
	section << DMFlag( tr("No respawn protection"), DF2_NO_RESPAWN_INVUL);
	section << DMFlag( tr("All players start with a shotgun"), DF2_COOP_SHOTGUNSTART);
	section << DMFlag( tr("Players respawn where they died (COOP)"), DF2_SAME_SPAWN_SPOT);
	section << DMFlag( tr("Don't clear frags after each level"), DF2_YES_KEEPFRAGS);
	section << DMFlag( tr("Player can't respawn"), DF2_NO_RESPAWN);
	section << DMFlag( tr("Lose a frag when killed"), DF2_YES_LOSEFRAG);
	section << DMFlag( tr("Infinite inventory"), DF2_INFINITE_INVENTORY);
	section << DMFlag( tr("All monsters must be killed before exiting"), DF2_KILL_MONSTERS);
	section << DMFlag( tr("Players can't see the automap"), DF2_NO_AUTOMAP);
	section << DMFlag( tr("Allies can't be seen on the automap"), DF2_NO_AUTOMAP_ALLIES);
	section << DMFlag( tr("You can't spy allies"), DF2_DISALLOW_SPYING);
	section << DMFlag( tr("Players can use chase cam"), DF2_CHASECAM);
	section << DMFlag( tr("Players can't suicide"), DF2_NOSUICIDE);
	section << DMFlag( tr("Players can't use autoaim"), DF2_NOAUTOAIM);
	section << DMFlag(tr("Don't check ammo when switching weapons"), DF2_DONTCHECKAMMO);
	section << DMFlag(tr("Kill all monsters spawned by a boss cube when the boss dies"), DF2_KILLBOSSMONST);
	return section;
}

DMFlagsSection ZandronumDmflags::zandronumDmflags()
{
	DMFlagsSection section("Zandronum Flags");
	section << DMFlag(tr("Clients can't identify targets"), ZADF_NO_IDENTIFY_TARGET);
	section << DMFlag(tr("lmsspectatorsettings applied in all game modes"), ZADF_ALWAYS_APPLY_LMS_SPECTATORSETTINGS);
	section << DMFlag(tr("Clients can't draw coop info"), ZADF_NO_COOP_INFO);
	section << DMFlag(tr("Unlagged is disabled"), ZADF_NOUNLAGGED);
	section << DMFlag(tr("Players don't block each other"), ZADF_UNBLOCK_PLAYERS);
	section << DMFlag(tr("Clients don't show medals"), ZADF_NO_MEDALS);
	section << DMFlag(tr("Keys are shared between players"), ZADF_SHARE_KEYS);
	section << DMFlag(tr("Player teams are preserved between maps"), ZADF_YES_KEEP_TEAMS);
	section << DMFlag(tr("Force OpenGL defaults"), ZADF_FORCE_GL_DEFAULTS);
	section << DMFlag(tr("No rocket jumping"), ZADF_NO_ROCKET_JUMPING);
	section << DMFlag(tr("Award damage instead of kills"), ZADF_AWARD_DAMAGE_INSTEAD_KILLS);
	section << DMFlag(tr("Force drawing alpha"), ZADF_FORCE_ALPHA);
	section << DMFlag(tr("Don't spawn multiplayer things"), ZADF_COOP_SP_ACTOR_SPAWN);
	section << DMFlag(tr("Force blood screen brightness on clients to emulate vanilla"), ZADF_MAX_BLOOD_SCALAR);
	section << DMFlag(tr("Teammates don't block each other"), ZADF_UNBLOCK_ALLIES);
	return section;
}

QList<DMFlagsSection> ZandronumDmflags::flags()
{
	QList<DMFlagsSection> result;
	result << dmflags();
	result << dmflags2();
	result << zandronumDmflags();
	result << compatFlags();
	result << zandronumCompatFlags();
	return result;
}

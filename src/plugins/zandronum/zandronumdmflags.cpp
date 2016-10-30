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
	section << DMFlag("Use Doom's shortest texture behavior", COMPATF_SHORTTEX,
		tr("Use Doom's shortest texture behavior"));
	section << DMFlag("Don't fix loop index for stair building", COMPATF_STAIRINDEX,
		tr("Don't fix loop index for stair building"));
	section << DMFlag("Pain elemental is limited to 20 lost souls", COMPATF_LIMITPAIN,
		tr("Pain elemental is limited to 20 lost souls"));
	section << DMFlag("Pickups are only heard locally", COMPATF_SILENTPICKUP,
		tr("Pickups are only heard locally"));
	section << DMFlag("Infinitly tall actors", COMPATF_NO_PASSMOBJ,
		tr("Infinitly tall actors"));
	section << DMFlag("Limit actors to only one sound", COMPATF_MAGICSILENCE,
		tr("Limit actors to only one sound"));
	section << DMFlag("Enable wallrunning", COMPATF_WALLRUN,
		tr("Enable wallrunning"));
	section << DMFlag("Dropped items spawn on floor", COMPATF_NOTOSSDROPS,
		tr("Dropped items spawn on floor"));
	section << DMFlag("Special lines block use line", COMPATF_USEBLOCKING,
		tr("Special lines block use line"));
	section << DMFlag("Disable BOOM local door light effect", COMPATF_NODOORLIGHT,
		tr("Disable BOOM local door light effect"));
	section << DMFlag("Raven's scrollers use their original speed", COMPATF_RAVENSCROLL,
		tr("Raven's scrollers use their original speed"));
	section << DMFlag("Use sector based sound target code", COMPATF_SOUNDTARGET,
		tr("Use sector based sound target code"));
	section << DMFlag("Limit dehacked MaxHealth to health bonus", COMPATF_DEHHEALTH,
		tr("Limit dehacked MaxHealth to health bonus"));
	section << DMFlag("Trace ignores lines with the same sector on both sides", COMPATF_TRACE,
		tr("Trace ignores lines with the same sector on both sides"));
	section << DMFlag("Monsters can not move when hanging over a drop off", COMPATF_DROPOFF,
		tr("Monsters can not move when hanging over a drop off"));
	section << DMFlag("Scrolling sectors are additive like Boom", COMPATF_BOOMSCROLL,
		tr("Scrolling sectors are additive like Boom"));
	section << DMFlag("Monsters can see semi-invisible players", COMPATF_INVISIBILITY,
		tr("Monsters can see semi-invisible players"));
	section << DMFlag("Instantly moving floors are not silent", COMPATF_SILENT_INSTANT_FLOORS,
		tr("Instantly moving floors are not silent"));
	section << DMFlag("Sector sounds use original method for sound origin", COMPATF_SECTORSOUNDS,
		tr("Sector sounds use original method for sound origin"));
	section << DMFlag("Use original Doom heights for clipping against projectiles", COMPATF_MISSILECLIP,
		tr("Use original Doom heights for clipping against projectiles"));
	section << DMFlag("Monsters can't be pushed over dropoffs", COMPATF_CROSSDROPOFF,
		tr("Monsters can't be pushed over dropoffs"));
	section << DMFlag("Any monster which calls BOSSDEATH counts for level specials", COMPATF_ANYBOSSDEATH,
		tr("Any monster which calls BOSSDEATH counts for level specials"));
	section << DMFlag("Minotaur's floor flame is exploded immediately when feet are clipped", COMPATF_MINOTAUR,
		tr("Minotaur's floor flame is exploded immediately when feet are clipped"));
	section << DMFlag("Force original velocity calculations for A_Mushroom in Dehacked mods", COMPATF_MUSHROOM,
		tr("Force original velocity calculations for A_Mushroom in Dehacked mods"));
	section << DMFlag("Monsters are affected by friction and pushers/pullers", COMPATF_MBFMONSTERMOVE,
		tr("Monsters are affected by friction and pushers/pullers"));
	section << DMFlag("Crushed monsters are turned into gibs, rather than replaced by gibs", COMPATF_CORPSEGIBS,
		tr("Crushed monsters are turned into gibs, rather than replaced by gibs"));
	section << DMFlag("Friendly monsters aren't blocked by monster-blocking lines", COMPATF_NOBLOCKFRIENDS,
		tr("Friendly monsters aren't blocked by monster-blocking lines"));
	section << DMFlag("Invert sprite sorting order for sprites of equal distance", COMPATF_SPRITESORT,
		tr("Invert sprite sorting order for sprites of equal distance"));
	section << DMFlag("Hitscans use original blockmap and hit check code", COMPATF_HITSCAN,
		tr("Hitscans use original blockmap and hit check code"));
	section << DMFlag("Find neighboring light level like like Doom", COMPATF_LIGHT,
		tr("Find neighboring light level like like Doom"));
	section << DMFlag("Draw polyobjects the old fashioned way", COMPATF_POLYOBJ,
		tr("Draw polyobjects the old fashioned way"));
	return section;
}

DMFlagsSection ZandronumDmflags::zandronumCompatFlags()
{
	DMFlagsSection section("ZA Compat. flags");
	section << DMFlag("Net scripts are client side", ZACOMPATF_NETSCRIPTS_ARE_CLIENTSIDE,
		tr("Net scripts are client side"));
	section << DMFlag("Clients send full button info", ZACOMPATF_CLIENTS_SEND_FULL_BUTTON_INFO,
		tr("Clients send full button info"));
	section << DMFlag("Players can't use 'land' CCMD", ZACOMPATF_NO_LAND,
		tr("Players can't use 'land' CCMD"));
	section << DMFlag("Use Doom's original random number generator",
		ZACOMPATF_OLD_RANDOM_GENERATOR,
		tr("Use Doom's original random number generator"));
	section << DMFlag("Spheres have NOGRAVITY flag", ZACOMPATF_NOGRAVITY_SPHERES,
		tr("Spheres have NOGRAVITY flag"));
	section << DMFlag("Don't stop player scripts on disconnect",
		ZACOMPATF_DONT_STOP_PLAYER_SCRIPTS_ON_DISCONNECT,
		tr("Don't stop player scripts on disconnect"));
	section << DMFlag("Use horizontal explosion thrust of old ZDoom versions",
		ZACOMPATF_OLD_EXPLOSION_THRUST,
		tr("Use horizontal explosion thrust of old ZDoom versions"));
	section << DMFlag("Non-SOLID things fall through invisible bridges",
		ZACOMPATF_OLD_BRIDGE_DROPS,
		tr("Non-SOLID things fall through invisible bridges"));
	section << DMFlag("Use old ZDoom jump physics", ZACOMPATF_OLD_ZDOOM_ZMOVEMENT,
		tr("Use old ZDoom jump physics"));
	section << DMFlag("Disallow weapon change when in mid raise/lower",
		ZACOMPATF_FULL_WEAPON_LOWER,
		tr("Disallow weapon change when in mid raise/lower"));
	section << DMFlag("Use vanilla's autoaim tracer behavior", ZACOMPATF_AUTOAIM,
		tr("Use vanilla's autoaim tracer behavior"));
	section << DMFlag("West spawns are silent", ZACOMPATF_SILENT_WEST_SPAWNS,
		tr("West spawns are silent"));
	section << DMFlag("Limited movement in the air", ZACOMPATF_LIMITED_AIRMOVEMENT,
		tr("Limited movement in the air"));
	section << DMFlag("Allow map01 \"plasma bump\" bug", ZACOMPATF_PLASMA_BUMP_BUG,
		tr("Allow map01 \"plasma bump\" bug"));
	section << DMFlag("Allow instant respawn after death", ZACOMPATF_INSTANTRESPAWN,
		tr("Allow instant respawn after death"));
	section << DMFlag("Disable taunting", ZACOMPATF_DISABLETAUNTS,
		tr("Disable taunting"));
	section << DMFlag("Use doom2.exe's original sound curve", ZACOMPATF_ORIGINALSOUNDCURVE,
		tr("Use doom2.exe's original sound curve"));
	section << DMFlag("Use original doom2 intermission music", ZACOMPATF_OLDINTERMISSION,
		tr("Use original doom2 intermission music"));
	section << DMFlag("Disable stealth monsters", ZACOMPATF_DISABLESTEALTHMONSTERS,
		tr("Disable stealth monsters"));
	section << DMFlag("Radius damage has infinite height", ZACOMPATF_OLDRADIUSDMG,
		tr("Radius damage has infinite height"));
	section << DMFlag("Disable crosshair", ZACOMPATF_NO_CROSSHAIR,
		tr("Disable crosshair"));
	section << DMFlag("Force weapon switch", ZACOMPATF_OLD_WEAPON_SWITCH,
		tr("Force weapon switch"));
	return section;
}

DMFlagsSection ZandronumDmflags::dmflags()
{
	DMFlagsSection section("DMFlags");
	section << DMFlag("Do not spawn health items (DM)", DF_NO_HEALTH,
		tr("Do not spawn health items (DM)"));
	section << DMFlag("Do not spawn powerups (DM)", DF_NO_ITEMS,
		tr("Do not spawn powerups (DM)"));
	section << DMFlag("Weapons remain after pickup (DM)", DF_WEAPONS_STAY,
		tr("Weapons remain after pickup (DM)"));
	section << DMFlag("Falling damage (old ZDoom)", DF_FORCE_FALLINGZD,
		tr("Falling damage (old ZDoom)"));
	section << DMFlag("Falling damage (Hexen)", DF_FORCE_FALLINGHX,
		tr("Falling damage (Hexen)"));
	section << DMFlag("Falling damage (Strife)", DF_FORCE_FALLINGST,
		tr("Falling damage (Strife)"));
	section << DMFlag("Stay on same map when someone exits (DM)", DF_SAME_LEVEL,
		tr("Stay on same map when someone exits (DM)"));
	section << DMFlag("Spawn players as far as possible (DM)", DF_SPAWN_FARTHEST,
		tr("Spawn players as far as possible (DM)"));
	section << DMFlag("Automatically respawn dead players (DM)", DF_FORCE_RESPAWN,
		tr("Automatically respawn dead players (DM)"));
	section << DMFlag("Don't spawn armor (DM)", DF_NO_ARMOR,
		tr("Don't spawn armor (DM)"));
	section << DMFlag("Kill anyone who tries to exit the level (DM)", DF_NO_EXIT,
		tr("Kill anyone who tries to exit the level (DM)"));
	section << DMFlag("Infinite ammo", DF_INFINITE_AMMO,
		tr("Infinite ammo"));
	section << DMFlag("No monsters", DF_NO_MONSTERS,
		tr("No monsters"));
	section << DMFlag("Monsters respawn", DF_MONSTERS_RESPAWN,
		tr("Monsters respawn"));
	section << DMFlag("Items other than invuln. and invis. respawn", DF_ITEMS_RESPAWN,
		tr("Items other than invuln. and invis. respawn"));
	section << DMFlag("Fast monsters", DF_FAST_MONSTERS,
		tr("Fast monsters"));
	section << DMFlag("No jumping", DF_NO_JUMP,
		tr("No jumping"));
	section << DMFlag("No freelook", DF_NO_FREELOOK,
		tr("No freelook"));
	section << DMFlag("Respawn invulnerability and invisibility", DF_RESPAWN_SUPER,
		tr("Respawn invulnerability and invisibility"));
	section << DMFlag("Arbitrator FOV", DF_NO_FOV,
		tr("Arbitrator FOV"));
	section << DMFlag("No multiplayer weapons in cooperative", DF_NO_COOP_WEAPON_SPAWN,
		tr("No multiplayer weapons in cooperative"));
	section << DMFlag("No crouching", DF_NO_CROUCH,
		tr("No crouching"));
	section << DMFlag("Lose all old inventory on respawn (COOP)", DF_COOP_LOSE_INVENTORY,
		tr("Lose all old inventory on respawn (COOP)"));
	section << DMFlag("Lose keys on respawn (COOP)", DF_COOP_LOSE_KEYS,
		tr("Lose keys on respawn (COOP)"));
	section << DMFlag("Lose weapons on respawn (COOP)", DF_COOP_LOSE_WEAPONS,
		tr("Lose weapons on respawn (COOP)"));
	section << DMFlag("Lose armor on respawn (COOP)", DF_COOP_LOSE_ARMOR,
		tr("Lose armor on respawn (COOP)"));
	section << DMFlag("Lose powerups on respawn (COOP)", DF_COOP_LOSE_POWERUPS,
		tr("Lose powerups on respawn (COOP)"));
	section << DMFlag("Lose ammo on respawn (COOP)", DF_COOP_LOSE_AMMO,
		tr("Lose ammo on respawn (COOP)"));
	section << DMFlag("Lose half your ammo on respawn (COOP)", DF_COOP_HALVE_AMMO,
		tr("Lose half your ammo on respawn (COOP)"));
	// Why do these two exist anyway?
	section << DMFlag("Jumping allowed", DF_YES_JUMP,
		tr("Jumping allowed"));
	section << DMFlag("Crouching allowed", DF_YES_CROUCH,
		tr("Crouching allowed"));
	return section;
}

DMFlagsSection ZandronumDmflags::dmflags2()
{
	DMFlagsSection section("DMFlags2");
	section << DMFlag("Drop weapons upon death", DF2_YES_WEAPONDROP,
		tr("Drop weapons upon death"));
	section << DMFlag("Don't spawn runes", DF2_NO_RUNES,
		tr("Don't spawn runes"));
	section << DMFlag("Instantly return flags (ST/CTF)", DF2_INSTANT_RETURN,
		tr("Instantly return flags (ST/CTF)"));
	section << DMFlag("Don't allow players to switch teams", DF2_NO_TEAM_SWITCH,
		tr("Don't allow players to switch teams"));
	section << DMFlag("Players are automatically assigned teams", DF2_NO_TEAM_SELECT,
		tr("Players are automatically assigned teams"));
	section << DMFlag("Double the amount of ammo given", DF2_YES_DOUBLEAMMO,
		tr("Double the amount of ammo given"));
	section << DMFlag("Players slowly lose health over 100% like Quake", DF2_YES_DEGENERATION,
		tr("Players slowly lose health over 100% like Quake"));
	section << DMFlag("Allow BFG freeaiming", DF2_YES_FREEAIMBFG,
		tr("Allow BFG freeaiming"));
	section << DMFlag("Barrels respawn", DF2_BARRELS_RESPAWN,
		tr("Barrels respawn"));
	section << DMFlag("No respawn protection", DF2_NO_RESPAWN_INVUL,
		tr("No respawn protection"));
	section << DMFlag("All players start with a shotgun", DF2_COOP_SHOTGUNSTART,
		tr("All players start with a shotgun"));
	section << DMFlag("Players respawn where they died (COOP)", DF2_SAME_SPAWN_SPOT,
		tr("Players respawn where they died (COOP)"));
	section << DMFlag("Don't clear frags after each level", DF2_YES_KEEPFRAGS,
		tr("Don't clear frags after each level"));
	section << DMFlag("Player can't respawn", DF2_NO_RESPAWN,
		tr("Player can't respawn"));
	section << DMFlag("Lose a frag when killed", DF2_YES_LOSEFRAG,
		tr("Lose a frag when killed"));
	section << DMFlag("Infinite inventory", DF2_INFINITE_INVENTORY,
		tr("Infinite inventory"));
	section << DMFlag("All monsters must be killed before exiting", DF2_KILL_MONSTERS,
		tr("All monsters must be killed before exiting"));
	section << DMFlag("Players can't see the automap", DF2_NO_AUTOMAP,
		tr("Players can't see the automap"));
	section << DMFlag("Allies can't be seen on the automap", DF2_NO_AUTOMAP_ALLIES,
		tr("Allies can't be seen on the automap"));
	section << DMFlag("You can't spy allies", DF2_DISALLOW_SPYING,
		tr("You can't spy allies"));
	section << DMFlag("Players can use chase cam", DF2_CHASECAM,
		tr("Players can use chase cam"));
	section << DMFlag("Players can't suicide", DF2_NOSUICIDE,
		tr("Players can't suicide"));
	section << DMFlag("Players can't use autoaim", DF2_NOAUTOAIM,
		tr("Players can't use autoaim"));
	section << DMFlag("Don't check ammo when switching weapons", DF2_DONTCHECKAMMO,
		tr("Don't check ammo when switching weapons"));
	section << DMFlag("Kill all monsters spawned by a boss cube when the boss dies", DF2_KILLBOSSMONST,
		tr("Kill all monsters spawned by a boss cube when the boss dies"));
	return section;
}

DMFlagsSection ZandronumDmflags::zandronumDmflags()
{
	DMFlagsSection section("Zandronum Flags");
	section << DMFlag("Clients can't identify targets", ZADF_NO_IDENTIFY_TARGET,
		tr("Clients can't identify targets"));
	section << DMFlag("lmsspectatorsettings applied in all game modes",
		ZADF_ALWAYS_APPLY_LMS_SPECTATORSETTINGS,
		tr("lmsspectatorsettings applied in all game modes"));
	section << DMFlag("Clients can't draw coop info", ZADF_NO_COOP_INFO,
		tr("Clients can't draw coop info"));
	section << DMFlag("Unlagged is disabled", ZADF_NOUNLAGGED,
		tr("Unlagged is disabled"));
	section << DMFlag("Players don't block each other", ZADF_UNBLOCK_PLAYERS,
		tr("Players don't block each other"));
	section << DMFlag("Clients don't show medals", ZADF_NO_MEDALS,
		tr("Clients don't show medals"));
	section << DMFlag("Keys are shared between players", ZADF_SHARE_KEYS,
		tr("Keys are shared between players"));
	section << DMFlag("Player teams are preserved between maps", ZADF_YES_KEEP_TEAMS,
		tr("Player teams are preserved between maps"));
	section << DMFlag("Force OpenGL defaults", ZADF_FORCE_GL_DEFAULTS,
		tr("Force OpenGL defaults"));
	section << DMFlag("No rocket jumping", ZADF_NO_ROCKET_JUMPING,
		tr("No rocket jumping"));
	section << DMFlag("Award damage instead of kills", ZADF_AWARD_DAMAGE_INSTEAD_KILLS,
		tr("Award damage instead of kills"));
	section << DMFlag("Force drawing alpha", ZADF_FORCE_ALPHA,
		tr("Force drawing alpha"));
	section << DMFlag("Don't spawn multiplayer things", ZADF_COOP_SP_ACTOR_SPAWN,
		tr("Don't spawn multiplayer things"));
	section << DMFlag("Force blood screen brightness on clients to emulate vanilla",
		ZADF_MAX_BLOOD_SCALAR,
		tr("Force blood screen brightness on clients to emulate vanilla"));
	section << DMFlag("Teammates don't block each other", ZADF_UNBLOCK_ALLIES,
		tr("Teammates don't block each other"));
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

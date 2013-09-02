//------------------------------------------------------------------------------
// zandronumgameinfo.h
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef DOOMSEEKER_PLUGIN_ZANDRONUMGAMEINFO_H
#define DOOMSEEKER_PLUGIN_ZANDRONUMGAMEINFO_H

#include "serverapi/serverstructs.h"
#include <QObject>

class ZandronumGameInfo : public QObject
{
	public:
		// DMFlag definitions as defined in "doomdef.h" of Zandronum's
		// source code.

		// [RH] dmflags bits (based on Q2's)
		// [RC] NOTE: If adding a flag, be sure to add a stub in serverconsole_dmflags.cpp.
		enum
		{
			DF_NO_HEALTH			= 1 << 0,	// Do not spawn health items (DM)
			DF_NO_ITEMS				= 1 << 1,	// Do not spawn powerups (DM) - [RC] Currently not implemented (no easy way to find if it's an object, like AArtifact).
			DF_WEAPONS_STAY			= 1 << 2,	// Leave weapons around after pickup (DM)
			DF_FORCE_FALLINGZD		= 1 << 3,	// Falling too far hurts (old ZDoom style)
			DF_FORCE_FALLINGHX		= 2 << 3,	// Falling too far hurts (Hexen style)
			DF_FORCE_FALLINGST		= 3 << 3,	// Falling too far hurts (Strife style)
		//							  1 << 5	-- this space left blank --
			DF_SAME_LEVEL			= 1 << 6,	// Stay on the same map when someone exits (DM)
			DF_SPAWN_FARTHEST		= 1 << 7,	// Spawn players as far as possible from other players (DM)
			DF_FORCE_RESPAWN		= 1 << 8,	// Automatically respawn dead players after respawn_time is up (DM)
			DF_NO_ARMOR				= 1 << 9,	// Do not spawn armor (DM)
			DF_NO_EXIT				= 1 << 10,	// Kill anyone who tries to exit the level (DM)
			DF_INFINITE_AMMO		= 1 << 11,	// Don't use up ammo when firing
			DF_NO_MONSTERS			= 1 << 12,	// Don't spawn monsters (replaces -nomonsters parm)
			DF_MONSTERS_RESPAWN		= 1 << 13,	// Monsters respawn sometime after their death (replaces -respawn parm)
			DF_ITEMS_RESPAWN		= 1 << 14,	// Items other than invuln. and invis. respawn
			DF_FAST_MONSTERS		= 1 << 15,	// Monsters are fast (replaces -fast parm)
			DF_NO_JUMP				= 1 << 16,	// Don't allow jumping
			// [BB] I don't want to change the dmflag numbers compared to 97D.
			DF_YES_JUMP				= 1 << 29,
			DF_NO_FREELOOK			= 1 << 17,	// Don't allow freelook
			DF_RESPAWN_SUPER		= 1 << 18,	// Respawn invulnerability and invisibility
			DF_NO_FOV				= 1 << 19,	// Only let the arbitrator set FOV (for all players)
			DF_NO_COOP_WEAPON_SPAWN	= 1 << 20,	// Don't spawn multiplayer weapons in coop games
			DF_NO_CROUCH			= 1 << 21,	// Don't allow crouching
			// [BB] I don't want to change the dmflag numbers compared to 97D.
			DF_YES_CROUCH			= 1 << 30,	//
			DF_COOP_LOSE_INVENTORY	= 1 << 22,	// Lose all your old inventory when respawning in coop
			DF_COOP_LOSE_KEYS		= 1 << 23,	// Lose keys when respawning in coop
			DF_COOP_LOSE_WEAPONS	= 1 << 24,	// Lose weapons when respawning in coop
			DF_COOP_LOSE_ARMOR		= 1 << 25,	// Lose armor when respawning in coop
			DF_COOP_LOSE_POWERUPS	= 1 << 26,	// Lose powerups when respawning in coop
			DF_COOP_LOSE_AMMO		= 1 << 27,	// Lose ammo when respawning in coop
			DF_COOP_HALVE_AMMO		= 1 << 28,	// Lose half your ammo when respawning in coop (but not less than the normal starting amount)
		};

		// [BC] More dmflags. w00p!
		// [RC] NOTE: If adding a flag, be sure to add a stub in serverconsole_dmflags.cpp.
		enum
		{
		//	DF2_YES_IMPALING		= 1 << 0,	// Player gets implaed on MF2_IMPALE items
			DF2_YES_WEAPONDROP		= 1 << 1,	// Drop current weapon upon death
			DF2_NO_RUNES			= 1 << 2,	// Don't spawn runes
			DF2_INSTANT_RETURN		= 1 << 3,	// Instantly return flags and skulls when player carrying it dies (ST/CTF)
			DF2_NO_TEAM_SWITCH		= 1 << 4,	// Do not allow players to switch teams in teamgames
			DF2_NO_TEAM_SELECT		= 1 << 5,	// Player is automatically placed on a team.
			DF2_YES_DOUBLEAMMO		= 1 << 6,	// Double amount of ammo that items give you like skill 1 and 5 do
			DF2_YES_DEGENERATION	= 1 << 7,	// Player slowly loses health when over 100% (Quake-style)
			DF2_YES_FREEAIMBFG		= 1 << 8,	// Allow BFG freeaiming in multiplayer games.
			DF2_BARRELS_RESPAWN		= 1 << 9,	// Barrels respawn (duh)
			DF2_NO_RESPAWN_INVUL	= 1 << 10,	// No respawn invulnerability.
			DF2_COOP_SHOTGUNSTART	= 1 << 11,	// All playres start with a shotgun when they respawn
			DF2_SAME_SPAWN_SPOT		= 1 << 12,	// Players respawn in the same place they died (co-op)

			// [BB] I don't want to change the dmflag numbers compared to 97D.
			DF2_YES_KEEP_TEAMS		= 1 << 13,	// Player keeps his team after a map change.

			DF2_YES_KEEPFRAGS		= 1 << 14,	// Don't clear frags after each level
			DF2_NO_RESPAWN			= 1 << 15,	// Player cannot respawn
			DF2_YES_LOSEFRAG		= 1 << 16,	// Lose a frag when killed. More incentive to try to not get yerself killed
			DF2_INFINITE_INVENTORY	= 1 << 17,	// Infinite inventory.
			DF2_KILL_MONSTERS		= 1 << 22,	// All monsters must be killed before the level exits.
			DF2_NO_AUTOMAP			= 1 << 23,	// Players are allowed to see the automap.
			DF2_NO_AUTOMAP_ALLIES	= 1 << 24,	// Allies can been seen on the automap.
			DF2_DISALLOW_SPYING		= 1 << 25,	// You can spy on your allies.
			DF2_CHASECAM			= 1 << 26,	// Players can use the chasecam cheat.
			DF2_NOSUICIDE			= 1 << 27,	// Players are allowed to suicide.
			DF2_NOAUTOAIM			= 1 << 28,	// Players cannot use autoaim.

			// [BB] Enforces some Gl rendering options to their default values.
			DF2_FORCE_GL_DEFAULTS		= 1 << 18,

			// [BB] P_RadiusAttack doesn't give players any z-momentum if the attack was made by a player. This essentially disables rocket jumping.
			DF2_NO_ROCKET_JUMPING		= 1 << 19,

			// [BB] Award actual damage dealt instead of kills.
			DF2_AWARD_DAMAGE_INSTEAD_KILLS		= 1 << 20,

			// [BB] Enforces clients to display alpha, i.e. render as if r_drawtrans == 1.
			DF2_FORCE_ALPHA		= 1 << 21,

			// [BB] Spawn map actors in coop as if the game was single player.
			DF2_COOP_SP_ACTOR_SPAWN		= 1 << 29,
		};

		// [BB] Even more dmflags...
		enum
		{
			// [BB] Enforces clients not to identify players, i.e. behave as if cl_identifytarget == 0.
			DF3_NO_IDENTIFY_TARGET		= 1 << 0,

			// [BB] Apply lmsspectatorsettings in all game modes.
			DF3_ALWAYS_APPLY_LMS_SPECTATORSETTINGS		= 1 << 1,

			// [BB] Enforces clients not to draw coop info, i.e. behave as if cl_drawcoopinfo == 0.
			DF3_NO_COOP_INFO		= 1 << 2,

			// [Spleen] Don't use ping-based backwards reconciliation for player-fired hitscans and rails.
			DF3_NOUNLAGGED			= 1 << 3,

			// [BB] Handle player bodies as if they had MF6_THRUSPECIES.
			DF3_UNBLOCK_PLAYERS			= 1 << 4,

			// [BB] Enforces clients not to show medals, i.e. behave as if cl_medals == 0.
			DF3_NO_MEDALS			= 1 << 5,
		};

		// [RH] Compatibility flags.
		// [RC] NOTE: If adding a flag, be sure to add a stub in serverconsole_dmflags.cpp.
		enum
		{
			COMPATF_SHORTTEX		= 1 << 0,	// Use Doom's shortest texture around behavior?
			COMPATF_STAIRINDEX		= 1 << 1,	// Don't fix loop index for stair building?
			COMPATF_LIMITPAIN		= 1 << 2,	// Pain elemental is limited to 20 lost souls?
			COMPATF_SILENTPICKUP	= 1 << 3,	// Pickups are only heard locally?
			COMPATF_NO_PASSMOBJ		= 1 << 4,	// Pretend every actor is infinitely tall?
			COMPATF_MAGICSILENCE	= 1 << 5,	// Limit actors to one sound at a time?
			COMPATF_WALLRUN			= 1 << 6,	// Enable buggier wall clipping so players can wallrun?
			COMPATF_NOTOSSDROPS		= 1 << 7,	// Spawn dropped items directly on the floor?
			COMPATF_USEBLOCKING		= 1 << 8,	// Any special line can block a use line
			COMPATF_NODOORLIGHT		= 1 << 9,	// Don't do the BOOM local door light effect
			COMPATF_RAVENSCROLL		= 1 << 10,	// Raven's scrollers use their original carrying speed
			COMPATF_SOUNDTARGET		= 1 << 11,	// Use sector based sound target code.
			COMPATF_DEHHEALTH		= 1 << 12,	// Limit deh.MaxHealth to the health bonus (as in Doom2.exe)
			COMPATF_TRACE			= 1 << 13,	// Trace ignores lines with the same sector on both sides
			COMPATF_DROPOFF			= 1 << 14,	// Monsters cannot move when hanging over a dropoff
			COMPATF_BOOMSCROLL		= 1 << 15,	// Scrolling sectors are additive like in Boom
			COMPATF_INVISIBILITY	= 1 << 16,	// Monsters can see semi-invisible players
			// [BB] Changed from 1 << 17 to 1<<27.
			COMPATF_SILENT_INSTANT_FLOORS = 1<<27,	// Instantly moving floors are not silent
			// [BB] Changed from 1 << 18 to 1<<28.
			COMPATF_SECTORSOUNDS	= 1 << 28,	// Sector sounds use original method for sound origin.
			// [BB] Changed from 1 << 19 to 1<<29.
			COMPATF_MISSILECLIP		= 1 << 29,	// Use original Doom heights for clipping against projectiles
			// [BB] Changed from 1 << 20 to 1<<30.
			COMPATF_CROSSDROPOFF	= 1 << 30,	// monsters can't be pushed over dropoffs

			// [BC] Start of new compatflags.

			// Limited movement in the air.
			COMPATF_LIMITED_AIRMOVEMENT	= 1 << 17,

			// Allow the map01 "plasma bump" bug.
			COMPATF_PLASMA_BUMP_BUG	= 1 << 18,

			// Allow instant respawn after death.
			COMPATF_INSTANTRESPAWN	= 1 << 19,

			// Taunting is disabled.
			COMPATF_DISABLETAUNTS	= 1 << 20,

			// Use doom2.exe's original sound curve.
			COMPATF_ORIGINALSOUNDCURVE	= 1 << 21,

			// Use doom2.exe's original intermission screens/music.
			COMPATF_OLDINTERMISSION		= 1 << 22,

			// Disable stealth monsters, since doom2.exe didn't have them.
			COMPATF_DISABLESTEALTHMONSTERS		= 1 << 23,

			// [BB] Always use the old radius damage code (infinite height)
			COMPATF_OLDRADIUSDMG		= 1 << 24,

			// Disable cooperative backpacks.
			// [BB] We are running out of numbers, 1 << 24 is now used for COMPATF_OLDRADIUSDMG
		//	COMPATF_DISABLECOOPERATIVEBACKPACKS	= 1 << 24,

			// [BB] Clients are not allowed to use a crosshair.
			COMPATF_NO_CROSSHAIR		= 1 << 25,

			// [BB] Clients use the vanilla Doom weapon on pickup behavior.
			COMPATF_OLD_WEAPON_SWITCH		= 1 << 26,
		};

		// [BB] More compatibility flags.
		enum
		{
			// [BB] Treat ACS scripts with the SCRIPTF_Net flag to be client side, i.e.
			// executed on the clients, but not on the server.
			COMPATF2_NETSCRIPTS_ARE_CLIENTSIDE		= 1 << 0,
			// [BB] Clients send ucmd.buttons as "long" instead of as "byte" in CLIENTCOMMANDS_ClientMove.
			// So far this is only necessary if the ACS function GetPlayerInput is used in a server side
			// script to check for buttons bigger than BT_ZOOM. Otherwise this information is completely
			// useless for the server and the additional net traffic to send it should be avoided.
			COMPATF2_CLIENTS_SEND_FULL_BUTTON_INFO		= 1 << 1,
			// [BB] Players are not allowed to use the land CCMD. Because of Zandronum's default amount
			// of air control, flying players can get a huge speed boast with the land CCMD. Disallowing
			// players to land, allows to keep the default air control most people are used to while not
			// giving flying players too much of an advantage.
			COMPATF2_NO_LAND						= 1 << 2,
			// [BB] Use Doom's random table instead of ZDoom's random number generator.
			COMPATF2_OLD_RANDOM_GENERATOR		= 1 << 3,
			// [BB] Add NOGRAVITY to actors named InvulnerabilitySphere, Soulsphere, Megasphere and BlurSphere
			// when spawned by the map.
			COMPATF2_NOGRAVITY_SPHERES		= 1 << 4,
			// [BB] When a player leaves the game, don't stop any scripts of that player that are still running.
			COMPATF2_DONT_STOP_PLAYER_SCRIPTS_ON_DISCONNECT		= 1 << 5,
			// If this is enabled, explosions cause a strong horizontal thrust like in old ZDoom versions.
			COMPATF2_EXPLOSION_THRUST = 1 << 6,
			// If this is enabled, non-SOLID things like flags fall through bridges 
			// (as they used to do in old ZDoom versions).
			COMPATF2_BRIDGE_DROPS = 1 << 7,
			// [CK] Uses old ZDoom jump physics, it's a minor bug in the gravity code that causes gravity application in the wrong place
			COMPATF2_ZDOOM_123B33_JUMP_PHYSICS = 1 << 8,
			// [CK] You can't change weapons mid raise/lower in vanilla
			COMPATF2_FULL_WEAPON_LOWER = 1 << 9,
		};

		enum LMSAllowedWeapons
		{
			LMSAW_Pistol = 1 << 0,
			LMSAW_Shotgun = 1 << 1,
			LMSAW_SuperShotgun = 1 << 2,
			LMSAW_Chaingun = 1 << 3,
			LMSAW_Minigun = 1 << 4,
			LMSAW_RocketLauncher = 1 << 5,
			LMSAW_GrenadeLauncher = 1 << 6,
			LMSAW_PlasmaRifle = 1 << 7,
			LMSAW_Railgun = 1 << 8,
			LMSAW_Chainsaw = 1 << 9
		};

		enum LMSSpectatorSettings
		{
			LMSSS_TalkToActivePlayers = 1 << 0,
			LMSSS_ViewTheGame = 1 << 1
		};

		enum ZandronumGameMode
		{
			GAMEMODE_COOPERATIVE,
			GAMEMODE_SURVIVAL,
			GAMEMODE_INVASION,
			GAMEMODE_DEATHMATCH,
			GAMEMODE_TEAMPLAY,
			GAMEMODE_DUEL,
			GAMEMODE_TERMINATOR,
			GAMEMODE_LASTMANSTANDING,
			GAMEMODE_TEAMLMS,
			GAMEMODE_POSSESSION,
			GAMEMODE_TEAMPOSSESSION,
			GAMEMODE_TEAMGAME,
			GAMEMODE_CTF,
			GAMEMODE_ONEFLAGCTF,
			GAMEMODE_SKULLTAG,
			GAMEMODE_DOMINATION
		};

		static const DMFlags*			dmFlags() { return &dmFlagsList; }
		static const QList<GameMode>*	gameModes() { return &gameModesList; }
		static const QList<GameCVar>*	gameModifiers() { return &gameModifiersList; }

	protected:
		static DMFlags					dmFlagsList;
		static QList<GameMode>			gameModesList;
		static QList<GameCVar>			gameModifiersList;

	private:
		ZandronumGameInfo();

		void						initDMFlags();
		void						initDMFlags1(DMFlagsSection& section);
		void						initDMFlags2(DMFlagsSection& section);
		void						initCompatFlags(DMFlagsSection& section);
		void						initGameModes();
		void						initGameModifiers();

		static ZandronumGameInfo* 	static_constructor;
};

#endif

//------------------------------------------------------------------------------
// srb2gameinfo.cpp
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
// Copyright (C) 2016 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "srb2gameinfo.h"

QString Srb2GameInfo::commandFromFlag(Flag flag)
{
	switch (flag)
	{
	case AllPlayersForExit:
		return "+playersforexit";
	case AllowExitLevel:
		return "+allowexitlevel";
	case AllowTeamChange:
		return "+allowteamchange";
	case RespawnItems:
		return "+respawnitem";
	case RingSlinger:
		return "+ringslinger";
	case TouchTag:
		return "+touchtag";
	default:
		return "";
	}
}

QList<DMFlagsSection> Srb2GameInfo::dmFlags()
{
	DMFlagsSection section(tr("Flags"));
	section << DMFlag(tr("All players must reach exit"), AllPlayersForExit);
	section << DMFlag(tr("Allow attacking in all game modes"), RingSlinger);
	section << DMFlag(tr("Allow level exit in all game modes"), AllowExitLevel);
	section << DMFlag(tr("Allow team change"), AllowTeamChange);
	section << DMFlag(tr("Tag players by simply touching them"), TouchTag);
	section << DMFlag(tr("Respawn items"), RespawnItems);
	section << DMFlag(tr("Listen server"), ListenServer);
	section << DMFlag(tr("Casual server"), CasualServer);

	QList<DMFlagsSection> flags;
	flags << section;
	return flags;
}

QList<GameMode> Srb2GameInfo::gameModes()
{
	QList<GameMode> modes;
	modes << GameMode::mkCooperative();
	modes << GameMode::ffaGame(Competition, tr("Competition"));
	modes << GameMode::ffaGame(Race, tr("Race"));
	modes << GameMode::mkDeathmatch();
	modes << GameMode::mkTeamDeathmatch();
	modes << GameMode::ffaGame(Tag, tr("Tag"));
	modes << GameMode::ffaGame(HideAndSeek, tr("Hide & Seek"));
	modes << GameMode::mkCaptureTheFlag();
	return modes;
}

QList<GameCVar> Srb2GameInfo::limits(const GameMode &gameMode)
{
	QList<GameCVar> limits;
	switch (gameMode.index())
	{
	case GameMode::SGM_Cooperative:
	case Competition:
		limits << GameCVar(tr("Starting lives"), "+startinglives", 3);
		break;
	case Race:
		limits << GameCVar(tr("Laps"), "+numlaps", 4);
		limits << GameCVar(tr("Countdown time"), "+countdowntime", 60);
		break;
	case Tag:
	case HideAndSeek:
		limits << GameCVar(tr("Hide time"), "+hidetime", 30);
		// Intentional fall-through.
	case GameMode::SGM_Deathmatch:
	case GameMode::SGM_TeamDeathmatch:
	case GameMode::SGM_CTF:
		limits << GameCVar(tr("Point limit"), "+pointlimit");
		limits << GameCVar(tr("Time limit"), "+timelimit");
		break;
	}
	if (gameMode.index() == GameMode::SGM_CTF)
	{
		limits << GameCVar(tr("Flag auto-return time"), "+flagtime", 30);
	}
	limits << GameCVar(tr("Respawn item time"), "+respawnitemtime", 30);
	limits << GameCVar(tr("Intermission time"), "+inttime", 20);
	return limits;
}

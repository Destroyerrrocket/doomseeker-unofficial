//------------------------------------------------------------------------------
// Turok2Exgameinfo.cpp
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
// Copyright (C) 2017 "Edward Richardson" <Edward850@crantime.org>
//------------------------------------------------------------------------------
#include "turok2exgameinfo.h"

QList<GameMode> Turok2ExGameInfo::gameModes()
{
	QList<GameMode> gameModesList;
	gameModesList << GameMode::ffaGame(MODE_ROKMATCH, tr("Rok Match"));
	gameModesList << GameMode::teamGame(MODE_TEAMROKMATCH, tr("Team Rok Match"));
	gameModesList << GameMode::ffaGame(MODE_FRAGTAG, tr("Frag Tag"));
	gameModesList << GameMode::ffaGame(MODE_LASTTUROKSTANDING, tr("Last Turok Standing"));
	gameModesList << GameMode::ffaGame(MODE_ROKMIX, tr("Rok Mix"));
	gameModesList << GameMode::ffaGame(MODE_INSTAROK, tr("Insta Rok"));
	gameModesList << GameMode::ffaGame(MODE_RAPTORFEST, tr("Raptor Fest"));
	return gameModesList;
}

QList<GameCVar> Turok2ExGameInfo::limits(const GameMode &gameMode)
{
	QList<GameCVar> limits;
	limits << GameCVar(tr("Frag limit"), "+sv_fraglimit", 20);
	limits << GameCVar(tr("Time limit (in seconds)"), "+sv_timelimit", 600);
	limits << GameCVar(tr("Point limit"), "+sv_pointlimit", 3);
	limits << GameCVar(tr("Force team balance"), "+sv_forcebalanceteams", 1);
	limits << GameCVar(tr("PFM overlap threshold"), "+sv_pfm_maxoverlap", 4);
	return limits;
}


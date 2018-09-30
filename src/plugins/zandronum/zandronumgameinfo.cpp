//------------------------------------------------------------------------------
// zandronumgameinfo.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "zandronumgameinfo.h"

const QString ZandronumGameInfo::OPTION_GAMEVERSION = "GameVersion";

QList<GameMode> ZandronumGameInfo::gameModes()
{
	QList<GameMode> gameModesList;
	gameModesList << GameMode::mkCooperative();;
	gameModesList << GameMode::ffaGame(GAMEMODE_SURVIVAL, tr("Survival"));
	gameModesList << GameMode::ffaGame(GAMEMODE_INVASION, tr("Invasion"));
	gameModesList << GameMode::mkDeathmatch();
	gameModesList << GameMode::mkTeamDeathmatch();
	gameModesList << GameMode::ffaGame(GAMEMODE_DUEL, tr("Duel"));
	gameModesList << GameMode::ffaGame(GAMEMODE_TERMINATOR, tr("Terminator"));
	gameModesList << GameMode::ffaGame(GAMEMODE_LASTMANSTANDING, tr("LMS"));
	gameModesList << GameMode::teamGame(GAMEMODE_TEAMLMS, tr("Team LMS"));
	gameModesList << GameMode::ffaGame(GAMEMODE_POSSESSION, tr("Possession"));
	gameModesList << GameMode::teamGame(GAMEMODE_TEAMPOSSESSION, tr("Team Poss"));
	gameModesList << GameMode::teamGame(GAMEMODE_TEAMGAME, tr("Team Game"));
	gameModesList << GameMode::mkCaptureTheFlag();
	gameModesList << GameMode::teamGame(GAMEMODE_ONEFLAGCTF, tr("One Flag CTF"));
	gameModesList << GameMode::teamGame(GAMEMODE_SKULLTAG, tr("Skulltag"));
	gameModesList << GameMode::teamGame(GAMEMODE_DOMINATION, tr("Domination"));
	gameModesList << GameMode::mkUnknown();
	return gameModesList;
}

QList<GameCVar> ZandronumGameInfo::gameModifiers()
{
	QList<GameCVar> gameModifiersList;
	gameModifiersList << GameCVar("Buckshot", "+buckshot");
	gameModifiersList << GameCVar("Instagib", "+instagib");
	return gameModifiersList;
}

QString ZandronumAboutProvider::provide()
{
	QString aboutDescription = "Zandronum (https://zandronum.com/)\n\n";
	aboutDescription += tr("This plugin is distributed under the terms of the LGPL v2.1 or later.\n\n");
	return aboutDescription;
}
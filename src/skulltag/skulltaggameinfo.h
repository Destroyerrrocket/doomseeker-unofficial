//------------------------------------------------------------------------------
// skulltaggameinfo.h
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
#ifndef __SKULLTAG_GAME_INFO_H_
#define __SKULLTAG_GAME_INFO_H_

#include "serverapi/serverstructs.h"
#include <QObject>

class SkulltagGameInfo : public QObject
{
	public:
		enum SkulltagGameMode
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
		SkulltagGameInfo();

		void						initDMFlags();
		void						initDMFlags1(DMFlagsSection& section);
		void						initDMFlags2(DMFlagsSection& section);
		void						initCompatFlags(DMFlagsSection& section);
		void						initGameModes();
		void						initGameModifiers();

		static SkulltagGameInfo* 	static_constructor;
};

#endif

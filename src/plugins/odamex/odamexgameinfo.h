//------------------------------------------------------------------------------
// odamexgameinfo.h
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
#ifndef __ODAMEX_GAME_INFO_H_
#define __ODAMEX_GAME_INFO_H_

#include "serverapi/serverstructs.h"
#include <QObject>

class OdamexGameInfo : public QObject
{
	public:
		enum OdamexGameModes
		{
			MODE_COOPERATIVE,
			MODE_DEATHMATCH,
			MODE_DEATHMATCH2,
			MODE_TEAM_DEATHMATCH,
			MODE_CAPTURE_THE_FLAG,
			MODE_DUEL
		};

		static const QList<DMFlagsSection>*	dmFlags() { return &dmFlagsList; }
		static const QList<GameMode>*	gameModes() { return &gameModesList; }
		static GameMode gameModeDuel();

	protected:
		static QList<DMFlagsSection>	dmFlagsList;
		static QList<GameMode>			gameModesList;

	private:
		OdamexGameInfo();

		void						initDMFlags();
		void						initGameModes();

		static OdamexGameInfo* 		static_constructor;
};

#endif

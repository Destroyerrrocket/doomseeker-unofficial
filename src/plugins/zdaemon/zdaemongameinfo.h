//------------------------------------------------------------------------------
// zdaemongameinfo.h
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
#ifndef __ZDAEMON_GAME_INFO_H_
#define __ZDAEMON_GAME_INFO_H_

#include "serverapi/serverstructs.h"
#include <QObject>

class ZDaemonGameInfo : public QObject
{
	public:
		enum ZDaemonGameModes
		{
			MODE_DEATHMATCH,
			MODE_TEAM_DEATHMATCH,
			MODE_COOPERATIVE,
			MODE_CAPTURE_THE_FLAG,
			MODE_SURVIVAL,
			MODE_DOUBLE_DOMINATION
		};

		static const DMFlags* dmFlags() { return &dmFlagsList; }
		static const QList<GameMode>* gameModes() { return &gameModesList; }

	protected:
		static DMFlags dmFlagsList;
		static QList<GameMode> gameModesList;

	private:
		ZDaemonGameInfo();

		void initDMFlags();
		void initDMFlags1(DMFlagsSection& section);
		void initDMFlags2(DMFlagsSection& section);
		void initGameModes();

		static ZDaemonGameInfo* static_constructor;
};

#endif

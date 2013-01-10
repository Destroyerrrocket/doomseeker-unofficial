//------------------------------------------------------------------------------
// player.h
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
#ifndef __PLAYER_H_
#define __PLAYER_H_

#define MAX_TEAMS 4

#include "global.h"
#include <QHash>
#include <QString>

/**
 *	@brief Data structure that holds information about players in a server.
 */
class MAIN_EXPORT Player
{
	public:
		enum PlayerTeam
		{
			TEAM_BLUE,
			TEAM_RED,
			TEAM_GREEN,
			TEAM_GOLD,

			TEAM_NONE = 0xFF
		};

		Player();
		Player(const QString &name, unsigned short score, unsigned short ping, PlayerTeam team=TEAM_NONE, bool spectator=false, bool bot=false);

		const QString	&name() const { return playerName; }
		short			score() const { return currentScore; }
		unsigned short	ping() const { return currentPing; }
		bool			isSpectating() const { return spectator; }
		bool			isBot() const { return bot; }
		bool			isTeamlessBot() const { return bot && team == TEAM_NONE; }
		PlayerTeam		teamNum() const { return team; }

		bool			operator==(const Player& other) const;

		/**
		 * Formats string into HTML format.
		 */
		QString			nameFormatted() const;

		/**
		 * Seeks for characters that are not from the <32; 126> range,
		 * removes them and the characters that appear after them,
		 * then returns new string.
		 */
		QString			nameColorTagsStripped() const;

	protected:

		QString			playerName;
		short			currentScore;
		unsigned short	currentPing;
		bool			spectator;
		bool			bot;
		PlayerTeam		team;
};

uint qHash(const Player& player);

#endif

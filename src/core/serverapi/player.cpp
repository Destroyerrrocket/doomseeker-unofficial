//------------------------------------------------------------------------------
// player.cpp
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
#include "player.h"
#include "strings.h"

Player::Player()
{
	currentScore = 0;
	currentPing = 0;
	spectator = false;
	bot = false;
	team = TEAM_NONE;
}

Player::Player(const QString &name, unsigned short score, unsigned short ping, PlayerTeam team, bool spectator, bool bot)
: playerName(name), currentScore(score), currentPing(ping),
  spectator(spectator), bot(bot), team(team)
{
}

QString Player::nameColorTagsStripped() const
{
	QString ret;
	for (int i = 0; i < playerName.length(); ++i)
	{
		if (playerName[i] < 32 || playerName[i] > 126)
		{
			// Lets only remove the following character on \c.
			// Removing the control characters is still a good idea though.
			if(playerName[i] == ESCAPE_COLOR_CHAR)
				++i;
			continue;
		}

		ret += playerName[i];
	}
	return ret;
}

QString	Player::nameFormatted() const
{
	QString ret;
	for (int i = 0; i < playerName.length(); ++i)
	{
		// cut out bad characters
		if ((playerName[i] < 32 || playerName[i] > 126) && playerName[i] != ESCAPE_COLOR_CHAR)
			continue;

		switch (playerName[i].toAscii())
		{
			case '<':
				ret += "&lt;";
				break;

			case '>':
				ret += "&gt;";
				break;

			default:
				ret += playerName[i];
				break;
		}
	}

	return Strings::colorizeString(ret);
}

bool Player::operator==(const Player& other) const
{
	return name().compare(other.name(), Qt::CaseInsensitive) == 0;
}

//////////////////////////////////////////////////////////////////////////////

uint qHash(const Player& player)
{
	return qHash(player.name());
}

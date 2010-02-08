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

// \c = '\034'
#define	ESCAPE_COLOR	'\034'

Player::Player(const QString &name, unsigned short score, unsigned short ping, PlayerTeam team, bool spectator, bool bot)
: playerName(name), currentScore(score), currentPing(ping),
  spectator(spectator), bot(bot), team(team)
{
}

const char Player::colorChart[22][7] =
{
	"FF91A4", //a
	"D2B48C", //b
	"808080", //c
	"32CD32", //d
	"918151", //e
	"F4C430", //f
	"E32636", //g
	"0000FF", //h
	"FF8C00", //i
	"C0C0C0", //j
	"FFD700", //k
	"E34234", //l
	"000000", //m
	"4169E1", //n
	"FFDEAD", //o
	"465945", //p
	"228b22", //q
	"800000", //r
	"704214", //s
	"A020F0", //t
	"404040", //u
	"007F7F", //v
};
QString Player::colorizeString(const QString &str, int current)
{
	QString ret;
	bool colored = false;
	for(int i = 0;i < str.length();i++)
	{
		if(str[i] == ESCAPE_COLOR)
		{
			i++;
			if(i >= str.length())
				break;
			QChar colorChar = str[i].toLower();
			int color = colorChar.toAscii() - 97;

			// special cases
			if(colorChar == '+')
				color = current == 0 ? 19 : current-1; // + is the current minus one, wrap if needed.
			else if(colorChar == '*')
				color = 3; // Chat color which is usally green
			else if(colorChar == '!')
				color = 16; // Team char (usually green, but made dark green here for distinction)
			else if(colorChar == '[') // Named!
			{
				int end = str.indexOf(']', i);
				if(end == -1)
					break;
				QString colorName = str.mid(i+1, end-i-1);
				if(colorName.indexOf('"') == -1) // Just in case there's a security problem.
					ret += QString("<span style=\"color: " + colorName + "\">");
				i += colorName.length()+1;
				colored = true;
				continue;
			}
			else if(colorChar == '-')
			{
				if(colored)
					ret += "</span>";
				colored = false;
				continue;
			}

			if(colored)
			{
				ret += "</span>";
				colored = false;
			}

			if(color >= 0 && color < 22)
			{
				ret += QString("<span style=\"color: #") + colorChart[color] + "\">";
				colored = true;
			}
			continue;
		}
		ret += str[i];
	}
	if(colored)
		ret += "</span>";
	return ret;
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
			if(playerName[i] == ESCAPE_COLOR)
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
		if ((playerName[i] < 32 || playerName[i] > 126) && playerName[i] != ESCAPE_COLOR)
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

	return colorizeString(ret);
}

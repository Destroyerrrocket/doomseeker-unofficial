//------------------------------------------------------------------------------
// gameinfotip.cpp
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
#include "gameinfotip.h"
#include "serverapi/playerslist.h"
#include "serverapi/server.h"
#include "serverapi/serverstructs.h"

class GameInfoTip::PrivData
{
	public:
		ServerCPtr server;
};

const QString GameInfoTip::UNLIMITED = QObject::tr("Unlimited");

GameInfoTip::GameInfoTip(const ServerCPtr &server)
{
	d = new PrivData();
	d->server = server;
}

GameInfoTip::~GameInfoTip()
{
	delete d;
}

QString GameInfoTip::generateHTML()
{
	QString table = "<table>";
	table += timelimitHTML();
	table += scorelimitHTML();
	table += teamScoresHTML();
	table += playersHTML();
	table += "</table>";

	return table;
}

QString	GameInfoTip::playersHTML()
{
	const QString PLAYERS = tr("Players");

	const PlayersList* playersList = d->server->players();
	int canJoin = d->server->maxPlayers() - playersList->numClients();
	if(canJoin < 0)
	{
		canJoin = 0;
	}

	QString players = "<tr><td>" + PLAYERS + ":&nbsp;</td><td>%1 / %2 (%3 can join)</td></tr>";
	players = players.arg(playersList->numClients()).arg(d->server->numTotalSlots()).arg(canJoin);

	return players;
}

QString	GameInfoTip::limitHTML(QString limitName, QString valueArgsTemplate, int value)
{
	QString row = "<tr><td>" + limitName + ":&nbsp;</td><td>" + valueArgsTemplate + "</td></tr>";

	if (value == 0)
	{
		row = row.arg(UNLIMITED);
	}
	else
	{
		row = row.arg(value);
	}

	return row;
}

QString	GameInfoTip::scorelimitHTML()
{
	const QString SCORELIMIT = tr("Scorelimit");
	QString row = limitHTML(SCORELIMIT, "%1", d->server->scoreLimit());

	return row;
}

QString GameInfoTip::teamScoresHTML()
{
	QString teamScores;
	if (d->server->gameMode().isTeamGame())
	{
		teamScores = "<tr><td colspan=\"2\">%1</td></tr>";
		QString teams;
		bool bPrependBar = false;
		for (int i = 0; i < MAX_TEAMS; ++i)
		{
			if (d->server->players()->numPlayersOnTeam(i) != 0)
			{
				if (bPrependBar)
				{
					teams += " | ";
				}
				teams += d->server->teamName(i) + ": " + QString::number(d->server->score(i));
				bPrependBar = true;
			}
		}
		teamScores = teamScores.arg(teams);
	}

	return teamScores;
}

QString	GameInfoTip::timelimitHTML()
{
	const QString TIMELIMIT = tr("Timelimit");
	int timeLimit = d->server->timeLimit();
	QString row = limitHTML(TIMELIMIT, "%1 %2", timeLimit);

	QString timeLeft = "";
	if (timeLimit != 0)
	{
		timeLeft = tr("(%1 left)").arg(d->server->timeLeft());

	}
	row = row.arg(timeLeft);

	return row;
}

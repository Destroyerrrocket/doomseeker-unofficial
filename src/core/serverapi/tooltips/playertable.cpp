//------------------------------------------------------------------------------
// playertable.cpp
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
#include "playertable.h"
#include "serverapi/server.h"
#include "serverapi/serverstructs.h"

PlayerTable::PlayerTable(const Server* server)
: numOfColumns(0), pServer(server)
{
	setNumberOfColumns();
}

QString PlayerTable::generateHTML()
{
	QString tableStyle = "background-color: #FFFFFF; color: #000000";
	QString table = "<table cellspacing=\"4\" style=\"" + tableStyle + "\">";
	table += tableHeader();
	table += tableContent();
	table += "</table>";
	return table;
}

void PlayerTable::setNumberOfColumns()
{
	if (pServer->gameMode().isTeamGame())
	{
		numOfColumns = 5;
	}
	else
	{
		numOfColumns = 4;
	}
}

QString PlayerTable::spawnPartOfPlayerTable(PlayersList& list, bool bAppendEmptyRowAtBeginning)
{
	QString ret;
	if (list.count() != 0)
	{
		if (bAppendEmptyRowAtBeginning)
		{
			ret = QString("<tr><td colspan=\"%1\">&nbsp;</td></tr>").arg(numOfColumns);
		}

		for (int i = 0; i < list.count(); ++i)
		{
			const Player& player = list[i];

			QString status = "";
			if (player.isBot())
			{
				status = tr("BOT");
			}
			else if (player.isSpectating())
			{
				status = tr("SPECTATOR");
			}

			QString strPlayer = "<tr>";
			if (pServer->gameMode().isTeamGame())
			{
				strPlayer += QString("<td>%1</td>").arg(pServer->teamName(player.teamNum()));
			}
			strPlayer += "<td>%1</td><td align=\"right\">%2</td><td align=\"right\">%3</td><td>%4</td></tr>";
			strPlayer = strPlayer.arg(player.nameFormatted()).arg(player.score()).arg(player.ping());
			strPlayer = strPlayer.arg(status);

			ret += strPlayer;
		}
	}

	return ret;
}

QString	PlayerTable::spawnPlayersRows(PlayersByTeams& playersByTeams)
{
	QString playersRows;

	bool bAppendEmptyRowAtBeginning = false;
	QMap<PairPlayersByTeams>::iterator it;
	for (it = playersByTeams.begin(); it != playersByTeams.end(); ++it)
	{
		playersRows += spawnPartOfPlayerTable(it.value(), bAppendEmptyRowAtBeginning);
		if (!bAppendEmptyRowAtBeginning)
		{
			bAppendEmptyRowAtBeginning = true;
		}
	}

	return playersRows;
}

QString	PlayerTable::tableContent()
{
	PlayersByTeams playersByTeams;
	PlayersList bots, spectators;

	const PlayersList* playersList = pServer->players();

	playersList->inGamePlayersByTeams(playersByTeams);
	playersList->botsWithoutTeam(bots);
	playersList->spectators(spectators);

	bool bAppendEmptyRowAtBeginning = false;
	QString playersRows = spawnPlayersRows(playersByTeams);

	bAppendEmptyRowAtBeginning = !playersRows.isEmpty();
	QString botsRows = spawnPartOfPlayerTable(bots, bAppendEmptyRowAtBeginning);

	bAppendEmptyRowAtBeginning = !(botsRows.isEmpty() && playersRows.isEmpty());
	QString spectatorsRows = spawnPartOfPlayerTable(spectators, bAppendEmptyRowAtBeginning);

	QString content = playersRows + botsRows + spectatorsRows;

	return content;
}

QString PlayerTable::tableHeader()
{
	const QString TEAM = tr("Team");
	const QString PLAYER = tr("Player");
	const QString SCORE = tr("Score");
	const QString PING = tr("Ping");
	const QString STATUS = tr("Status");

	QString teamHeader;
	QString header = "<tr>";

	if (pServer->gameMode().isTeamGame())
	{
		teamHeader = "<td>" + TEAM + "</td>";
	}

	header += teamHeader;
	header += "<td>" + PLAYER + "</td>";
	header += "<td align=\"right\">&nbsp;" + SCORE + "</td>";
	header += "<td align=\"right\">&nbsp;" + PING + "</td>";
	header += "<td>" + STATUS + "</td>";
	header += "</tr>";
	header += QString("<tr><td colspan=\"%1\"><hr width=\"100%\"></td></tr>").arg(numOfColumns);

	return header;
}

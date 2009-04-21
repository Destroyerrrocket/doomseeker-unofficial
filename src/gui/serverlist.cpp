//------------------------------------------------------------------------------
// serverlist.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include "gui/serverlist.h"
#include <QHeaderView>
#include <QStandardItem>
#include <QToolTip>
#include "skulltag/skulltagmasterclient.h"


SLHandler::SLHandler(ServerListView* tab)
{
	table = tab;
	master = NULL;
	prepareServerTable();
}

SLHandler::~SLHandler()
{
	if (master != NULL)
	{
		delete master;
	}
}

void SLHandler::clearTable()
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	model->destroyRows();
}

void SLHandler::prepareServerTable()
{
	sortOrder = Qt::AscendingOrder;
	sortIndex = -1;

	ServerListModel* model = new ServerListModel(this);
	model->prepareHeaders();
	table->setModel(model);

	// Now set column widths and other properties
	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		ServerListColumn* columns = ServerListModel::columns;
		table->setColumnWidth(i, columns[i].width);
		table->setColumnHidden(i, columns[i].bHidden);
	}

	// We don't really need a vertical header so lets remove it
	table->verticalHeader()->hide();
	// Also some other flags that can't be set from the Designer
	table->horizontalHeader()->setSortIndicatorShown(true);
	table->horizontalHeader()->setHighlightSections(false);

	table->setMouseTracking(true);

	QHeaderView* header = table->horizontalHeader();
	connect(header, SIGNAL( sectionClicked(int) ), this, SLOT ( columnHeaderClicked(int) ) );
	connect(model, SIGNAL( allRowsContentChanged() ), table, SLOT( updateAllRows() ) );
	connect(table, SIGNAL( rightMouseClick(const QModelIndex&) ), this, SLOT ( tableRightClicked(const QModelIndex&)) );
	connect(table, SIGNAL( entered(const QModelIndex&) ), this, SLOT ( mouseEntered(const QModelIndex&)) );
	connect(table, SIGNAL( leftMouseDoubleClicked(const QModelIndex&)), this, SLOT( doubleClicked(const QModelIndex&)) );

	columnHeaderClicked(0);
}
/////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
QString SLHandler::createPlayersToolTip(const Server* server)
{
	if (server == NULL)
		return QString();

	QString firstTable = spawnGeneralInfoTable(server);
	QString plTab = spawnPlayerTable(server);

	QString ret;
	ret = "<div style='white-space: pre'>";
	ret += firstTable;
	ret += plTab;
	ret += "</div>";
	return ret;
}

QString SLHandler::createServerNameToolTip(const Server* server)
{
	if (server == NULL)
		return QString();

	QString ret;
	ret = "<div style='white-space: pre'>";
	ret += server->generalInfoHTML();
	ret += "</div>";
	return ret;
}

QString SLHandler::createPwadsToolTip(const Server* server)
{
	if (server == NULL)
		return QString();

	QString ret;
	ret = "<div style='white-space: pre'>";
	ret += server->pwads().join("\n");
	ret += "</div>";
	return ret;
}
//////////////////////////////////////////////////////////////


void SLHandler::setMaster(MasterClient* mc)
{
	if (master != NULL)
	{
		delete master;
	}

	clearTable();
	master = mc;
}
//////////////////////////////////////////////////////////////
QString SLHandler::spawnGeneralInfoTable(const Server* server)
{
	const QString timelimit = tr("Timelimit");
	const QString scorelimit = tr("Scorelimit");
	const QString unlimited = tr("Unlimited");
	const QString players = tr("Players");

	// Timelimit
    QString firstTableTimelimit = "<TR><TD>" + timelimit + ":&nbsp;</TD><TD>%1 %2</TD></TR>";
    if (server->timeLimit() == 0)
    {
    	firstTableTimelimit = firstTableTimelimit.arg(unlimited, "");
    }
    else
    {
		QString strLeft = tr("(%1 left)").arg(server->timeLeft());
		firstTableTimelimit = firstTableTimelimit.arg(server->timeLimit()).arg(strLeft);
    }

	// Scorelimit
	QString firstTableScorelimit = "<TR><TD>" + scorelimit + ":&nbsp;</TD><TD>%1</TD></TR>";
	if (server->scoreLimit() == 0)
	{
		firstTableScorelimit = firstTableScorelimit.arg(unlimited);
	}
	else
	{
		firstTableScorelimit = firstTableScorelimit.arg(server->scoreLimit());
	}

	// Team score
	QString firstTableTeamscore;
	if (server->gameMode().isTeamGame())
	{
		firstTableTeamscore = "<TR><TD COLSPAN=2>%1</TD></TR>";
		QString teams;
		bool bPrependBar = false;
		for (int i = 0; i < MAX_TEAMS; ++i)
		{
			if (server->teamPlayerCount(i) != 0)
			{
				if (bPrependBar)
					teams += " | ";
				teams += Player::teamName(i) + ": " + QString::number(server->score(i));
				bPrependBar = true;
			}
		}
		firstTableTeamscore = firstTableTeamscore.arg(teams);
	}

	// Players
	QString firstTablePlayers = "<TR><TD>" + players + ":&nbsp;</TD><TD>%1 / %2</TD></TR>";
	firstTablePlayers = firstTablePlayers.arg(server->numPlayers()).arg(server->maximumClients());

	QString firstTable = "<TABLE>";
	firstTable += firstTableTimelimit;
	firstTable += firstTableScorelimit;
	firstTable += firstTableTeamscore;
	firstTable += firstTablePlayers;
	firstTable += "</TABLE>";

	return firstTable;
}

QString SLHandler::spawnPlayerTable(const Server* server)
{
	// Sort the players out first.
	QHash<int, QList<const Player*> > sortedPlayers;
	QList<const Player*> botList;
	QList<const Player*> specList;

	for (int i = 0; i < server->numPlayers(); ++i)
	{
		const Player& p = server->player(i);

		if (p.isSpectating())
		{
			specList.append(&p);
			continue;
		}

		if (server->gameMode().isTeamGame())
		{
			int team = p.teamNum();

			QHash<int, QList<const Player*> >::iterator it = sortedPlayers.find(team);
			if (it == sortedPlayers.end())
			{
				QList<const Player*> l;
				l.append(&p);
				sortedPlayers.insert(team, l);
			}
			else
			{
				it.value().append(&p);
			}
		}
		else
		{
			if (p.isBot())
			{
				botList.append(&p);
				continue;
			}

			if (sortedPlayers.count() == 0)
				sortedPlayers.insert(0, QList<const Player*>());

			sortedPlayers.find(0).value().append(&p);
		}

	}

	const QString team = tr("Team");
	const QString player = tr("Player");
	const QString score = tr("Score");
	const QString ping = tr("Ping");
	const QString status = tr("Status");

	QString plTabTeamHeader;
	QString plTabHeader = "<TR>";
	int plTabColNum = 4;
	if (server->gameMode().isTeamGame())
	{
		plTabColNum = 5;
		plTabTeamHeader = "<TD>" + team + "</TD>";
	}
	plTabHeader += plTabTeamHeader + "<TD>" + player + "</TD><TD ALIGN=right>&nbsp;" + score + "</TD><TD ALIGN=right>&nbsp;" + ping + "</TD><TD ALIGN>" + status + "</TD></TR>";
	plTabHeader += QString("<TR><TD COLSPAN=%1><HR WIDTH=100%></TD></TR>").arg(plTabColNum);

	QString plTabPlayers;
	QHash<int, QList<const Player*> >::iterator it;
	bool bAppendEmptyRowAtBeginning = false;
	for (it = sortedPlayers.begin(); it != sortedPlayers.end(); ++it)
	{
		plTabPlayers += spawnPartOfPlayerTable(it.value(), "", plTabColNum, server->gameMode().isTeamGame(), bAppendEmptyRowAtBeginning);
		if (!bAppendEmptyRowAtBeginning)
		{
			bAppendEmptyRowAtBeginning = true;
		}
	}

	bAppendEmptyRowAtBeginning = !plTabPlayers.isEmpty();
	QString plTabBots = spawnPartOfPlayerTable(botList, tr("BOT"), plTabColNum, server->gameMode().isTeamGame(), bAppendEmptyRowAtBeginning);

	bAppendEmptyRowAtBeginning = !(plTabBots.isEmpty() && plTabPlayers.isEmpty());
	QString plTabSpecs = spawnPartOfPlayerTable(specList, tr("SPECTATOR"), plTabColNum, server->gameMode().isTeamGame(), bAppendEmptyRowAtBeginning);


	QString plTab = "<table cellspacing=\"4\" style=\"background-color: #FFFFFF;color: #000000\">";
	plTab += plTabHeader;
	plTab += plTabPlayers;
	plTab += plTabBots;
	plTab += plTabSpecs;
	plTab += "</table>";
	return plTab;
}

QString SLHandler::spawnPartOfPlayerTable(QList<const Player*> list, QString status, int colspan, bool isTeamgame, bool bAppendEmptyRowAtBeginning)
{
	QString ret;
	if (list.count() != 0)
	{
		if (bAppendEmptyRowAtBeginning)
		{
			ret = QString("<TR><TD COLSPAN=%1>&nbsp;</TD></TR>").arg(colspan);
		}

		for (int i = 0; i < list.count(); ++i)
		{
			const Player& p = *list[i];

			QString strPlayer = "<TR>";
			if (isTeamgame)
			{
				strPlayer += QString("<TD>%1</TD>").arg(p.teamName());
			}
			strPlayer += "<TD>%1</TD><TD ALIGN=right>%2</TD><TD ALIGN=right>%3</TD><TD>%4</TD></TR>";
			strPlayer = strPlayer.arg(p.nameFormatted()).arg(p.score()).arg(p.ping());
			strPlayer = strPlayer.arg(status);

			ret += strPlayer;
		}
	}

	return ret;
}
//////////////////////////////////////////////////////////////
// Slots
void SLHandler::serverUpdated(Server *server, int response)
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	QModelIndex index = model->findServerOnTheList(server);
	int row = 0;
	if (index.isValid())
	{
		row = model->updateServer(index.row(), server, response);
	}
	else
	{
		row = model->addServer(server, response);
	}

	if (sortIndex >= 0)
	{
		model->sort(sortIndex, sortOrder);
	}

	table->resizeRowToContents(row);
}

void SLHandler::refreshAll()
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	for (int i = 0; i < table->model()->rowCount(); ++i)
	{
		model->setRefreshing(i);
	}
}

void SLHandler::tableRightClicked(const QModelIndex& index)
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	QItemSelectionModel* selModel = table->selectionModel();
	QModelIndexList indexList = selModel->selectedRows();

	for(int i = 0; i < indexList.count(); ++i)
	{
		model->setRefreshing(indexList[i].row());
	}
}

void SLHandler::columnHeaderClicked(int index)
{
	// if user clicked on different column than the sorting is currently set on
	if (sortIndex != index)
	{
		// set sorting order to default for current column
		switch(index)
		{
			case ServerListModel::SLCID_PLAYERS:
				sortOrder = Qt::DescendingOrder;
				break;

			default:
				sortOrder = Qt::AscendingOrder;
				break;
		}
	}
	// if user clicked on the same column
	else
	{
		// change sorting order
		if (sortOrder == Qt::AscendingOrder)
		{
			sortOrder = Qt::DescendingOrder;
		}
		else
		{
			sortOrder = Qt::AscendingOrder;
		}
	}
	sortIndex = index;

	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	model->sort(sortIndex, sortOrder);

	QHeaderView* header = table->horizontalHeader();
	header->setSortIndicator(sortIndex, sortOrder);

	//table->fixRowSize();
}

void SLHandler::mouseEntered(const QModelIndex& index)
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	Server* server = model->serverFromList(index);
	QString tooltip;

	if (!server->isKnown())
	{
		tooltip = "";
	}
	else
	{
		switch(index.column())
		{
			case ServerListModel::SLCID_PLAYERS:
				tooltip = createPlayersToolTip(server);
				break;

			case ServerListModel::SLCID_SERVERNAME:
				tooltip = createServerNameToolTip(server);
				break;

			case ServerListModel::SLCID_WADS:
				if (server->numWads() == 0)
				{
					tooltip = "";
				}
				else
				{
					tooltip = createPwadsToolTip(server);
				}
				break;

			default:
				tooltip = "";
				break;
		}
	}

	QToolTip::showText(QCursor::pos(), tooltip, table);
}

void SLHandler::doubleClicked(const QModelIndex& index)
{
	ServerListModel* model = static_cast<ServerListModel*>(table->model());
	Server* server = model->serverFromList(index);
	emit serverDoubleClicked(server);
}

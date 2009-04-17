#include "gui/serverlist.h"
#include <QHeaderView>
#include <QStandardItem>
#include "skulltag/skulltagmasterclient.h"

ServerListColumn SLHandler::columns[] =
{
	{ tr("Players"), 60 },
	{ tr("Ping"), 50 },
	{ tr("Servername"), 200 },
	{ tr("Address"), 120 },
	{ tr("IWAD"), 90 },
	{ tr("MAP"), 70 },
	{ tr("Wads"), 120 },
	{ tr("Gametype"), 150 }
};
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
SLHandler::SLHandler(QTableView* tab)
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
	if (table != NULL)
	{
		int rowCount = table->model()->rowCount();
		table->model()->removeRows(1, rowCount - 1);
	}
}
////////////////////////////////////////////////////
void SLHandler::fillItem(QStandardItem* item, const QString& str)
{
	QString newStr = str.toLower();
	item->setData(str, Qt::DisplayRole);
	item->setData(newStr, SLDT_SORT);
}

void SLHandler::fillItem(QStandardItem* item, int num)
{
	QVariant var = num;

	item->setData(var, Qt::DisplayRole);
	item->setData(var, SLDT_SORT);
}

void SLHandler::fillItem(QStandardItem* item, const QHostAddress& addr)
{
	QVariant var = addr.toIPv4Address();

	item->setData(addr.toString(), Qt::DisplayRole);
	item->setData(var, SLDT_SORT);
}
////////////////////////////////////////////////////
void SLHandler::prepareServerTable()
{
	sortOrder = Qt::AscendingOrder;
	sortIndex = -1;

	QStandardItemModel* model = new QStandardItemModel(this);

	QStringList labels;
	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		labels << columns[i].name;
	}
	model->setHorizontalHeaderLabels(labels);
	model->setSortRole(SLDT_SORT);

	table->setModel(model);

	// Now set column widths
	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		table->setColumnWidth(i, columns[i].width);
	}

	// We don't really need a vertical header so lets remove it
	table->verticalHeader()->hide();
	// Also some other flags that can't be set from the Designer
	table->horizontalHeader()->setSortIndicatorShown(true);
	table->horizontalHeader()->setHighlightSections(false);

	QHeaderView* header = table->horizontalHeader();
	connect(header, SIGNAL( sectionClicked(int) ), this, SLOT ( columnHeaderClicked(int) ) );
	connect(table, SIGNAL( rightMouseClick(const QModelIndex&) ), this, SLOT ( tableRightClicked(const QModelIndex&)) );

	columnHeaderClicked(0);
}
/////////////////////////////////////////////////////////
QModelIndex SLHandler::findServerOnTheList(const Server* server)
{
	if (server != NULL)
	{
	    QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());
		for (int i = 0; i < model->rowCount(); ++i)
		{
			QStandardItem* item = model->item(i);
			const Server* savedServ = serverFromList(item);
			if (server == savedServ)
			{
				QModelIndex index = model->indexFromItem(item);
				return index;
			}
		}
	}
	return QModelIndex();
}
//////////////////////////////////////////////////////////////
void SLHandler::addServer(Server* server)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());
	QList<QStandardItem*> columns;

	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		columns.append(new QStandardItem());
	}

	model->appendRow(columns);
	QModelIndex index = model->indexFromItem(columns[0]);
	updateServer(index.row(), server);

	// Compress row size.
	table->resizeRowToContents(model->indexFromItem(columns[0]).row());
}

void SLHandler::updateServer(int row, Server* server)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());
	QStandardItem* item;
	QString strTmp;

    item = model->item(row, SLCID_PLAYERS);
    // Save pointer to the column
	ServerPointer ptr(server);
	QVariant savePointer = qVariantFromValue(ptr);
	item->setData(savePointer, SLDT_POINTER_TO_SERVER_STRUCTURE);

	fillItem(item, server->numPlayers());
	item->setToolTip(createPlayersToolTip(server));

	item = model->item(row, SLCID_PING);
	fillItem(item, server->ping());

	item = model->item(row, SLCID_SERVERNAME);
	fillItem(item, server->name());

	item = model->item(row, SLCID_ADDRESS);
	fillItem(item, server->address());

	item = model->item(row, SLCID_IWAD);
	fillItem(item, server->iwadName());

	item = model->item(row, SLCID_MAP);
	fillItem(item, server->map());

	strTmp = server->pwads().join(" ");
	item = model->item(row, SLCID_WADS);
	fillItem(item, strTmp);

	item = model->item(row, SLCID_GAMETYPE);
	fillItem(item, server->gameMode().name());

}

void SLHandler::setRefreshing(int row)
{
	Server* serv = serverFromList(row);
	serv->refresh();

	QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());
	QStandardItem* item = model->item(row, SLCID_SERVERNAME);
	item->setText(tr("<REFRESHING>"));
}
//////////////////////////////////////////////////////////////
QString SLHandler::createPlayersToolTip(const Server* server) const
{
	const QString timelimit = tr("Timelimit");
	const QString scorelimit = tr("Scorelimit");
	const QString unlimited = tr("Unlimited");
	const QString players = tr("Players");

	QString ret;


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
	// END OF FIRST TABLE



	// Player table
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
	plTabHeader += plTabTeamHeader + "<TD>" + player + "</TD><TD>" + score + "</TD><TD>" + ping + "</TD><TD>" + status + "</TD></TR>";
	plTabHeader += QString("<TR><TD COLSPAN=%1><HR WIDTH=100%></TD></TR>").arg(plTabColNum);

	QString plTabPlayers;
	for (int i = 0; i < server->numPlayers(); ++i)
	{
		const Player& p = server->player(i);

		QString strPlayer = "<TR>";
		if (server->gameMode().isTeamGame())
		{
			strPlayer += QString("<TD>%1</TD>").arg(p.teamName());
		}
		strPlayer += "<TD>%1</TD><TD ALIGN=right>%2</TD><TD ALIGN=right>%3</TD><TD>%4</TD></TR>";
		strPlayer = strPlayer.arg(p.nameFormatted()).arg(p.score()).arg(p.ping());
		strPlayer = strPlayer.arg("STATUS");

		plTabPlayers += strPlayer;
	}

	QString plTab = "<TABLE CELLSPACING=5>";
	plTab += plTabHeader;
	plTab += plTabPlayers;
	plTab += "</TABLE>";

	ret = "<p style='white-space: pre'>";
	ret += firstTable;
	ret += plTab;
	ret += "</p>";
	return ret;
}
//////////////////////////////////////////////////////////////
Server* SLHandler::serverFromList(int rowNum)
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());

    QStandardItem* item = model->item(rowNum);
    return serverFromList(item);
}

Server* SLHandler::serverFromList(const QModelIndex& index)
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());

    QStandardItem* item = model->item(index.row());
    return serverFromList(item);
}

Server* SLHandler::serverFromList(const QStandardItem* item)
{
    QVariant pointer = qVariantFromValue(item->data(SLDT_POINTER_TO_SERVER_STRUCTURE));
    if (!pointer.isValid())
    {
        return NULL;
    }
    ServerPointer savedServ = qVariantValue<ServerPointer>(pointer);
    return savedServ.ptr;
}

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
// Slots
void SLHandler::serverUpdated(Server *server, int response)
{
	QModelIndex index = findServerOnTheList(server);
	if (index.isValid())
	{
		updateServer(index.row(), server);
	}
	else
	{
		addServer(server);
	}

	if (sortIndex >= 0)
	{
		QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());
		model->sort(sortIndex, sortOrder);
	}
}

void SLHandler::refreshAll()
{
	for (int i = 0; i < table->model()->rowCount(); ++i)
	{
		setRefreshing(i);
	}
}

void SLHandler::tableRightClicked(const QModelIndex& index)
{
	QItemSelectionModel* selModel = table->selectionModel();
	QModelIndexList indexList = selModel->selectedRows();

	for(int i = 0; i < indexList.count(); ++i)
	{
		setRefreshing(indexList[i].row());
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
			case SLCID_PLAYERS:
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

	QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());
	model->sort(sortIndex, sortOrder);

	QHeaderView* header = table->horizontalHeader();
	header->setSortIndicator(sortIndex, sortOrder);
}

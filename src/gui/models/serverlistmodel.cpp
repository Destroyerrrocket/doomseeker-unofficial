#include "gui/models/serverlistmodel.h"

ServerListColumn ServerListModel::columns[] =
{
	{ tr("Players"), 60, false },
	{ tr("Ping"), 50, false },
	{ tr("Servername"), 200, false },
	{ tr("Address"), 120, false },
	{ tr("IWAD"), 90, false },
	{ tr("MAP"), 70, false },
	{ tr("Wads"), 120, false },
	{ tr("Gametype"), 150, false },
	{ "SORT_GROUP", 0, true },
	{ "SERVER_POINTER", 0, true }
};
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
ServerListModel::ServerListModel(QObject* parent) : QStandardItemModel(parent)
{
	setSortRole(SLDT_SORT);
}

void ServerListModel::destroyRows()
{
	int rows = rowCount();
	removeRows(0, rows);
}

void ServerListModel::clearRows()
{
	int rows = rowCount();
	for (int i = 0; i < rows; ++i)
	{
		takeRow(0);
	}
}

void ServerListModel::prepareHeaders()
{
	QStringList labels;
	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		labels << columns[i].name;
	}
	setHorizontalHeaderLabels(labels);
}

void ServerListModel::fillItem(QStandardItem* item, const QString& str)
{
	QString newStr = str.toLower();
	item->setData(str, Qt::DisplayRole);
	item->setData(newStr, SLDT_SORT);
}

void ServerListModel::fillItem(QStandardItem* item, int num)
{
	QVariant var = num;

	item->setData(var, Qt::DisplayRole);
	item->setData(var, SLDT_SORT);
}

void ServerListModel::fillItem(QStandardItem* item, const QHostAddress& addr, const QString& actualDisplay)
{
	QVariant var = addr.toIPv4Address();

	if (actualDisplay.isEmpty())
	{
		item->setData(addr.toString(), Qt::DisplayRole);
	}
	else
	{
		item->setData(actualDisplay, Qt::DisplayRole);
	}
	item->setData(var, SLDT_SORT);
}

int ServerListModel::addServer(Server* server, int response)
{
	QList<QStandardItem*> columns;

	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		columns.append(new QStandardItem());
	}

	appendRow(columns);
	QModelIndex index = indexFromItem(columns[0]);
	return updateServer(index.row(), server, response);
}

int ServerListModel::updateServer(int row, Server* server, int response)
{
	QStandardItem* qstdItem;
	QStandardItem* itemPointer;

    itemPointer = item(row, SLCID_HIDDEN_SERVER_POINTER);
    // Save pointer to the column
	ServerPointer ptr(server);
	QVariant savePointer = qVariantFromValue(ptr);
	itemPointer->setData(savePointer, SLDT_POINTER_TO_SERVER_STRUCTURE);

	// Address is set no matter what, so it's set here.
	qstdItem = item(row, SLCID_ADDRESS);
	fillItem(qstdItem, server->address(), QString(server->address().toString() + ":" + QString::number(server->port())) );

	switch(response)
	{
		case Server::RESPONSE_BAD:
			setBad(row, server);
			break;

		case Server::RESPONSE_BANNED:
			setBanned(row, server);
			break;

		case Server::RESPONSE_GOOD:
		case Server::RESPONSE_WAIT:
			setGood(row, server);
			break;

		case Server::RESPONSE_TIMEOUT:
		    setTimeout(row, server);
			break;
	}

	return row;
}

void ServerListModel::setBad(int row, Server* server)
{
	QStandardItem* qstdItem;

	qstdItem = item(row, SLCID_PLAYERS);
	fillItem(qstdItem, -1);

	qstdItem = item(row, SLCID_PING);
	fillItem(qstdItem, 99999);

	qstdItem = item(row, SLCID_SERVERNAME);
	fillItem(qstdItem, tr("<ERROR>"));

	qstdItem = item(row, SLCID_IWAD);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_MAP);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_WADS);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_GAMETYPE);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_HIDDEN_GROUP);
	fillItem(qstdItem, SG_BAD);
}

void ServerListModel::setBanned(int row, Server* server)
{
	QStandardItem* qstdItem;

	qstdItem = item(row, SLCID_PLAYERS);
	fillItem(qstdItem, -1);

	qstdItem = item(row, SLCID_PING);
	fillItem(qstdItem, 99999);

	qstdItem = item(row, SLCID_SERVERNAME);
	fillItem(qstdItem, tr("You are banned from this server"));

	qstdItem = item(row, SLCID_IWAD);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_MAP);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_WADS);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_GAMETYPE);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_HIDDEN_GROUP);
	fillItem(qstdItem, SG_BANNED);
}

void ServerListModel::setGood(int row, Server* server)
{
	QStandardItem* qstdItem;
	QString strTmp;

	qstdItem = item(row, SLCID_PLAYERS);
	fillItem(qstdItem, server->numPlayers());

	qstdItem = item(row, SLCID_PING);
	fillItem(qstdItem, server->ping());

	qstdItem = item(row, SLCID_SERVERNAME);
	fillItem(qstdItem, server->name());

	qstdItem = item(row, SLCID_IWAD);
	fillItem(qstdItem, server->iwadName());

	qstdItem = item(row, SLCID_MAP);
	fillItem(qstdItem, server->map());

	strTmp = server->pwads().join(" ");
	qstdItem = item(row, SLCID_WADS);
	fillItem(qstdItem, strTmp);

	qstdItem = item(row, SLCID_GAMETYPE);
	fillItem(qstdItem, server->gameMode().name());

	qstdItem = item(row, SLCID_HIDDEN_GROUP);
	fillItem(qstdItem, SG_NORMAL);
}

void ServerListModel::setTimeout(int row, Server* server)
{
	QStandardItem* qstdItem;

	qstdItem = item(row, SLCID_PLAYERS);
	fillItem(qstdItem, -1);

	qstdItem = item(row, SLCID_PING);
	fillItem(qstdItem, 99999);

	qstdItem = item(row, SLCID_SERVERNAME);
	fillItem(qstdItem, tr("<NO RESPONSE>"));

	qstdItem = item(row, SLCID_IWAD);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_MAP);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_WADS);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_GAMETYPE);
	fillItem(qstdItem, "");

	qstdItem = item(row, SLCID_HIDDEN_GROUP);
	fillItem(qstdItem, SG_BAD);
}

void ServerListModel::setRefreshing(int row)
{
	Server* serv = serverFromList(row);
	serv->refresh();

	QStandardItem* qstdItem = item(row, SLCID_SERVERNAME);
	qstdItem->setText(tr("<REFRESHING>"));
}

QModelIndex ServerListModel::findServerOnTheList(const Server* server)
{
	if (server != NULL)
	{
		for (int i = 0; i < rowCount(); ++i)
		{
			QStandardItem* qstdItem = item(i, SLCID_HIDDEN_SERVER_POINTER);
			const Server* savedServ = serverFromList(qstdItem);
			if (server == savedServ)
			{
				QModelIndex index = indexFromItem(qstdItem);
				return index;
			}
		}
	}
	return QModelIndex();
}

Server* ServerListModel::serverFromList(int rowNum)
{
    QStandardItem* qstdItem = item(rowNum, SLCID_HIDDEN_SERVER_POINTER);
    return serverFromList(qstdItem);
}

Server* ServerListModel::serverFromList(const QModelIndex& index)
{
    QStandardItem* qstdItem = item(index.row(), SLCID_HIDDEN_SERVER_POINTER);
    return serverFromList(qstdItem);
}

Server* ServerListModel::serverFromList(const QStandardItem* item)
{
    QVariant pointer = qVariantFromValue(item->data(SLDT_POINTER_TO_SERVER_STRUCTURE));
    if (!pointer.isValid())
    {
        return NULL;
    }
    ServerPointer savedServ = qVariantValue<ServerPointer>(pointer);
    return savedServ.ptr;
}

void ServerListModel::sort(int column, Qt::SortOrder order)
{
	QList<int> groupEndList;

	if (rowCount() == 0)
		return;

	// Sort the groups first.
	int time = clock();
	for (int i = 0; i < rowCount(); ++i)
	{
		ServerGroup sg1 = serverGroup(i);
		for (int j = i + 1; j < rowCount(); ++j)
		{
			ServerGroup sg2 = serverGroup(j);
			if (sg2 > sg1)
			{
				swapRows(i, j);
				break;
			}
		}
	}

	// Save groups positions
	ServerGroup sg1 = serverGroup(0);
	for (int i = 1; i < rowCount(); ++i)
	{
		ServerGroup sg2 = serverGroup(i);
		if (sg2 != sg1)
		{
			groupEndList.append(i);
			sg1 = sg2;
		}
	}

	// Now sort groups internally
	QList<int>::iterator it;
	int index = 0;
	for (it = groupEndList.begin(); it != groupEndList.end(); ++it)
	{
		for (int i = index; i < *it; ++i)
		{
			QVariant var1 = columnSortData(i, column);
			int swap = i;
			for (int j = i + 1; j < *it; ++j)
			{
				QVariant var2 = columnSortData(j, column);
				int result = compareColumnSortData(var1, var2, column);
				if ((order == Qt::AscendingOrder && result > 0) || (order == Qt::DescendingOrder && result < 0))
				{
					var1 = var2;
					swap = j;
				}
			}

			// swapRows() checks whether arguments are equal and
			// does nothing if they are.
			swapRows(i, swap);

		}
		index = *it;
	}

	emit allRowsContentChanged();

	printf("Time: %d\n", clock() - time);
}

ServerListModel::ServerGroup ServerListModel::serverGroup(int row)
{
	QStandardItem* qstdItem = item(row, SLCID_HIDDEN_GROUP);
	return static_cast<ServerListModel::ServerGroup>(qstdItem->data(SLDT_SORT).toInt());
}

int	ServerListModel::compareColumnSortData(QVariant& var1, QVariant& var2, int column)
{
	int result = 0;
	switch(column)
	{
		case SLCID_ADDRESS:
		case SLCID_PING:
		case SLCID_PLAYERS:
			if (var1.toInt() < var2.toInt())
				result = -1;
			else if (var1.toInt() == var2.toInt())
				result = 0;
			else
				result = 1;
			break;

		case SLCID_GAMETYPE:
		case SLCID_IWAD:
		case SLCID_MAP:
		case SLCID_SERVERNAME:
		case SLCID_WADS:
			if (var1.toString() < var2.toString())
				result = -1;
			else if (var1.toString() == var2.toString())
				result = 0;
			else
				result = 1;
			break;
	}
	return result;
}

int	ServerListModel::compareColumnSortData(int row1, int row2, int column)
{
	QVariant var1 = item(row1, column)->data(SLDT_SORT);
	QVariant var2 = item(row2, column)->data(SLDT_SORT);

	return compareColumnSortData(var1, var2, column);
}

QVariant ServerListModel::columnSortData(int row, int column)
{
	QStandardItem* it = item(row, column);
	return it->data(SLDT_SORT);
}

inline void ServerListModel::swapRows(unsigned int row1, unsigned int row2)
{
	if (row1 == row2 || row1 >= rowCount() || row2 >= rowCount())
		return;

	Server* server1 = serverFromList(row1);
	Server* server2 = serverFromList(row2);

	if ( !(server1 == NULL || server2 == NULL) )
	{
		updateServer(row1, server2, server2->previousResponse());
		updateServer(row2, server1, server1->previousResponse());
	}

	emit rowContentChanged(row1);
	emit rowContentChanged(row2);
}

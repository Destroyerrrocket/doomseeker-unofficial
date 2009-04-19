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
/*
void ServerListModel::sort(int column, Qt::SortOrder order)
{

	if (rowCount() == 0)
		return;

	typedef QList< QList<QStandardItem*> > RowsList;

	QHash<ServerGroup, RowsList> map;
	int time = clock();
	for (int i = 0; i < rowCount(); ++i)
	{
		ServerGroup key = static_cast<ServerGroup>(item(i, SLCID_HIDDEN_GROUP)->data(SLDT_SORT).toInt());
		QList<QStandardItem*> row;
		for (int j = 0; j < columnCount(); ++j)
		{
			row.append(item(i, j));
		}

		QHash<ServerGroup, RowsList>::iterator it = map.find(key);
		if (it == map.end())
		{
			// This group is empty, create it and append current row.
			RowsList l;
			l.append(row);
			map.insert(key, l);
		}
		else
		{
			// This group is not empty, we need to find where we should insert the row
			// (in which ashtray put the ciggarete)
			RowsList::iterator listit;
			bool bInserted = false;
			QVariant cigarette = row[column]->data(SLDT_SORT);
			if (order == Qt::AscendingOrder)
			{
				for (listit = it.value().begin(); listit != it.value().end(); ++listit)
				{
					QList<QStandardItem*>& rowExisting = *listit;
					QVariant ashtray = rowExisting[column]->data(SLDT_SORT);
					int result = compareColumnSortData(cigarette, ashtray, column);
					if (result < 0)
					{
						it.value().insert(listit, row);
						bInserted = true;
						break;
					}
				}
			}
			else
			{
				for (listit = it.value().begin(); listit != it.value().end(); ++listit)
				{
					QList<QStandardItem*>& rowExisting = *listit;
					QVariant ashtray = rowExisting[column]->data(SLDT_SORT);
					int result = compareColumnSortData(cigarette, ashtray, column);
					if (result > 0)
					{
						it.value().insert(listit, row);
						bInserted = true;
						break;
					}
				}
			}

			// Row wasn't inserted, it means it should be on the end of the list,
			// append it there.
			if (!bInserted)
				it.value().append(row);
		}
	}

	// Now clear the table and rebuild it
	clearRows();

	QHash<ServerGroup, RowsList>::iterator mapit;
	for (mapit = map.begin(); mapit != map.end(); ++mapit)
	{
		RowsList& rowlist = mapit.value();
		RowsList::iterator listit;

		for (listit = rowlist.begin(); listit != rowlist.end(); ++listit)
		{
			QList<QStandardItem*>& row = *listit;
			appendRow(row);
		}
	}

	printf("Time: %d\n", clock() - time);
	emit allRowsContentChanged();
}
*/

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

	if (row1 > row2)
		qSwap(row1, row2);

	QList<QStandardItem*> it1 = takeRow(row1);
	QList<QStandardItem*> it2 = takeRow(row2 - 1);

	if ( !(it1.isEmpty() || it2.isEmpty()) )
	{
		insertRow(row1, it2);
		insertRow(row2, it1);
	}

	emit rowContentChanged(row1);
	emit rowContentChanged(row2);
}

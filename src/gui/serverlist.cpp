#include "gui/serverlist.h"

ServerListColumn SLCHandler::columns[] =
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


SLCHandler::SLCHandler(QTableView* tab)
{
	table = tab;
	prepareServerTable();
}

SLCHandler::~SLCHandler()
{

}
////////////////////////////////////////////////////
void SLCHandler::prepareServerTable()
{
	QStandardItemModel* model = new QStandardItemModel(this);

	QStringList labels;
	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		labels << columns[i].name;
	}
	model->setHorizontalHeaderLabels(labels);

	table->setModel(model);

	// Now set column widths
	for (int i = 0; i < HOW_MANY_SERVERLIST_COLUMNS; ++i)
	{
		table->setColumnWidth(i, columns[i].width);
	}
}
/////////////////////////////////////////////////////////
QModelIndex SLCHandler::findServerOnTheList(const Server* server)
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

void SLCHandler::addServer(const Server* server)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());
	QStandardItem* item;
	QString strTmp;
	QVariant varTmp;

	varTmp = server->numPlayers();
	QStandardItem* itemColZero = new QStandardItem("");
	itemColZero->setData(varTmp, Qt::DisplayRole);

	// Save pointer to the column
	ServerPointer ptr(server);
	QVariant savePointer = qVariantFromValue(ptr);
	itemColZero->setData(savePointer, SLDT_POINTER_TO_SERVER_STRUCTURE);

	// Append additional data to the rows
	QList<QStandardItem*> columns;
	columns.append(itemColZero);

	varTmp = server->ping();
	item = new QStandardItem();
	item->setData(varTmp, Qt::DisplayRole);
	columns.append(item);

	item = new QStandardItem(server->name());
	columns.append(item);

	item = new QStandardItem(server->address().toString());
	columns.append(item);

	item = new QStandardItem(server->iwadName());
	columns.append(item);

	item = new QStandardItem(server->map());
	columns.append(item);

	strTmp = server->pwads().join(" ");
	item = new QStandardItem(strTmp);
	columns.append(item);

	item = new QStandardItem(server->gameMode().name());
	columns.append(item);

	model->appendRow(columns);
}

void SLCHandler::updateServer(const QModelIndex&, const Server* server)
{

}

const Server* SLCHandler::serverFromList(int rowNum) const
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());

    QStandardItem* item = model->item(rowNum);
    return serverFromList(item);
}

const Server* SLCHandler::serverFromList(const QModelIndex& index) const
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());

    QStandardItem* item = model->item(index.row());
    return serverFromList(item);
}

const Server* SLCHandler::serverFromList(const QStandardItem* item) const
{
    QVariant pointer = qVariantFromValue(item->data(SLDT_POINTER_TO_SERVER_STRUCTURE));
    if (!pointer.isValid())
    {
        return NULL;
    }
    ServerPointer savedServ = qVariantValue<ServerPointer>(pointer);
    return savedServ.ptr;
}
//////////////////////////////////////////////////////////////
// Slots
void SLCHandler::serverUpdated(const Server *server)
{
	QModelIndex index = findServerOnTheList(server);
	if (index.isValid())
	{
		updateServer(index, server);
	}
	else
	{
		addServer(server);
	}
}

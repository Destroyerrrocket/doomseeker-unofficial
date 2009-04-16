#include "gui/serverlist.h"
#include <QHeaderView>

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

void SLCHandler::clearTable()
{
	if (table != NULL)
	{
		int rowCount = table->model()->rowCount();
		table->model()->removeRows(1, rowCount - 1);
	}
}
////////////////////////////////////////////////////
void SLCHandler::fillItem(QStandardItem* item, const QString& str)
{
	QString newStr = str.toLower();
	item->setData(str, Qt::DisplayRole);
	item->setData(newStr, SLDT_SORT);
}

void SLCHandler::fillItem(QStandardItem* item, int num)
{
	QVariant var = num;

	item->setData(var, Qt::DisplayRole);
	item->setData(var, SLDT_SORT);
}

void SLCHandler::fillItem(QStandardItem* item, const QHostAddress& addr)
{
	QVariant var = addr.toIPv4Address();

	item->setData(addr.toString(), Qt::DisplayRole);
	item->setData(var, SLDT_SORT);
}
////////////////////////////////////////////////////
void SLCHandler::prepareServerTable()
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
	//table->verticalHeader()->hide();

	QHeaderView* header = table->horizontalHeader();
	connect(header, SIGNAL( sectionClicked(int) ), this, SLOT ( columnHeaderClicked(int) ) );

	columnHeaderClicked(0);
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

	QStandardItem* itemColZero = new QStandardItem();
	fillItem(itemColZero, server->numPlayers());

	// Save pointer to the column
	ServerPointer ptr(server);
	QVariant savePointer = qVariantFromValue(ptr);
	itemColZero->setData(savePointer, SLDT_POINTER_TO_SERVER_STRUCTURE);

	// Append additional data to the rows
	QList<QStandardItem*> columns;
	columns.append(itemColZero);

	item = new QStandardItem();
	fillItem(item, server->ping());
	columns.append(item);

	item = new QStandardItem();
	fillItem(item, server->name());
	columns.append(item);

	item = new QStandardItem();
	fillItem(item, server->address());
	columns.append(item);

	item = new QStandardItem();
	fillItem(item, server->iwadName());
	columns.append(item);

	item = new QStandardItem();
	fillItem(item, server->map());
	columns.append(item);

	strTmp = server->pwads().join(" ");
	item = new QStandardItem();
	fillItem(item, strTmp);
	columns.append(item);

	item = new QStandardItem();
	fillItem(item, server->gameMode().name());
	columns.append(item);

	model->appendRow(columns);

	// Compress row size.
	table->resizeRowToContents(model->indexFromItem(columns[0]).row());
}

void SLCHandler::updateServer(const QModelIndex&, const Server* server)
{

}
//////////////////////////////////////////////////////////////
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

	if (sortIndex >= 0)
	{
		QStandardItemModel* model = static_cast<QStandardItemModel*>(table->model());
		model->sort(sortIndex, sortOrder);
	}
}

void SLCHandler::columnHeaderClicked(int index)
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

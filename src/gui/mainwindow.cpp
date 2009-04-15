#include "skulltag/skulltagmasterclient.h"

#include "gui/mainwindow.h"
#include "gui/configureDlg.h"
#include "gui/engineSkulltagConfig.h"
#include <QPointer>

MainWindow::MainWindow(int argc, char** argv)
{
	config = new Config();
	config->locateConfigFile(argc, argv);

	tester = new Tester();
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	setupUi(this);
	prepareServerTable();

	connect(btnRefresh, SIGNAL( clicked() ), this, SLOT( refresh() ));
	connect(menuActionConfigure, SIGNAL( triggered() ), this, SLOT( menuOptionsConfigure() ));
}

MainWindow::~MainWindow()
{
	delete tester;
	delete config;
}
/////////////////////////////////////////////////////////
void MainWindow::prepareServerTable()
{
	QStandardItemModel* model = new QStandardItemModel(this);

	QStringList labels;
	labels << tr("Players") << tr("Ping") << tr("Server Name") << tr("Address") << tr("IWAD") << tr("MAP") << tr("Wads") << tr("Gametype");
	model->setHorizontalHeaderLabels(labels);

	tableServers->setModel(model);
}
/////////////////////////////////////////////////////////
QModelIndex MainWindow::findServerOnTheList(const Server* server)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(tableServers->model());

	if (server != NULL)
	{
		for (int i = 0; i < model->rowCount(); ++i)
		{
			QStandardItem* item = model->item(i);
			QVariant pointer = qVariantFromValue(item->data(SLDT_POINTER_TO_SERVER_STRUCTURE));
			ServerPointer savedServ = qVariantValue<ServerPointer>(pointer);
			if (server == savedServ)
			{
				QModelIndex index = model->indexFromItem(item);
				return index;
			}
		}
	}
	return QModelIndex();
}

void MainWindow::addServer(const Server* server)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(tableServers->model());
	QStandardItem* item;
	QString tmp;

	tmp = QString::number(server->numPlayers());
	QStandardItem* itemColZero = new QStandardItem(tmp);

	// Save pointer to the column
	ServerPointer ptr(server);
	QVariant savePointer = qVariantFromValue(ptr);
	itemColZero->setData(savePointer, SLDT_POINTER_TO_SERVER_STRUCTURE);

	// Append additional data to the rows
	QList<QStandardItem*> columns;
	columns.append(itemColZero);

	tmp = QString::number(server->ping());
	item = new QStandardItem(tmp);
	columns.append(item);

	item = new QStandardItem(server->name());
	columns.append(item);

	item = new QStandardItem(server->address().toString());
	columns.append(item);

	item = new QStandardItem(server->iwadName());
	columns.append(item);

	item = new QStandardItem(server->map());
	columns.append(item);

	item = new QStandardItem("WADS");
	columns.append(item);

	item = new QStandardItem(server->gameMode().name());
	columns.append(item);

	model->appendRow(columns);
}

void MainWindow::updateServer(const QModelIndex&, const Server* server)
{

}
/////////////////////////////////////////////////////////
// Slots
void MainWindow::serverUpdated(const Server *server)
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

void MainWindow::refresh()
{
	SkulltagMasterClient mc(QHostAddress("91.121.87.67"), 15300);
	mc.refresh();

	if (mc.numServers() == 0)
	{
		return;
	}

	int rowCount = tableServers->model()->rowCount();
	tableServers->model()->removeRows(1, rowCount - 1);

	for(int i = 0;i < mc.numServers();i++)
	{
		//printf("%s:%d\n", mc[i]->getAddress().toString().toAscii().data(), mc[i]->getPort());
		Qt::ConnectionType conType = Qt::DirectConnection;
		//QObject::connect(mc[i], SIGNAL(updated(const Server *)), tester, SLOT(serverUpdated(const Server *)), conType);
		QObject::connect(mc[i], SIGNAL(updated(const Server *)), this, SLOT(serverUpdated(const Server *)) );
		mc[i]->refresh();
	}
}

void MainWindow::menuOptionsConfigure()
{
	ConfigureDlg dlg(config, this);

	EngineConfiguration* ec = EngineSkulltagConfigBox::createStructure(config, &dlg);
	dlg.addEngineConfiguration(ec);

	dlg.exec();
}

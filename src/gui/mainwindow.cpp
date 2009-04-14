#include "skulltag/skulltagmasterclient.h"

#include "gui/mainwindow.h"
#include "gui/configureDlg.h"
#include "gui/engineSkulltagConfig.h"

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

}

void MainWindow::addServer(const Server* server)
{
}

void MainWindow::updateServer(const QModelIndex&, const Server* server)
{
}
/////////////////////////////////////////////////////////
// Slots
void MainWindow::serverUpdated(const Server *server)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(tableServers->model());
	QStandardItem* item;
	item = new QStandardItem("lol");

	model->appendRow(item);
}

void MainWindow::refresh()
{
	SkulltagMasterClient mc(QHostAddress("91.121.87.67"), 15300);
	mc.refresh();

	printf("Servers: %d\n", mc.numServers());

	for(int i = 0;i < mc.numServers();i++)
	{
		//printf("%s:%d\n", mc[i]->getAddress().toString().toAscii().data(), mc[i]->getPort());
		Qt::ConnectionType conType = Qt::DirectConnection;
		QObject::connect(mc[i], SIGNAL(updated(const Server *)), tester, SLOT(serverUpdated(const Server *)), conType);
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

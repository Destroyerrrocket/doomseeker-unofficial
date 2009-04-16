#include "skulltag/skulltagmasterclient.h"

#include "gui/mainwindow.h"
#include "gui/configureDlg.h"
#include "gui/engineSkulltagConfig.h"
#include <QPointer>
#include <QHeaderView>

MainWindow::MainWindow(int argc, char** argv)
{
	config = new Config();
	config->locateConfigFile(argc, argv);

	tester = new Tester();
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	setupUi(this);

	serverTableHandler = new SLCHandler(tableServers);

	connect(btnRefresh, SIGNAL( clicked() ), this, SLOT( refresh() ));
	connect(menuActionConfigure, SIGNAL( triggered() ), this, SLOT( menuOptionsConfigure() ));
}

MainWindow::~MainWindow()
{
	delete tester;
	delete config;
}
/////////////////////////////////////////////////////////
// Slots

void MainWindow::checkRefreshFinished()
{
	// Probably not the best solution to the problem for now it will do.
	// Once we can get a progress bar in it would probably work better to be
	// attached to whenever that reaches 100%.
	if(Server::refresherThreadPool().activeThreadCount() <= Server::refresherThreadPool().maxThreadCount()/4)
		btnRefresh->setEnabled(true);
}

void MainWindow::refresh()
{
	SkulltagMasterClient mc(QHostAddress("91.121.87.67"), 15300);
	mc.refresh();

	if (mc.numServers() == 0)
	{
		return;
	}

	serverTableHandler->clearTable();

	for(int i = 0;i < mc.numServers();i++)
	{
		//printf("%s:%d\n", mc[i]->getAddress().toString().toAscii().data(), mc[i]->getPort());
		Qt::ConnectionType conType = Qt::DirectConnection;
		//QObject::connect(mc[i], SIGNAL(updated(const Server *)), tester, SLOT(serverUpdated(const Server *)), conType);
		QObject::connect(mc[i], SIGNAL(updated(const Server *)), serverTableHandler, SLOT(serverUpdated(const Server *)) );
		QObject::connect(mc[i], SIGNAL(updated(const Server *)), this, SLOT(checkRefreshFinished()));
		mc[i]->refresh();
	}

	// disable refresh.
	btnRefresh->setEnabled(false);
}

void MainWindow::menuOptionsConfigure()
{
	ConfigureDlg dlg(config, this);

	ConfigurationBoxInfo* ec = EngineSkulltagConfigBox::createStructure(config, &dlg);
	dlg.addEngineConfiguration(ec);

	dlg.exec();
}

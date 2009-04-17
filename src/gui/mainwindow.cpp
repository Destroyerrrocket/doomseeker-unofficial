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

	serverTableHandler = new SLHandler(tableServers);

	connect(btnRefresh, SIGNAL( clicked() ), this, SLOT( btnRefresh_Click() ));
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
	btnRefresh->setEnabled(true);
}

void MainWindow::btnRefresh_Click()
{
	MasterClient* mc = new SkulltagMasterClient(QHostAddress("91.121.87.67"), 15300);
	mc->refresh();

	if (mc->numServers() == 0)
	{
		return;
	}

	serverTableHandler->setMaster(mc);

	for(int i = 0;i < mc->numServers();i++)
	{
		QObject::connect((*mc)[i], SIGNAL(updated(Server *, int)), serverTableHandler, SLOT(serverUpdated(Server *, int)) );
		(*mc)[i]->refresh();
	}

	ServerRefresher* guardian = new ServerRefresher(NULL);
	connect(guardian, SIGNAL( allServersRefreshed() ), this, SLOT(checkRefreshFinished()) );
	guardian->startGuardian();

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

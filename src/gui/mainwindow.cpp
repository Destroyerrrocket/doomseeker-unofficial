#include "skulltag/skulltagmasterclient.h"

#include "gui/mainwindow.h"
#include "gui/configureDlg.h"
#include "gui/engineSkulltagConfig.h"
#include <QFileInfo>
#include <QProcess>
#include <QHeaderView>
#include <QMessageBox>

MainWindow::MainWindow(int argc, char** argv)
{
	config = new Config();
	config->locateConfigFile(argc, argv);

	this->setAttribute(Qt::WA_DeleteOnClose, true);
	setupUi(this);

	serverTableHandler = new SLHandler(tableServers);

	connect(btnGetServers, SIGNAL( clicked() ), this, SLOT( btnGetServers_Click() ));
	connect(btnRefreshAll, SIGNAL( clicked() ), serverTableHandler, SLOT( refreshAll() ));
	connect(menuActionConfigure, SIGNAL( triggered() ), this, SLOT( menuOptionsConfigure() ));
	connect(serverTableHandler, SIGNAL( serverDoubleClicked(const Server*) ), this, SLOT( runGame(const Server*) ) );
}

MainWindow::~MainWindow()
{
	delete config;
}
/////////////////////////////////////////////////////////
// Slots
void MainWindow::checkRefreshFinished()
{
	btnGetServers->setEnabled(true);
	btnRefreshAll->setEnabled(true);
}

void MainWindow::btnGetServers_Click()
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
	btnGetServers->setEnabled(false);
	btnRefreshAll->setEnabled(false);
}

void MainWindow::menuOptionsConfigure()
{
	ConfigureDlg dlg(config, this);

	ConfigurationBoxInfo* ec = EngineSkulltagConfigBox::createStructure(config, &dlg);
	dlg.addEngineConfiguration(ec);

	dlg.exec();
}

void MainWindow::runGame(const Server* server)
{
	SettingsData* setting = config->setting("SkullTagBinaryPath");
	QFileInfo fileinfo(setting->string());

	if (!fileinfo.exists() || fileinfo.isDir())
	{
		QMessageBox::critical(this, "Doomseeker - error", "File: " + fileinfo.absoluteFilePath() + "\ndoesn't exist or is a directory");
		return;
	}

	QStringList args;
	QProcess proc;
	if( !proc.startDetached(fileinfo.absoluteFilePath(), args, fileinfo.absolutePath()) )
	{
		QMessageBox::critical(this, "Doomseeker - error", "File: " + fileinfo.absoluteFilePath() + "\ncannot be run");
		return;
	}
}

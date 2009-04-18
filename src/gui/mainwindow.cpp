#include "skulltag/skulltagmasterclient.h"

#include "gui/aboutDlg.h"
#include "gui/mainwindow.h"
#include "gui/configureDlg.h"
#include "gui/engineSkulltagConfig.h"
#include "pathfinder.h"
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
	connect(menuActionAbout, SIGNAL( triggered() ), this, SLOT( menuHelpAbout() ));
	connect(menuActionConfigure, SIGNAL( triggered() ), this, SLOT( menuOptionsConfigure() ));
	connect(menuActionQuit, SIGNAL( triggered() ), this, SLOT( close() ));
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

void MainWindow::menuHelpAbout()
{
	AboutDlg dlg(this);
	dlg.exec();
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
	const QString errorCaption = tr("Doomseeker - error");
	SettingsData* setting = config->setting("SkullTagBinaryPath");
	if (setting->string().isEmpty())
	{
		QMessageBox::critical(this, errorCaption, tr("No executable specified for this engine."));
		return;
	}

	QFileInfo fileinfo(setting->string());

	if (!fileinfo.exists() || fileinfo.isDir())
	{
		QMessageBox::critical(this, errorCaption, tr("File: ") + fileinfo.absoluteFilePath() + tr("\ndoesn't exist or is a directory"));
		return;
	}

	PathFinder pf(config);
	QStringList args;
	QStringList missingPwads;

	// Connect
	args << "+connect" << QString(server->address().toString() + ":" + QString::number(server->port()));

	// Iwad
	QString iwad = pf.findWad(server->iwadName().toLower());
	args << "-iwad" << iwad;

	// Pwads
	if (server->numWads() != 0)
	{
		args << "-file";
	}

	for (int i = 0; i < server->numWads(); ++i)
	{
		QString pwad = pf.findWad(server->wad(i));
		if (pwad.isEmpty())
		{
			missingPwads << server->wad(i);
		}
		else
		{
			args << pwad;
		}
	}

	if (iwad.isEmpty() || !missingPwads.isEmpty())
	{
		const QString filesMissingCaption = tr("Doomseeker - files are missing");
		QString error = tr("Following files are missing:\n");

		if (iwad.isEmpty())
		{
			error += tr("IWAD: ") + server->iwadName().toLower() + "\n";
		}

		if (!missingPwads.isEmpty())
		{
			error += tr("PWADS: ") + missingPwads.join(" ");
		}

		QMessageBox::critical(this, filesMissingCaption, error);
		return;
	}

	printf("Starting: %s %s", fileinfo.absoluteFilePath().toAscii().constData(), args.join(" ").toAscii().constData());

	QProcess proc;
	if( !proc.startDetached(fileinfo.absoluteFilePath(), args, fileinfo.absolutePath()) )
	{
		QMessageBox::critical(this, errorCaption, tr("File: ") + fileinfo.absoluteFilePath() + tr("\ncannot be run"));
		return;
	}
}

//------------------------------------------------------------------------------
// createserverdialog.cpp
//------------------------------------------------------------------------------
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//
//------------------------------------------------------------------------------
// Copyright (C) 2009-2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "createserverdialog.h"

#include "configuration/doomseekerconfig.h"
#include "copytextdlg.h"
#include "gui/widgets/createserverdialogpage.h"
#include "datapaths.h"
#include "commongui.h"
#include "ini/ini.h"
#include "ini/settingsproviderqt.h"
#include "pathfinder/pathfinder.h"
#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"
#include "scanner.h"
#include "serverapi/exefile.h"
#include "serverapi/gameexeretriever.h"
#include "serverapi/gamecreateparams.h"
#include "serverapi/gamehost.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "apprunner.h"
#include "commandline.h"
#include "gamedemo.h"
#include "strings.h"

#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>
#include <QScopedPointer>
#include <QStandardItemModel>
#include <QTimer>

class CreateServerDialog::PrivData
{
	public:
		bool bSuppressMissingExeErrors;
		bool remoteGameSetup;
		QList<CreateServerDialogPage*> currentCustomPages;
		EnginePlugin *currentEngine;
		QList<DMFlagsTabWidget*> dmFlagsTabs;
};

const QString CreateServerDialog::TEMP_SERVER_CONFIG_FILENAME = "/tmpserver.ini";

CreateServerDialog::CreateServerDialog(QWidget* parent)
: QDialog(parent)
{
	d = new PrivData();
	// Have the console delete itself
	setAttribute(Qt::WA_DeleteOnClose);

	d->bSuppressMissingExeErrors = true;
	d->remoteGameSetup = false;
	d->currentEngine = NULL;

	setupUi(this);
	connect(btnBrowseExecutable, SIGNAL( clicked() ), this, SLOT ( btnBrowseExecutableClicked() ) );
	connect(btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect(btnCommandLine, SIGNAL( clicked() ), this, SLOT( btnCommandLineClicked() ) );
	connect(btnDefaultExecutable, SIGNAL( clicked() ), this, SLOT( btnDefaultExecutableClicked() ) );
	connect(btnIwadBrowse, SIGNAL( clicked() ), this, SLOT ( btnIwadBrowseClicked() ) );
	connect(btnLoad, SIGNAL( clicked() ), this, SLOT ( btnLoadClicked() ) );
	connect(btnPlayOffline, SIGNAL( clicked() ), this, SLOT ( btnPlayOfflineClicked() ) );

	connect(btnSave, SIGNAL( clicked() ), this, SLOT ( btnSaveClicked() ) );
	connect(btnStartServer, SIGNAL( clicked() ), this, SLOT( btnStartServerClicked() ) );

	connect(cboEngine, SIGNAL( currentIndexChanged(int) ), this, SLOT( cboEngineSelected(int) ) );
	connect(cboGamemode, SIGNAL( currentIndexChanged(int) ), this, SLOT( cboGamemodeSelected(int) ) );

	cboIwad->setEditable(true);

	initPrimary();
	d->bSuppressMissingExeErrors = false;

	// This is a crude solution to the problem where message boxes appear
	// before the actual Create Server dialog. We need to give some time
	// for the Dialog to appear. Unfortunately reimplementing
	// QDialog::showEvent() didn't work very well.
	QTimer::singleShot(1, this, SLOT(firstLoadConfigTimer()) );
}

CreateServerDialog::~CreateServerDialog()
{
	delete d;
}

void CreateServerDialog::addIwad(const QString& path)
{
	if (path.isEmpty())
	{
		return;
	}

	for (int i = 0; i < cboIwad->count(); ++i)
	{
		if (cboIwad->itemText(i).compare(path) == 0)
		{
			cboIwad->setCurrentIndex(i);
			return;
		}
	}

	QString cleanPath = Strings::normalizePath(path);

	cboIwad->addItem(cleanPath);
	cboIwad->setCurrentIndex(cboIwad->count() - 1);
}

void CreateServerDialog::btnBrowseExecutableClicked()
{
	QString dialogDir = gConfig.doomseeker.previousCreateServerExecDir;
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - Add file"), dialogDir);

	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		gConfig.doomseeker.previousCreateServerExecDir = fi.absolutePath();

		leExecutable->setText(fi.absoluteFilePath());
	}
}

void CreateServerDialog::btnCommandLineClicked()
{
	QString executable;
	QStringList args;
	if(commandLineArguments(executable, args))
	{
		// Lines below directly modify the passed values.
		CommandLine::escapeExecutable(executable);
		CommandLine::escapeArgs(args);

		CopyTextDlg ctd(executable + " " + args.join(" "), "Host server command line:", this);
		ctd.exec();
	}
}

void CreateServerDialog::btnDefaultExecutableClicked()
{
	Message message;
	leExecutable->setText(pathToServerExe(message));

	if (!message.isIgnore())
	{
		QMessageBox::critical(NULL, tr("Obtaining default server binary path."),
			message.contents(),QMessageBox::Ok, QMessageBox::Ok);
	}
}

void CreateServerDialog::btnIwadBrowseClicked()
{
	QString dialogDir = gConfig.doomseeker.previousCreateServerWadDir;
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - select IWAD"), dialogDir);

	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		gConfig.doomseeker.previousCreateServerWadDir = fi.absolutePath();

		addIwad(strFile);
	}
}

void CreateServerDialog::btnLoadClicked()
{
	QString dialogDir = gConfig.doomseeker.previousCreateServerConfigDir;
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - load server config"), dialogDir, tr("Config files (*.ini)"));

	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		gConfig.doomseeker.previousCreateServerConfigDir = fi.absolutePath();

		loadConfig(strFile);
	}
}

void CreateServerDialog::btnPlayOfflineClicked()
{
	runGame(true);
}

void CreateServerDialog::btnSaveClicked()
{
	QString dialogDir = gConfig.doomseeker.previousCreateServerConfigDir;
	QString strFile = QFileDialog::getSaveFileName(this, tr("Doomseeker - save server config"), dialogDir, tr("Config files (*.ini)"));
	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		gConfig.doomseeker.previousCreateServerConfigDir = fi.absolutePath();

		if (fi.suffix().isEmpty())
		{
			strFile += ".ini";
		}

		if (!saveConfig(strFile))
		{
			QMessageBox::critical(NULL, tr("Doomseeker - save server config"), tr("Unable to save server configuration!"));
		}
	}

}

void CreateServerDialog::btnStartServerClicked()
{
	if(!d->remoteGameSetup)
		runGame(false);
	else
		accept();
}

void CreateServerDialog::cboEngineSelected(int index)
{
	if (index >= 0)
	{
		unsigned enginePluginIndex = cboEngine->itemData(index).toUInt();
		if (enginePluginIndex < gPlugins->numPlugins())
		{
			initEngineSpecific(gPlugins->info(enginePluginIndex));
		}
	}
}

void CreateServerDialog::cboGamemodeSelected(int index)
{
	if (index >= 0)
	{
		const QList<GameMode>* gameModes = d->currentEngine->data()->gameModes;
		if (gameModes != NULL)
		{
			initGamemodeSpecific((*gameModes)[index]);
		}
	}
}

bool CreateServerDialog::commandLineArguments(QString &executable, QStringList &args)
{
	const QString errorCapt = tr("Doomseeker - create game");
	if (d->currentEngine == NULL)
	{
		QMessageBox::critical(NULL, errorCapt, tr("No engine selected"));
		return false;
	}

	GameCreateParams gameParams;
	if (createHostInfo(gameParams, false))
	{
		CommandLineInfo cli;
		QString error;

		GameHost* gameRunner = d->currentEngine->gameHost();
		Message message = gameRunner->createHostCommandLine(gameParams, cli);

		delete gameRunner;

		if (message.isError())
		{
			QMessageBox::critical(NULL, tr("Doomseeker - error"), message.contents());
			return false;
		}
		else
		{
			executable = cli.executable.absoluteFilePath();
			args = cli.args;
			return true;
		}
	}
	return false;
}

bool CreateServerDialog::createHostInfo(GameCreateParams& params, bool offline)
{
	// Since some operating systems have different offline and server binaries
	// We will see if they are playing offline and switch to the client
	// binary if the specified executable is the same as what is provided
	// as the server.
	Message message;
	QString offlineExePath = pathToOfflineExe(message);
	QString serverExePath = pathToServerExe(message);
	bool bIsLineEditPotiningToServerBinary = (leExecutable->text() == serverExePath);
	bool bShouldUseClientBinary = (offline || d->remoteGameSetup) && message.isIgnore() && bIsLineEditPotiningToServerBinary;

	params.setHostMode(offline ? GameCreateParams::Offline : GameCreateParams::Host);
	if (bShouldUseClientBinary)
	{
		params.setExecutablePath(offlineExePath);
	}
	else
	{
		params.setExecutablePath(leExecutable->text());
	}

	params.setIwadPath(cboIwad->currentText());
	params.setPwadsPaths(wadsPicker->filePaths());

	// DMFlags
	foreach(const DMFlagsTabWidget* p, d->dmFlagsTabs)
	{
		DMFlagsSection sec(p->section.name());
		for (int i = 0; i < p->section.count(); ++i)
		{
			if (p->checkBoxes[i]->isChecked())
			{
				sec.add(p->section[i]);
			}
		}
		params.dmFlags() << sec;
	}

	// Custom pages.
	QStringList customPagesParams;
	foreach (CreateServerDialogPage* page, d->currentCustomPages)
	{
		if (page->validate())
		{
			customPagesParams << page->generateGameRunParameters();
		}
		else
		{
			// Pages must take care of displaying their own error messages.
			tabWidget->setCurrentIndex(tabWidget->indexOf(page));
			return false;
		}
	}
	params.setCustomParameters(customPagesParams);

	// Custom parameters
	customParamsPanel->fillInParams(params);

	// Misc. page
	miscPanel->fillInParams(params);

	// Other
	rulesPanel->fillInParams(params);
	params.setBroadcastToLan(cbBroadcastToLAN->isChecked());
	params.setBroadcastToMaster(cbBroadcastToMaster->isChecked());
	params.setMap(leMap->text());
	params.setName(leServername->text());
	params.setPort(spinPort->isEnabled() ? spinPort->value() : 0);

	const QList<GameMode>* gameModes = d->currentEngine->data()->gameModes;
	if (gameModes != NULL)
	{
		foreach (const GameMode& mode, (*gameModes))
		{
			if (mode.name().compare(cboGamemode->currentText()) == 0)
			{
				params.setGameMode(mode);
				break;
			}
		}
	}

	createHostInfoDemoRecord(params, offline);
	return true;
}

void CreateServerDialog::createHostInfoDemoRecord(GameCreateParams& params, bool offline)
{
	if (gConfig.doomseeker.bRecordDemo && offline)
	{
		params.setDemoPath(GameDemo::mkDemoFullPath(GameDemo::Managed, *d->currentEngine));
		params.setDemoRecord(GameDemo::Managed);
	}
}

GameMode CreateServerDialog::currentGameMode() const
{
	const QList<GameMode>* gameModes = d->currentEngine->data()->gameModes;
	if (gameModes != NULL)
	{
		foreach (const GameMode& mode, (*gameModes))
		{
			if (mode.name().compare(cboGamemode->currentText()) == 0)
			{
				return mode;
			}
		}
	}
	return GameMode();
}

void CreateServerDialog::firstLoadConfigTimer()
{
	QString tmpServerCfgPath = gDefaultDataPaths->programsDataDirectoryPath() + TEMP_SERVER_CONFIG_FILENAME;

	QFileInfo fi(tmpServerCfgPath);
	if (fi.exists())
	{
		loadConfig(tmpServerCfgPath);
	}
}

void CreateServerDialog::initDMFlagsTabs()
{
	removeDMFlagsTabs();

	if (d->currentEngine->data()->createDMFlagsPagesAutomatic)
	{
		int paramsIndex = tabWidget->indexOf(tabCustomParameters);
		const QList<DMFlagsSection>* dmFlagsSec = d->currentEngine->data()->allDMFlags;
		if(dmFlagsSec == NULL || dmFlagsSec->empty())
		{
			return; // Nothing to do
		}

		const QList<DMFlagsSection>& dmFlagsSections = *dmFlagsSec;

		for (int i = 0; i < dmFlagsSections.count(); ++i)
		{
			DMFlagsTabWidget* dmftw = new DMFlagsTabWidget();

			QWidget* flagsTab = new QWidget(this);
			dmftw->widget = flagsTab;
			dmftw->section = dmFlagsSections[i];

			QHBoxLayout* hLayout = new QHBoxLayout(flagsTab);

			QVBoxLayout* layout = NULL;
			for (int j = 0; j < dmFlagsSections[i].count(); ++j)
			{
				if ((j % 16) == 0)
				{
					if (layout != NULL)
					{
						layout->addStretch();
					}

					layout = new QVBoxLayout();
					hLayout->addLayout(layout);
				}

				QCheckBox* checkBox = new QCheckBox();
				checkBox->setText(dmFlagsSections[i].name());
				dmftw->checkBoxes << checkBox;
				layout->addWidget(checkBox);
			}

			if (layout != NULL)
			{
				layout->addStretch();
			}

			d->dmFlagsTabs << dmftw;
			tabWidget->insertTab(paramsIndex++, flagsTab, dmFlagsSections[i].name());
		}
	}
}

void CreateServerDialog::initEngineSpecific(EnginePlugin* engineInfo)
{
	if (engineInfo == d->currentEngine || engineInfo == NULL)
	{
		return;
	}

	d->currentEngine = engineInfo;

	// Executable path
	Message message;

	if (d->remoteGameSetup)
	{
		// When we setup a remote game, we want to use a client
		// executable to connect to it.
		ServerPtr server = d->currentEngine->server(QHostAddress("127.0.0.1"), 1);
		leExecutable->setText(pathToClientExe(server.data(), message));
	}
	else
	{
		leExecutable->setText(pathToServerExe(message));
	}

	if (message.isError() && !d->bSuppressMissingExeErrors)
	{
		QString caption = tr("Doomseeker - error obtaining server binary");
		QString error = tr("Server binary for engine \"%1\" cannot be obtained.\nFollowing error has occured:\n%2").arg(engineInfo->data()->name, message.contents());

		QMessageBox::warning(NULL, caption, error);
	}

	spinPort->setValue(d->currentEngine->data()->defaultServerPort);

	cboGamemode->clear();

	const QList<GameMode>* gameModes = d->currentEngine->data()->gameModes;
	if (gameModes != NULL)
	{
		for (int i = 0; i < gameModes->count(); ++i)
		{
			cboGamemode->addItem((*gameModes)[i].name(), i);
		}
	}
	else
	{
		rulesPanel->setupForEngine(d->currentEngine, GameMode());
	}

	initDMFlagsTabs();
	initEngineSpecificPages(engineInfo);
	initInfoAndPassword();
	initRules();
}

void CreateServerDialog::initEngineSpecificPages(EnginePlugin* engineInfo)
{
	// First, get rid of the original pages.
	foreach (CreateServerDialogPage* page, d->currentCustomPages)
	{
		delete page;
	}
	d->currentCustomPages.clear();

	// Add new custom pages to the dialog.
	d->currentCustomPages = engineInfo->createServerDialogPages(this);
	foreach (CreateServerDialogPage* page, d->currentCustomPages)
	{
		int idxInsertAt = tabWidget->indexOf(tabCustomParameters);
		tabWidget->insertTab(idxInsertAt, page, page->name());
	}
}

void CreateServerDialog::initGamemodeSpecific(const GameMode& gameMode)
{
	rulesPanel->setupForEngine(d->currentEngine, gameMode);
}

void CreateServerDialog::initInfoAndPassword()
{
	miscPanel->setupForEngine(d->currentEngine);
	tabWidget->setTabEnabled(tabWidget->indexOf(tabMisc), miscPanel->isAnythingAvailable());
}

void CreateServerDialog::initPrimary()
{
	cboEngine->clear();

	for (unsigned i = 0; i < gPlugins->numPlugins(); ++i)
	{
		const EnginePlugin* plugin = gPlugins->info(i);
		cboEngine->addItem(plugin->icon(), plugin->data()->name, i);
	}

	if (cboEngine->count() > 0)
	{
		cboEngine->setCurrentIndex(0);
	}

	const QString iwads[] = { "doom.wad", "doom1.wad", "doom2.wad", "tnt.wad", "plutonia.wad", "heretic.wad", "hexen.wad", "hexdd.wad", "freedoom.wad", "strife1.wad", "" };

	cboIwad->clear();

	for (int i = 0; !iwads[i].isEmpty(); ++i)
	{
		PathFinder pathFinder;
		QString path = pathFinder.findFile(iwads[i]);
		if (!path.isEmpty())
		{
			cboIwad->addItem(path);
		}
	}
}

void CreateServerDialog::initRules()
{
	rulesPanel->setupForEngine(d->currentEngine, currentGameMode());
}

bool CreateServerDialog::loadConfig(const QString& filename)
{
	QAbstractItemModel* model;
	QStringList stringList;
	QSettings settingsFile(filename, QSettings::IniFormat);
	SettingsProviderQt settingsProvider(&settingsFile);
	Ini ini(&settingsProvider);
	IniSection general = ini.section("General");
	IniSection dmflags = ini.section("DMFlags");

	// General
	if (!d->remoteGameSetup)
	{
		QString engineName = general["engine"];
		const EnginePlugin* prevEngine = d->currentEngine;
		if(!setEngine(engineName))
			return false;

		bool bChangeExecutable = (prevEngine != d->currentEngine || !cbLockExecutable->isChecked());

		// First let's check if we can use executable stored in the server's config.
		// We will save the path to this executable in a local variable.
		QString executablePath = "";
		if (bChangeExecutable)
		{
			executablePath = *general["executable"];
			QFileInfo fileInfo(executablePath);
			if (!fileInfo.exists())
			{
				// Executable cannot be found, display error message and reset
				// the local variable.
				QMessageBox::warning(NULL, tr("Doomseeker - load server config"), tr("Game executable saved in config cannot be found.\nDefault executable will be used."));
				executablePath = "";
			}
		}

		// If we successfuly retrieved path from the config we shall
		// set this path in the line edit control.
		if (!executablePath.isEmpty())
		{
			leExecutable->setText(executablePath);
		}
	}

	leServername->setText(general["name"]);
	spinPort->setValue(general["port"]);
	cboGamemode->setCurrentIndex(general["gamemode"]);
	leMap->setText(general["map"]);
	addIwad(general["iwad"]);

	wadsPicker->setFilePaths(general["pwads"].valueString().split(";"));

	cbBroadcastToLAN->setChecked(general["broadcastToLAN"]);
	cbBroadcastToMaster->setChecked(general["broadcastToMaster"]);

	// Rules
	rulesPanel->loadConfig(ini);

	// Misc.
	miscPanel->loadConfig(ini);

	// DMFlags
	foreach(DMFlagsTabWidget* p, d->dmFlagsTabs)
	{
		for (int i = 0; i < p->section.count(); ++i)
		{
			QRegExp re("[^a-zA-Z]");
			QString name1 = p->section.name();
			QString name2 = p->section[i].name();
			name1 = name1.remove(re);
			name2 = name2.remove(re);
			p->checkBoxes[i]->setChecked(dmflags[name1 + name2]);
		}
	}

	// Custom pages.
	foreach (CreateServerDialogPage* page, d->currentCustomPages)
	{
		page->loadConfig(ini);
	}

	// Custom parameters
	customParamsPanel->loadConfig(ini);
	return true;
}

void CreateServerDialog::makeSetupRemoteGameDialog(const EnginePlugin *plugin)
{
	d->bSuppressMissingExeErrors = true;
	d->remoteGameSetup = true;
	setEngine(plugin->data()->name);

	cbAllowTheGameToChoosePort->hide();
	btnCommandLine->hide();

	// Disable some stuff
	QWidget *disableControls[] =
	{
		cboEngine, leExecutable, btnBrowseExecutable, btnDefaultExecutable,
		cbLockExecutable, leServername, spinPort, cbBroadcastToLAN,
		cbBroadcastToMaster, btnPlayOffline,

		NULL
	};
	for(int i = 0;disableControls[i] != NULL;++i)
		disableControls[i]->setDisabled(true);

	rulesPanel->setupForRemoteGame();
}

QString CreateServerDialog::pathToClientExe(Server* server, Message& message)
{
	QScopedPointer<ExeFile> f(server->clientExe());
	return f->pathToExe(message);
}

QString CreateServerDialog::pathToOfflineExe(Message& message)
{
	return GameExeRetriever(*d->currentEngine->gameExe()).pathToOfflineExe(message);
}

QString CreateServerDialog::pathToServerExe(Message& message)
{
	return GameExeRetriever(*d->currentEngine->gameExe()).pathToServerExe(message);
}

void CreateServerDialog::removeDMFlagsTabs()
{
	foreach (DMFlagsTabWidget* flags, d->dmFlagsTabs)
	{
		int index = tabWidget->indexOf(flags->widget);
		tabWidget->removeTab(index);
		delete flags->widget;
		delete flags;
	}

	d->dmFlagsTabs.clear();
}

void CreateServerDialog::runGame(bool offline)
{
	const QString errorCapt = tr("Doomseeker - create game");
	if (d->currentEngine == NULL)
	{
		QMessageBox::critical(NULL, errorCapt, tr("No engine selected"));
		return;
	}

	GameCreateParams gameParams;
	if (createHostInfo(gameParams, offline))
	{
		QString error;

		GameHost* gameRunner = d->currentEngine->gameHost();
		Message message = gameRunner->host(gameParams);

		delete gameRunner;

		if (message.isError())
		{
			QMessageBox::critical(NULL, tr("Doomseeker - error"), message.contents());
		}
		else
		{
			QString tmpServerConfigPath = gDefaultDataPaths->programsDataDirectoryPath() + TEMP_SERVER_CONFIG_FILENAME;
			saveConfig(tmpServerConfigPath);
		}
	}
}

bool CreateServerDialog::saveConfig(const QString& filename)
{
	QStringList stringList;
	QSettings settingsFile(filename, QSettings::IniFormat);
	SettingsProviderQt settingsProvider(&settingsFile);
	Ini ini(&settingsProvider);
	IniSection general = ini.createSection("General");
	IniSection dmflags = ini.createSection("DMFlags");

	// General
	general["engine"] = cboEngine->currentText();
	general["executable"] = leExecutable->text();
	general["name"] = leServername->text();
	general["port"] = spinPort->value();
	general["gamemode"] = cboGamemode->currentIndex();
	general["map"] = leMap->text();
	general["iwad"] = cboIwad->currentText();

	general["pwads"] = wadsPicker->filePaths().join(";");

	general["broadcastToLAN"] = cbBroadcastToLAN->isChecked();
	general["broadcastToMaster"] = cbBroadcastToMaster->isChecked();

	rulesPanel->saveConfig(ini);

	// Misc.
	miscPanel->saveConfig(ini);

	// DMFlags
	foreach(DMFlagsTabWidget* p, d->dmFlagsTabs)
	{
		for (int i = 0; i < p->section.count(); ++i)
		{
			QRegExp re("[^a-zA-Z]");
			QString name1 = p->section.name();
			QString name2 = p->section[i].name();
			name1 = name1.remove(re);
			name2 = name2.remove(re);
			dmflags[name1 + name2] = p->checkBoxes[i]->isChecked();
		}
	}

	// Custom pages.
	foreach (CreateServerDialogPage* page, d->currentCustomPages)
	{
		page->saveConfig(ini);
	}

	// Custom parameters
	customParamsPanel->saveConfig(ini);

	if (settingsFile.isWritable())
	{
		settingsFile.sync();
		return true;
	}
	return false;
}

bool CreateServerDialog::setEngine(const QString &engineName)
{
	int engIndex = gPlugins->pluginIndexFromName(engineName);
	if (engIndex < 0)
	{
		QMessageBox::critical(NULL, tr("Doomseeker - load server config"), tr("Plugin for engine \"%1\" is not present!").arg(engineName));
		return false;
	}

	// Select engine. This will also select the default executable path.
	cboEngine->setCurrentIndex(engIndex);
	return true;
}

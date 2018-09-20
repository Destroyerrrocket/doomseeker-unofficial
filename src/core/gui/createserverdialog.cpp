//------------------------------------------------------------------------------
// createserverdialog.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2009-2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "createserverdialog.h"
#include "ui_createserverdialog.h"

#include "configuration/doomseekerconfig.h"
#include "copytextdlg.h"
#include "gui/widgets/createserverdialogpage.h"
#include "gui/configuration/doomseekerconfigurationdialog.h"
#include "datapaths.h"
#include "ini/ini.h"
#include "ini/settingsproviderqt.h"
#include "plugins/engineplugin.h"
#include "serverapi/gamecreateparams.h"
#include "serverapi/gamehost.h"
#include "serverapi/message.h"
#include "apprunner.h"
#include "commandline.h"
#include "gamedemo.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

DClass<CreateServerDialog> : public Ui::CreateServerDialog
{
	public:
		bool remoteGameSetup;
		QList<CreateServerDialogPage*> currentCustomPages;
		EnginePlugin *currentEngine;
};

DPointered(CreateServerDialog)

const QString CreateServerDialog::TEMP_SERVER_CONFIG_FILENAME = "/tmpserver.ini";

CreateServerDialog::CreateServerDialog(QWidget* parent)
: QDialog(parent)
{
	// Have the console delete itself
	setAttribute(Qt::WA_DeleteOnClose);

	d->remoteGameSetup = false;
	d->currentEngine = NULL;

	d->setupUi(this);
	setConfigureButtonVisible(false);

	d->generalSetupPanel->setCreateServerDialog(this);
	d->rulesPanel->setCreateServerDialog(this);

	// This is a crude solution to the problem where message boxes appear
	// before the actual Create Server dialog. We need to give some time
	// for the Dialog to appear. Unfortunately reimplementing
	// QDialog::showEvent() didn't work very well.
	QTimer::singleShot(1, this, SLOT(firstLoadConfigTimer()) );
}

CreateServerDialog::~CreateServerDialog()
{
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

void CreateServerDialog::btnLoadClicked()
{
	QString dialogDir = gConfig.doomseeker.previousCreateServerConfigDir;
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - load server config"), dialogDir, tr("Config files (*.ini)"));

	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		gConfig.doomseeker.previousCreateServerConfigDir = fi.absolutePath();

		loadConfig(strFile, false);
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
	if (d->remoteGameSetup)
	{
		params.setHostMode(GameCreateParams::Remote);
	}
	else
	{
		params.setHostMode(offline ? GameCreateParams::Offline : GameCreateParams::Host);
	}
	d->generalSetupPanel->fillInParams(params);
	d->dmflagsPanel->fillInParams(params);

	if (!fillInParamsFromPluginPages(params))
	{
		return false;
	}

	d->customParamsPanel->fillInParams(params);
	d->miscPanel->fillInParams(params);
	d->rulesPanel->fillInParams(params);

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
	return d->generalSetupPanel->currentGameMode();
}

void CreateServerDialog::firstLoadConfigTimer()
{
	initEngineSpecific(d->generalSetupPanel->currentPlugin());
	QString tmpServerCfgPath = gDefaultDataPaths->programsDataDirectoryPath() + TEMP_SERVER_CONFIG_FILENAME;

	QFileInfo fi(tmpServerCfgPath);
	if (fi.exists())
	{
		loadConfig(tmpServerCfgPath, true);
	}
}

void CreateServerDialog::initDMFlagsTabs()
{
	bool flagsAdded = d->dmflagsPanel->initDMFlagsTabs(d->currentEngine);
	int tabIndex = d->tabWidget->indexOf(d->tabFlags);
	if (flagsAdded && tabIndex < 0)
	{
		d->tabWidget->insertTab(d->tabWidget->indexOf(d->tabCustomParameters), d->tabFlags, tr("Flags"));
	}
	else if (!flagsAdded && tabIndex >= 0)
	{
		d->tabWidget->removeTab(tabIndex);
	}
}

void CreateServerDialog::initEngineSpecific(EnginePlugin* engine)
{
	if (engine == d->currentEngine || engine == NULL)
	{
		return;
	}

	d->currentEngine = engine;

	d->generalSetupPanel->setupForEngine(engine);
	initDMFlagsTabs();
	initEngineSpecificPages(engine);
	initInfoAndPassword();
	initRules();
}

void CreateServerDialog::initEngineSpecificPages(EnginePlugin* engine)
{
	// First, get rid of the original pages.
	foreach (CreateServerDialogPage* page, d->currentCustomPages)
	{
		delete page;
	}
	d->currentCustomPages.clear();

	// Add new custom pages to the dialog.
	d->currentCustomPages = engine->createServerDialogPages(this);
	foreach (CreateServerDialogPage* page, d->currentCustomPages)
	{
		int idxInsertAt = d->tabWidget->indexOf(d->tabCustomParameters);
		d->tabWidget->insertTab(idxInsertAt, page, page->name());
	}
}

void CreateServerDialog::initGamemodeSpecific(const GameMode &gameMode)
{
	d->rulesPanel->setupForEngine(d->currentEngine, gameMode);
}

void CreateServerDialog::initInfoAndPassword()
{
	d->miscPanel->setupForEngine(d->currentEngine);
	d->tabWidget->setTabEnabled(d->tabWidget->indexOf(d->tabMisc), d->miscPanel->isAnythingAvailable());
}

void CreateServerDialog::initRules()
{
	d->rulesPanel->setupForEngine(d->currentEngine, currentGameMode());
}

bool CreateServerDialog::loadConfig(const QString& filename, bool loadingPrevious)
{
	QSettings settingsFile(filename, QSettings::IniFormat);
	SettingsProviderQt settingsProvider(&settingsFile);
	Ini ini(&settingsProvider);

	d->generalSetupPanel->loadConfig(ini, loadingPrevious);
	d->rulesPanel->loadConfig(ini);
	d->miscPanel->loadConfig(ini);
	d->dmflagsPanel->loadConfig(ini);

	// Custom pages.
	foreach (CreateServerDialogPage* page, d->currentCustomPages)
	{
		page->loadConfig(ini);
	}

	d->customParamsPanel->loadConfig(ini);
	return true;
}

void CreateServerDialog::makeRemoteGameSetupDialog(const EnginePlugin *plugin)
{
	d->remoteGameSetup = true;

	d->btnCommandLine->hide();
	d->btnPlayOffline->setDisabled(true);

	d->generalSetupPanel->setupForRemoteGame();
	d->rulesPanel->setupForRemoteGame();
}

MapListPanel* CreateServerDialog::mapListPanel()
{
	return d->rulesPanel->mapListPanel();
}

QString CreateServerDialog::mapName() const
{
	return d->generalSetupPanel->mapName();
}

bool CreateServerDialog::fillInParamsFromPluginPages(GameCreateParams &params)
{
	foreach (CreateServerDialogPage* page, d->currentCustomPages)
	{
		if (page->validate())
		{
			page->fillInGameCreateParams(params);
		}
		else
		{
			// Pages must take care of displaying their own error messages.
			d->tabWidget->setCurrentIndex(d->tabWidget->indexOf(page));
			return false;
		}
	}
	return true;
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
	QSettings settingsFile(filename, QSettings::IniFormat);
	SettingsProviderQt settingsProvider(&settingsFile);
	Ini ini(&settingsProvider);
	IniSection general = ini.section("General");

	d->generalSetupPanel->saveConfig(ini);
	d->rulesPanel->saveConfig(ini);
	d->miscPanel->saveConfig(ini);
	d->dmflagsPanel->saveConfig(ini);

	// Custom pages.
	foreach (CreateServerDialogPage* page, d->currentCustomPages)
	{
		page->saveConfig(ini);
	}

	d->customParamsPanel->saveConfig(ini);

	if (settingsFile.isWritable())
	{
		settingsFile.sync();
		return true;
	}
	return false;
}

void CreateServerDialog::setIwadByName(const QString &iwad)
{
	d->generalSetupPanel->setIwadByName(iwad);
}

void CreateServerDialog::setConfigureButtonVisible(bool visible)
{
	d->btnConfigure->setVisible(visible);
}

void CreateServerDialog::showConfiguration()
{
	DoomseekerConfigurationDialog::openConfiguration(this, d->currentEngine);
	d->generalSetupPanel->reloadAppConfig();
}

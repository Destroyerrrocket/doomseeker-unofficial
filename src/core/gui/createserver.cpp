//------------------------------------------------------------------------------
// createserver.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "createserver.h"
#include "configuration/doomseekerconfig.h"
#include "copytextdlg.h"
#include "main.h"
#include "commonGUI.h"
#include "ini/ini.h"
#include "plugins/engineplugin.h"
#include "sdeapi/config.hpp"
#include "serverapi/binaries.h"
#include "serverapi/gamerunner.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "strings.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>
#include <QStandardItemModel>
#include <QTimer>

const QString CreateServerDlg::TEMP_SERVER_CONFIG_FILENAME = "/tmpserver.ini";

CreateServerDlg::CreateServerDlg(QWidget* parent) : QDialog(parent)
{
	bSuppressMissingExeErrors = true;
	bIsServerSetup = false;
	currentEngine = NULL;

	setupUi(this);
	connect(btnAddMapToMaplist, SIGNAL( clicked() ), this, SLOT ( btnAddMapToMaplistClicked() ) );
	connect(btnAddPwad, SIGNAL( clicked() ), this, SLOT ( btnAddPwadClicked() ) );
	connect(btnBrowseExecutable, SIGNAL( clicked() ), this, SLOT ( btnBrowseExecutableClicked() ) );
	connect(btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect(btnCommandLine, SIGNAL( clicked() ), this, SLOT( btnCommandLineClicked() ) );
	connect(btnClearPwadList, SIGNAL( clicked() ), this, SLOT( btnClearPwadListClicked() ) );
	connect(btnDefaultExecutable, SIGNAL( clicked() ), this, SLOT( btnDefaultExecutableClicked() ) );
	connect(btnIwadBrowse, SIGNAL( clicked() ), this, SLOT ( btnIwadBrowseClicked() ) );
	connect(btnLoad, SIGNAL( clicked() ), this, SLOT ( btnLoadClicked() ) );
	connect(btnPlayOffline, SIGNAL( clicked() ), this, SLOT ( btnPlayOfflineClicked() ) );
	connect(btnRemoveMapFromMaplist, SIGNAL( clicked() ), this, SLOT ( btnRemoveMapFromMaplistClicked() ) );
	connect(btnRemovePwad, SIGNAL( clicked() ), this, SLOT ( btnRemovePwadClicked() ) );
	connect(btnSave, SIGNAL( clicked() ), this, SLOT ( btnSaveClicked() ) );
	connect(btnStartServer, SIGNAL( clicked() ), this, SLOT( btnStartServerClicked() ) );

	connect(cboEngine, SIGNAL( currentIndexChanged(int) ), this, SLOT( cboEngineSelected(int) ) );
	connect(cboGamemode, SIGNAL( currentIndexChanged(int) ), this, SLOT( cboGamemodeSelected(int) ) );
	
	connect(lstAdditionalFiles, SIGNAL( fileSystemPathDropped(const QString& ) ),
		this, SLOT( lstAdditionalFilesPathDnd(const QString&) ) );

	connect(QApplication::instance(), SIGNAL( focusChanged(QWidget*, QWidget*) ), this, SLOT( focusChanged(QWidget*, QWidget*) ));

	cboIwad->setEditable(true);
	lstAdditionalFiles->setModel(new QStandardItemModel(this));
	lstMaplist->setModel(new QStandardItemModel(this));

	initPrimary();
	bSuppressMissingExeErrors = false;

	// This is a crude solution to the problem where message boxes appear
	// before the actual Create Server dialog. We need to give some time
	// for the Dialog to appear. Unfortunately reimplementing
	// QDialog::showEvent() didn't work very well.
	QTimer::singleShot(1, this, SLOT(firstLoadConfigTimer()) );
}

CreateServerDlg::~CreateServerDlg()
{

}

void CreateServerDlg::addIwad(const QString& path)
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

void CreateServerDlg::addMapToMaplist(const QString& map)
{
	if (map.isEmpty())
	{
		return;
	}

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstMaplist->model());

	QStandardItem* it = new QStandardItem(map);

	it->setDragEnabled(true);
	it->setDropEnabled(false);

	model->appendRow(it);
}

void CreateServerDlg::addWadPath(const QString& strPath)
{
	if (strPath.isEmpty())
	{
		return;
	}

	QStandardItemModel* model = static_cast<QStandardItemModel*>(lstAdditionalFiles->model());

	// Check if this path exists already, if so - do nothing.
	for(int i = 0; i < model->rowCount(); ++i)
	{
		QStandardItem* item = model->item(i);
		QString dir = item->text();
		Qt::CaseSensitivity cs;

		#ifdef Q_OS_WIN32
		cs = Qt::CaseInsensitive;
		#else
		cs = Qt::CaseSensitive;
		#endif

		if (dir.compare(strPath, cs) == 0)
		{
			return;
		}
	}

	QStandardItem* it = new QStandardItem(strPath);

	it->setDragEnabled(true);
	it->setDropEnabled(false);
	it->setToolTip(strPath);

	model->appendRow(it);
}

void CreateServerDlg::btnAddMapToMaplistClicked()
{
	addMapToMaplist(leMapname->text());
}

void CreateServerDlg::btnAddPwadClicked()
{
	QString dialogDir = gConfig.doomseeker.previousCreateServerWadDir;
	QStringList filesNames = QFileDialog::getOpenFileNames(this, tr("Doomseeker - Add file(s)"), dialogDir);
	
	if (!filesNames.isEmpty())
	{
		// Remember the directory of the first file. This directory will be
		// restored the next time this dialog is opened.
		QFileInfo fi(filesNames[0]);
		gConfig.doomseeker.previousCreateServerWadDir = fi.absolutePath();	
		
		foreach (const QString& strFile, filesNames)
		{
			addWadPath(strFile);
		}
	}
}

void CreateServerDlg::btnBrowseExecutableClicked()
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

void CreateServerDlg::btnClearPwadListClicked()
{
	QStandardItemModel* pModel = (QStandardItemModel*)lstAdditionalFiles->model();
	pModel->clear();
}

void CreateServerDlg::btnCommandLineClicked()
{
	QString executable;
	QStringList args;
	if(commandLineArguments(executable, args))
	{
		CopyTextDlg ctd(executable + " " + args.join(" "), "Host server command line:", this);
		ctd.exec();
	}
}

void CreateServerDlg::btnDefaultExecutableClicked()
{
	QString error;

	// The info here doesn't really matter. We need to create a Server instance
	// because this is the only way to get Binaries instance.
	//
	// Explanation: such horrendous heresy wouldn't be necessary if not for
	// the Skulltag testing system which requires passing a SkulltagServer*
	// to SkulltagBinaries constructor. This renders getting Binaries through
	// plugin's interface a not recommended feature.
	Server* server = currentEngine->server(QHostAddress("127.0.0.1"), 0);
	Binaries* binaries = server->binaries();
	Message message;
	leExecutable->setText(binaries->serverBinary(message));

	if (!message.isIgnore())
	{
		QMessageBox::critical(NULL, tr("Obtaining default server binary path."), message.contents(), QMessageBox::Ok, QMessageBox::Ok);
	}

	delete binaries;
	delete server;
}

void CreateServerDlg::btnIwadBrowseClicked()
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

void CreateServerDlg::btnLoadClicked()
{
	QString dialogDir = gConfig.doomseeker.previousCreateServerConfigDir;
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - load server config"), dialogDir, tr("Config files (*.ini *.cfg)"));

	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		gConfig.doomseeker.previousCreateServerConfigDir = fi.absolutePath();

		if (fi.suffix().compare("cfg", Qt::CaseInsensitive) == 0)
			loadOldConfig(strFile);
		else
			loadConfig(strFile);
	}
}

void CreateServerDlg::btnPlayOfflineClicked()
{
	runGame(true);
}

void CreateServerDlg::btnRemoveMapFromMaplistClicked()
{
	CommonGUI::removeSelectedItemsFromStandardItemView(lstMaplist);
}

void CreateServerDlg::btnRemovePwadClicked()
{
	CommonGUI::removeSelectedItemsFromStandardItemView(lstAdditionalFiles);
}

void CreateServerDlg::btnSaveClicked()
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

void CreateServerDlg::btnStartServerClicked()
{
	if(!bIsServerSetup)
		runGame(false);
	else
		accept();
}

void CreateServerDlg::cboEngineSelected(int index)
{
	if (index >= 0)
	{
		unsigned enginePluginIndex = cboEngine->itemData(index).toUInt();
		if (enginePluginIndex < Main::enginePlugins->numPlugins())
		{
			const EnginePlugin* nfo = (*Main::enginePlugins)[enginePluginIndex]->info;

			initEngineSpecific(nfo);
		}
	}
}

void CreateServerDlg::cboGamemodeSelected(int index)
{
	if (index >= 0)
	{
		const QList<GameMode>* gameModes = currentEngine->data()->gameModes;
		if (gameModes != NULL)
		{
			initGamemodeSpecific((*gameModes)[index]);
		}
	}
}

bool CreateServerDlg::commandLineArguments(QString &executable, QStringList &args)
{
	const QString errorCapt = tr("Doomseeker - create server");
	if (currentEngine == NULL)
	{
		QMessageBox::critical(NULL, errorCapt, tr("No engine selected"));
		return false;
	}

	Server* server = currentEngine->server(QHostAddress(), spinPort->value());
	HostInfo hi;
	GameRunner::HostMode mode = bIsServerSetup ? GameRunner::OFFLINE : GameRunner::HOST;

	if (createHostInfo(hi, server, mode))
	{
		CommandLineInfo cli;
		QString error;

		GameRunner* gameRunner = server->gameRunner();
		Message message = gameRunner->createHostCommandLine(hi, cli, mode);

		delete server;
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
	else if (server != NULL)
	{
		delete server;
	}
	return false;
}

bool CreateServerDlg::createHostInfo(HostInfo& hostInfo, Server* server, bool offline)
{
	if (server != NULL)
	{
		// Since some operating systems have different client and server binaries
		// We will see if they are playing offline and switch to the client
		// binary if the specified executable is the same as what is provided
		// as the server.
		Binaries *binaries = server->binaries();
		Message message;
		QString client = binaries->clientBinary(message);

		bool bIsLineEditPotiningToServerBinary = (leExecutable->text() == binaries->serverBinary(message));
		bool bShouldUseClientBinary = (offline || bIsServerSetup) && message.isIgnore() && bIsLineEditPotiningToServerBinary;

		if(bShouldUseClientBinary)
		{
			hostInfo.executablePath = client;
		}
		else
		{
			hostInfo.executablePath = leExecutable->text();
		}

		hostInfo.iwadPath = cboIwad->currentText();
		hostInfo.pwadsPaths = CommonGUI::listViewStandardItemsToStringList(lstAdditionalFiles);

		// DMFlags
		foreach(const DMFlagsTabWidget* p, dmFlagsTabs)
		{
			DMFlagsSection* sec = new DMFlagsSection();
			sec->name = p->section->name;

			for (int i = 0; i < p->section->flags.count(); ++i)
			{
				if (p->checkBoxes[i]->isChecked())
				{
					sec->flags << p->section->flags[i];
				}
			}

			hostInfo.dmFlags << sec;
		}

		// limits
		foreach(GameLimitWidget* p, limitWidgets)
		{
			p->limit.setValue(p->spinBox->value());
			hostInfo.cvars << p->limit;
		}

		// modifier
		int modIndex = cboModifier->currentIndex();
		if (modIndex > 0) // Index zero is always "< NONE >"
		{
			--modIndex;
			gameModifiers[modIndex].setValue(1);
			hostInfo.cvars << gameModifiers[modIndex];
		}

		// Custom parameters
		hostInfo.customParameters = pteCustomParameters->toPlainText().split('\n');

		// Other
		server->setBroadcastToLAN(cbBroadcastToLAN->isChecked());
		server->setBroadcastToMaster(cbBroadcastToMaster->isChecked());
		server->setHostEmail(leEmail->text());
		server->setMap(leMap->text());
		server->setMapList(CommonGUI::listViewStandardItemsToStringList(lstMaplist));
		server->setRandomMapRotation(cbRandomMapRotation->isChecked());
		server->setMaximumClients(spinMaxClients->value());
		server->setMaximumPlayers(spinMaxPlayers->value());
		server->setMOTD(pteMOTD->toPlainText());
		server->setName(leServername->text());
		server->setPasswordConnect(leConnectPassword->text());
		server->setPasswordJoin(leJoinPassword->text());
		server->setPasswordRCon(leRConPassword->text());
		server->setPort(spinPort->value());
		server->setSkill(cboDifficulty->currentIndex());
		server->setWebsite(leURL->text());

		const QList<GameMode>* gameModes = currentEngine->data()->gameModes;
		if (gameModes != NULL)
		{
			for (int i = 0; i < gameModes->count(); ++i)
			{
				if ((*gameModes)[i].name().compare(cboGamemode->currentText()) == 0)
				{
					server->setGameMode((*gameModes)[i]);
					break;
				}
			}
		}

		return true;
	}

	return false;
}

void CreateServerDlg::firstLoadConfigTimer()
{
	QString tmpServerCfgPath = Main::dataPaths->programsDataDirectoryPath() + TEMP_SERVER_CONFIG_FILENAME;

	QFileInfo fi(tmpServerCfgPath);
	if (fi.exists())
	{
		loadConfig(tmpServerCfgPath);
	}
}


void CreateServerDlg::focusChanged(QWidget* oldW, QWidget* newW)
{
	if (newW == leMapname)
	{
		btnAddMapToMaplist->setDefault(true);
	}
	else if (oldW == leMapname)
	{
		btnAddMapToMaplist->setDefault(false);
	}
}

void CreateServerDlg::initDMFlagsTabs()
{
	removeDMFlagsTabs();

	int paramsIndex = tabWidget->indexOf(tabCustomParameters);
	const DMFlags* dmFlagsSec = currentEngine->data()->allDMFlags;
	if(dmFlagsSec == NULL || dmFlagsSec->empty())
	{
		return; // Nothing to do
	}

	const QList<DMFlagsSection*>& dmFlagsSections = *dmFlagsSec;

	for (int i = 0; i < dmFlagsSections.count(); ++i)
	{
		DMFlagsTabWidget* dmftw = new DMFlagsTabWidget();

		QWidget* flagsTab = new QWidget(this);
		dmftw->widget = flagsTab;
		dmftw->section = dmFlagsSections[i];

		QHBoxLayout* hLayout = new QHBoxLayout(flagsTab);

		QVBoxLayout* layout = NULL;
		for (int j = 0; j < dmFlagsSections[i]->flags.count(); ++j)
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
			checkBox->setText(dmFlagsSections[i]->flags[j].name);
			dmftw->checkBoxes << checkBox;
			layout->addWidget(checkBox);
		}

		if (layout != NULL)
		{
			layout->addStretch();
		}

		dmFlagsTabs << dmftw;
		tabWidget->insertTab(paramsIndex++, flagsTab, dmFlagsSections[i]->name);
	}
}

void CreateServerDlg::initEngineSpecific(const EnginePlugin* engineInfo)
{
	if (engineInfo == currentEngine || engineInfo == NULL)
	{
		return;
	}

	currentEngine = engineInfo;

	// Executable path
	Message message;

	// See: btnDefaultExecutableClicked()
	Server* server = currentEngine->server(QHostAddress("127.0.0.1"), 1);
	Binaries* binaries = server->binaries();
	if(bIsServerSetup)
		leExecutable->setText(binaries->clientBinary(message));
	else
		leExecutable->setText(binaries->serverBinary(message));

	if (message.isError() && !bSuppressMissingExeErrors)
	{
		QString caption = tr("Doomseeker - error obtaining server binary");
		QString error = tr("Server binary for engine \"%1\" cannot be obtained.\nFollowing error has occured:\n%2").arg(engineInfo->data()->name, message.contents());

		QMessageBox::warning(NULL, caption, error);
	}

	delete binaries;
	delete server;

	spinPort->setValue(currentEngine->data()->defaultServerPort);

	cboGamemode->clear();

	const QList<GameMode>* gameModes = currentEngine->data()->gameModes;
	if (gameModes != NULL)
	{
		for (int i = 0; i < gameModes->count(); ++i)
		{
			cboGamemode->addItem((*gameModes)[i].name(), i);
		}
	}

	initDMFlagsTabs();
	initInfoAndPassword();
	initRules();
}

void CreateServerDlg::initGamemodeSpecific(const GameMode& gameMode)
{
	// Rules tab
	removeLimitWidgets();
	QList<GameCVar> limits = currentEngine->limits(gameMode);
	QList<GameCVar>::iterator it;

	int number = 0;
	for (it = limits.begin(); it != limits.end(); ++it, ++number)
	{
		QLabel* label = new QLabel(this);
		label->setText(it->name);
		QSpinBox* spinBox = new QSpinBox(this);
		spinBox->setMaximum(999999);

		limitsLayout->addRow(label, spinBox);

		GameLimitWidget* glw = new GameLimitWidget();
		glw->label = label;
		glw->spinBox = spinBox;
		glw->limit = (*it);
		limitWidgets << glw;
	}
}

void CreateServerDlg::initInfoAndPassword()
{
	const static int MISC_TAB_INDEX = 2;

	bool bAtLeastOneVisible = false;
	bool bIsVisible = false;

	bIsVisible = currentEngine->data()->allowsConnectPassword;
	labelConnectPassword->setVisible(bIsVisible);
	leConnectPassword->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	bIsVisible = currentEngine->data()->allowsEmail;
	labelEmail->setVisible(bIsVisible);
	leEmail->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	bIsVisible = currentEngine->data()->allowsJoinPassword;
	labelJoinPassword->setVisible(bIsVisible);
	leJoinPassword->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	bIsVisible = currentEngine->data()->allowsMOTD;
	labelMOTD->setVisible(bIsVisible);
	pteMOTD->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	bIsVisible = currentEngine->data()->allowsRConPassword;
	labelRConPassword->setVisible(bIsVisible);
	leRConPassword->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	bIsVisible = currentEngine->data()->allowsURL;
	labelURL->setVisible(bIsVisible);
	leURL->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	tabWidget->setTabEnabled(MISC_TAB_INDEX, bAtLeastOneVisible);
}

void CreateServerDlg::initPrimary()
{
	cboEngine->clear();

	for (unsigned i = 0; i < Main::enginePlugins->numPlugins(); ++i)
	{
		const EnginePlugin* nfo = (*Main::enginePlugins)[i]->info;
		cboEngine->addItem(nfo->icon(), nfo->data()->name, i);
	}

	if (cboEngine->count() > 0)
	{
		cboEngine->setCurrentIndex(0);
	}

	cboDifficulty->clear();

	cboDifficulty->addItem("1 - I'm too young to die", 0);
	cboDifficulty->addItem("2 - Hey, not too rough", 1);
	cboDifficulty->addItem("3 - Hurt me plenty", 2);
	cboDifficulty->addItem("4 - Ultra-violence", 3);
	cboDifficulty->addItem("5 - NIGHTMARE!", 4);

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

void CreateServerDlg::initRules()
{
	cbRandomMapRotation->setVisible(currentEngine->data()->supportsRandomMapRotation);

	cboModifier->clear();
	gameModifiers.clear();

	const QList<GameCVar>* pEngineGameModifiers = currentEngine->data()->gameModifiers;

	if (pEngineGameModifiers != NULL && !pEngineGameModifiers->isEmpty())
	{
		const QList<GameCVar>& engineGameModifiers = *pEngineGameModifiers;

		cboModifier->show();
		labelModifiers->show();

		cboModifier->addItem(tr("< NONE >"));

		for (int i = 0; i < engineGameModifiers.count(); ++i)
		{
			cboModifier->addItem(engineGameModifiers[i].name);
			gameModifiers << engineGameModifiers[i];
		}
	}
	else
	{
		cboModifier->hide();
		labelModifiers->hide();
	}
}

bool CreateServerDlg::loadConfig(const QString& filename)
{
	QAbstractItemModel* model;
	QStringList stringList;
	Ini ini(filename);
	IniSection general = ini.section("General");
	IniSection rules = ini.section("Rules");
	IniSection misc = ini.section("Misc");
	IniSection dmflags = ini.section("DMFlags");

	// General
	if(!bIsServerSetup)
	{
		QString engineName = general["engine"];
		const EnginePlugin* prevEngine = currentEngine;
		if(!setEngine(engineName))
			return false;

		bool bChangeExecutable = (prevEngine != currentEngine || !cbLockExecutable->isChecked());

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

	stringList = general["pwads"].valueString().split(";");
	model = lstAdditionalFiles->model();
	model->removeRows(0, model->rowCount());
	foreach (QString s, stringList)
	{
		addWadPath(s);
	}

	cbBroadcastToLAN->setChecked(general["broadcastToLAN"]);
	cbBroadcastToMaster->setChecked(general["broadcastToMaster"]);

	// Rules
	cboDifficulty->setCurrentIndex(rules["difficulty"]);
	cboModifier->setCurrentIndex(rules["modifier"]);
	spinMaxClients->setValue(rules["maxClients"]);
	spinMaxPlayers->setValue(rules["maxPlayers"]);

	QList<GameLimitWidget*>::iterator it;
	for (it = limitWidgets.begin(); it != limitWidgets.end(); ++it)
	{
		(*it)->spinBox->setValue(rules[(*it)->limit.consoleCommand]);
	}

	stringList = rules["maplist"].valueString().split(";");
	model = lstMaplist->model();
	model->removeRows(0, model->rowCount());
	foreach(QString s, stringList)
	{
		addMapToMaplist(s);
	}
	cbRandomMapRotation->setChecked(rules["randomMapRotation"]);

	// Misc.
	leURL->setText(misc["URL"]);
	leEmail->setText(misc["eMail"]);
	leConnectPassword->setText(misc["connectPassword"]);
	leJoinPassword->setText(misc["joinPassword"]);
	leRConPassword->setText(misc["RConPassword"]);
	pteMOTD->document()->setPlainText(misc["MOTD"]);

	// DMFlags
	foreach(DMFlagsTabWidget* p, dmFlagsTabs)
	{
		for (int i = 0; i < p->section->flags.count(); ++i)
		{
			QRegExp re("[^a-zA-Z]");
			QString name1 = p->section->name;
			QString name2 = p->section->flags[i].name;
			name1 = name1.remove(re);
			name2 = name2.remove(re);
			p->checkBoxes[i]->setChecked(dmflags[name1 + name2]);
		}
	}

	// Custom parameters
	pteCustomParameters->document()->setPlainText(misc["CustomParams"]);
	return true;
}
// NOTE: Remove for 0.9
bool CreateServerDlg::loadOldConfig(const QString& filename)
{
	QAbstractItemModel* model;
	QStringList stringList;
	Config cfg(filename);

	// General
	if(!bIsServerSetup)
	{
		QString engineName = cfg.setting("engine")->string();
		const EnginePlugin* prevEngine = currentEngine;
		if(!setEngine(engineName))
			return false;

		bool bChangeExecutable = (prevEngine != currentEngine || !cbLockExecutable->isChecked());

		// First let's check if we can use executable stored in the server's config.
		// We will save the path to this executable in a local variable.
		QString executablePath = "";
		if (bChangeExecutable)
		{
			executablePath = cfg.setting("executable")->string();
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

	leServername->setText(cfg.setting("name")->string());
	spinPort->setValue(cfg.setting("port")->integer());
	cboGamemode->setCurrentIndex(cfg.setting("gamemode")->integer());
	leMap->setText(cfg.setting("map")->string());
	addIwad(cfg.setting("iwad")->string());

	stringList = cfg.setting("pwads")->string().split(";");
	model = lstAdditionalFiles->model();
	model->removeRows(0, model->rowCount());
	foreach (QString s, stringList)
	{
		addWadPath(s);
	}

	cbBroadcastToLAN->setChecked(cfg.setting("broadcastToLAN")->boolean());
	cbBroadcastToMaster->setChecked(cfg.setting("broadcastToMaster")->boolean());

	// Rules
	cboDifficulty->setCurrentIndex(cfg.setting("difficulty")->integer());
	cboModifier->setCurrentIndex(cfg.setting("modifier")->integer());
	spinMaxClients->setValue(cfg.setting("maxClients")->integer());
	spinMaxPlayers->setValue(cfg.setting("maxPlayers")->integer());

	QList<GameLimitWidget*>::iterator it;
	for (it = limitWidgets.begin(); it != limitWidgets.end(); ++it)
	{
		(*it)->spinBox->setValue(cfg.setting((*it)->limit.consoleCommand)->integer());
	}

	stringList = cfg.setting("maplist")->string().split(";");
	model = lstMaplist->model();
	model->removeRows(0, model->rowCount());
	foreach(QString s, stringList)
	{
		addMapToMaplist(s);
	}
	cbRandomMapRotation->setChecked(cfg.setting("randomMapRotation")->boolean());

	// Misc.
	leURL->setText(cfg.setting("URL")->string());
	leEmail->setText(cfg.setting("eMail")->string());
	leConnectPassword->setText(cfg.setting("connectPassword")->string());
	leJoinPassword->setText(cfg.setting("joinPassword")->string());
	leRConPassword->setText(cfg.setting("RConPassword")->string());
	pteMOTD->document()->setPlainText(cfg.setting("MOTD")->string());

	// DMFlags
	foreach(DMFlagsTabWidget* p, dmFlagsTabs)
	{
		for (int i = 0; i < p->section->flags.count(); ++i)
		{
			QRegExp re("[^a-zA-Z]");
			QString name1 = p->section->name;
			QString name2 = p->section->flags[i].name;
			name1 = name1.remove(re);
			name2 = name2.remove(re);
			bool b = cfg.setting(name1 + name2)->boolean();
			p->checkBoxes[i]->setChecked(b);
		}
	}

	// Custom parameters
	pteCustomParameters->document()->setPlainText(cfg.setting("CustomParams")->string());
	return true;
}

void CreateServerDlg::lstAdditionalFilesPathDnd(const QString& path)
{
	QFileInfo fileInfo(path);
	if (fileInfo.isFile())
	{
		addWadPath(path);
	}
}

void CreateServerDlg::makeSetupServerDialog(const EnginePlugin *plugin)
{
	bSuppressMissingExeErrors = true;
	bIsServerSetup = true;
	setEngine(plugin->data()->name);

	// Disable some stuff
	QWidget *disableControls[] =
	{
		cboEngine, leExecutable, btnBrowseExecutable, btnDefaultExecutable,
		cbLockExecutable, leServername, spinPort, cbBroadcastToLAN,
		cbBroadcastToMaster, btnPlayOffline, spinMaxClients, spinMaxPlayers,

		NULL
	};
	for(int i = 0;disableControls[i] != NULL;++i)
		disableControls[i]->setDisabled(true);
}

void CreateServerDlg::removeDMFlagsTabs()
{
	QList<DMFlagsTabWidget*>::iterator it;
	for (it = dmFlagsTabs.begin(); it != dmFlagsTabs.end(); ++it)
	{
		int index = tabWidget->indexOf((*it)->widget);
		tabWidget->removeTab(index);
		delete (*it)->widget;
		delete *it;
	}

	dmFlagsTabs.clear();
}

void CreateServerDlg::removeLimitWidgets()
{
	QList<GameLimitWidget*>::iterator it;

	for (it = limitWidgets.begin(); it != limitWidgets.end(); ++it)
	{
		delete (*it)->label;
		delete (*it)->spinBox;
		delete *it;
	}

	limitWidgets.clear();
}

void CreateServerDlg::runGame(bool offline)
{
	const QString errorCapt = tr("Doomseeker - create server");
	if (currentEngine == NULL)
	{
		QMessageBox::critical(NULL, errorCapt, tr("No engine selected"));
		return;
	}

	Server* server = currentEngine->server(QHostAddress(), spinPort->value());
	HostInfo hi;

	if (createHostInfo(hi, server, offline))
	{
		QString error;

		GameRunner* gameRunner = server->gameRunner();

		Message message = gameRunner->host(hi, offline ? GameRunner::OFFLINE : GameRunner::HOST);

		delete gameRunner;
		delete server;

		if (message.isError())
		{
			QMessageBox::critical(NULL, tr("Doomseeker - error"), message.contents());
		}
		else
		{
			QString tmpServerConfigPath = Main::dataPaths->programsDataDirectoryPath() + TEMP_SERVER_CONFIG_FILENAME;
			saveConfig(tmpServerConfigPath);
		}
	}
	else if (server != NULL)
	{
		delete server;
	}
}

bool CreateServerDlg::saveConfig(const QString& filename)
{
	QStringList stringList;
	Ini ini(filename);
	IniSection general = ini.createSection("General");
	IniSection rules = ini.createSection("Rules");
	IniSection misc = ini.createSection("Misc");
	IniSection dmflags = ini.createSection("DMFlags");

	// General
	general["engine"] = cboEngine->currentText();
	general["executable"] = leExecutable->text();
	general["name"] = leServername->text();
	general["port"] = spinPort->value();
	general["gamemode"] = cboGamemode->currentIndex();
	general["map"] = leMap->text();
	general["iwad"] = cboIwad->currentText();

	stringList = CommonGUI::listViewStandardItemsToStringList(lstAdditionalFiles);
	general["pwads"] = stringList.join(";");

	general["broadcastToLAN"] = cbBroadcastToLAN->isChecked();
	general["broadcastToMaster"] = cbBroadcastToMaster->isChecked();

	// Rules
	rules["difficulty"] = cboDifficulty->currentIndex();
	rules["modifier"] = cboModifier->currentIndex();
	rules["maxClients"] = spinMaxClients->value();
	rules["maxPlayers"] = spinMaxPlayers->value();

	QList<GameLimitWidget*>::iterator it;
	for (it = limitWidgets.begin(); it != limitWidgets.end(); ++it)
	{
		rules[(*it)->limit.consoleCommand] = (*it)->spinBox->value();
	}

	stringList = CommonGUI::listViewStandardItemsToStringList(lstMaplist);
	rules["maplist"] = stringList.join(";");
	rules["randomMapRotation"] = cbRandomMapRotation->isChecked();

	// Misc.
	misc["URL"] = leURL->text();
	misc["eMail"] = leEmail->text();
	misc["connectPassword"] = leConnectPassword->text();
	misc["joinPassword"] = leJoinPassword->text();
	misc["RConPassword"] = leRConPassword->text();
	misc["MOTD"] = pteMOTD->toPlainText();

	// DMFlags
	foreach(DMFlagsTabWidget* p, dmFlagsTabs)
	{
		for (int i = 0; i < p->section->flags.count(); ++i)
		{
			QRegExp re("[^a-zA-Z]");
			QString name1 = p->section->name;
			QString name2 = p->section->flags[i].name;
			name1 = name1.remove(re);
			name2 = name2.remove(re);
			dmflags[name1 + name2] = p->checkBoxes[i]->isChecked();
		}
	}

	// Custom parameters
	misc["CustomParams"] = pteCustomParameters->toPlainText();

	return ini.save();
}

bool CreateServerDlg::setEngine(const QString &engineName)
{
	int engIndex = Main::enginePlugins->pluginIndexFromName(engineName);
	if (engIndex < 0)
	{
		QMessageBox::critical(NULL, tr("Doomseeker - load server config"), tr("Plugin for engine \"%1\" is not present!").arg(engineName));
		return false;
	}

	// Select engine. This will also select the default executable path.
	cboEngine->setCurrentIndex(engIndex);
	return true;
}

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
#include "copytextdlg.h"
#include "main.h"
#include "commonGUI.h"
#include "serverapi/binaries.h"
#include "serverapi/gamerunner.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>

const QString CreateServerDlg::TEMP_SERVER_CONFIG_FILENAME = "/tmpserver.cfg";

CreateServerDlg::CreateServerDlg(QWidget* parent) : QDialog(parent)
{
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

	connect(QApplication::instance(), SIGNAL( focusChanged(QWidget*, QWidget*) ), this, SLOT( focusChanged(QWidget*, QWidget*) ));

	cboIwad->setEditable(true);
	lstAdditionalFiles->setModel(new QStandardItemModel(this));
	lstMaplist->setModel(new QStandardItemModel(this));

	initPrimary();
	
	QString tmpServerCfgPath = Main::dataPaths.programsDataDirectoryPath() + TEMP_SERVER_CONFIG_FILENAME;

	QFileInfo fi(tmpServerCfgPath);
	if (fi.exists())
	{
		loadConfig(tmpServerCfgPath);
	}
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

	cboIwad->addItem(path);
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
	QString dialogDir = Main::config->setting("PreviousCreateServerWadDir")->string();
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - Add file"), dialogDir);

	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		Main::config->setting("PreviousCreateServerWadDir")->setValue(fi.absolutePath());

		addWadPath(strFile);
	}
}

void CreateServerDlg::btnBrowseExecutableClicked()
{
	QString dialogDir = Main::config->setting("PreviousCreateServerExecDir")->string();
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - Add file"), dialogDir);

	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		Main::config->setting("PreviousCreateServerExecDir")->setValue(fi.absolutePath());

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
	const QString errorCapt = tr("Doomseeker - create server");
	if (currentEngine == NULL)
	{
		QMessageBox::critical(this, errorCapt, tr("No engine selected"));
		return;
	}

	Server* server = currentEngine->pInterface->server(QHostAddress(), spinPort->value());
	HostInfo hi;

	if (createHostInfo(hi, server, false))
	{
		CommandLineInfo cli;
		QString error;

		GameRunner* gameRunner = server->gameRunner();
		MessageResult result = gameRunner->createHostCommandLine(hi, cli, false);

		delete server;
		delete gameRunner;

		if (result.isError)
		{
			QMessageBox::critical(this, result.caption, result.message);
		}
		else
		{
			CopyTextDlg ctd(cli.executable.absoluteFilePath() + " " + cli.args.join(" "), "Host server command line:", this);
			ctd.exec();
		}
	}
	else if (server != NULL)
	{
		delete server;
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
	Server* server = currentEngine->pInterface->server(QHostAddress("127.0.0.1"), 0);
	Binaries* binaries = server->binaries();
	leExecutable->setText(binaries->serverBinary(error));

	if (!error.isNull())
	{
		QMessageBox::critical(this, tr("Obtaining default server binary path."), error, QMessageBox::Ok, QMessageBox::Ok);
	}

	delete binaries;
	delete server;
}

void CreateServerDlg::btnIwadBrowseClicked()
{
	QString dialogDir = Main::config->setting("PreviousCreateServerWadDir")->string();
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - select IWAD"), dialogDir);

	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		Main::config->setting("PreviousCreateServerWadDir")->setValue(fi.absolutePath());

		addIwad(strFile);
	}
}

void CreateServerDlg::btnLoadClicked()
{
	QString dialogDir = Main::config->setting("PreviousCreateServerConfigDir")->string();
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - load server config"), dialogDir, tr("Config files (*.cfg)"));

	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		Main::config->setting("PreviousCreateServerConfigDir")->setValue(fi.absolutePath());

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
	QString dialogDir = Main::config->setting("PreviousCreateServerConfigDir")->string();
	QString strFile = QFileDialog::getSaveFileName(this, tr("Doomseeker - save server config"), dialogDir, tr("Config files (*.cfg)"));
	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		Main::config->setting("PreviousCreateServerConfigDir")->setValue(fi.absolutePath());

		if (fi.suffix().isEmpty())
			strFile += ".cfg";

		if (!saveConfig(strFile))
		{
			QMessageBox::critical(this, tr("Doomseeker - save server config"), tr("Unable to save server configuration!"));
		}
	}

}

void CreateServerDlg::btnStartServerClicked()
{
	runGame(false);
}

void CreateServerDlg::cboEngineSelected(int index)
{
	if (index >= 0)
	{
		unsigned enginePluginIndex = cboEngine->itemData(index).toUInt();
		if (enginePluginIndex < Main::enginePlugins->numPlugins())
		{
			const PluginInfo* nfo = (*Main::enginePlugins)[enginePluginIndex]->info;

			initEngineSpecific(nfo);
		}
	}
}

void CreateServerDlg::cboGamemodeSelected(int index)
{
	if (index >= 0)
	{
		const QList<GameMode>* gameModes = currentEngine->pInterface->gameModes();
		if (gameModes != NULL)
		{
			initGamemodeSpecific((*gameModes)[index]);
		}
	}
}

bool CreateServerDlg::createHostInfo(HostInfo& hi, Server* server, bool offline)
{
	if (server != NULL)
	{
		// Since some operating systems have different client and server binaries
		// We will see if they are playing offline and switch to the client
		// binary if the specified executable is the same as what is provided
		// as the server.
		Binaries *binaries = server->binaries();
		QString error;
		QString client = binaries->clientBinary(error);
		if(offline && error.isEmpty() && leExecutable->text() == binaries->serverBinary(error))
			hi.executablePath = client;
		else
			hi.executablePath = leExecutable->text();

		hi.iwadPath = cboIwad->currentText();
		hi.pwadsPaths = CommonGUI::listViewStandardItemsToStringList(lstAdditionalFiles);

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

			hi.dmFlags << sec;
		}

		// limits
		foreach(GameLimitWidget* p, limitWidgets)
		{
			p->limit.setValue(p->spinBox->value());
			hi.cvars << p->limit;
		}

		// modifier
		int modIndex = cboModifier->currentIndex();
		if (modIndex > 0) // Index zero is always "< NONE >"
		{
			--modIndex;
			gameModifiers[modIndex].setValue(1);
			hi.cvars << gameModifiers[modIndex];
		}

		// Custom parameters
		hi.customParameters = pteCustomParameters->toPlainText().split('\n');

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

		const QList<GameMode>* gameModes = currentEngine->pInterface->gameModes();
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
	const DMFlags* dmFlagsSec = currentEngine->pInterface->allDMFlags();
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

void CreateServerDlg::initEngineSpecific(const PluginInfo* engineInfo)
{
	if (engineInfo == currentEngine || engineInfo == NULL)
	{
		return;
	}

	currentEngine = engineInfo;
	const EnginePlugin* engine = currentEngine->pInterface;

	// Executable path
	QString dummy;

	// See: btnDefaultExecutableClicked()
	Server* server = engine->server(QHostAddress("127.0.0.1"), 1);
	Binaries* binaries = server->binaries();
	leExecutable->setText(binaries->serverBinary(dummy));
	delete binaries;
	delete server;

	spinPort->setValue(engine->defaultServerPort());

	cboGamemode->clear();

	const QList<GameMode>* gameModes = engine->gameModes();
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
	QList<GameCVar> limits = currentEngine->pInterface->limits(gameMode);
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
	
	const EnginePlugin* engine = currentEngine->pInterface;
	
	bool bAtLeastOneVisible = false;
	bool bIsVisible = false;
	
	bIsVisible = engine->allowsConnectPassword();
	labelConnectPassword->setVisible(bIsVisible);
	leConnectPassword->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	bIsVisible = engine->allowsEmail();
	labelEmail->setVisible(bIsVisible);
	leEmail->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	bIsVisible = engine->allowsJoinPassword();
	labelJoinPassword->setVisible(bIsVisible);
	leJoinPassword->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	bIsVisible = engine->allowsMOTD();
	labelMOTD->setVisible(bIsVisible);
	pteMOTD->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	bIsVisible = engine->allowsRConPassword();
	labelRConPassword->setVisible(bIsVisible);
	leRConPassword->setVisible(bIsVisible);
	bAtLeastOneVisible = bAtLeastOneVisible || bIsVisible;

	bIsVisible = engine->allowsURL();
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
		const PluginInfo* nfo = (*Main::enginePlugins)[i]->info;
		cboEngine->addItem(nfo->pInterface->icon(), nfo->name, i);
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
		PathFinder pf(Main::config);
		QString path = pf.findFile(iwads[i]);
		if (!path.isEmpty())
			cboIwad->addItem(path);
	}
}

void CreateServerDlg::initRules()
{
	const EnginePlugin* engine = currentEngine->pInterface;

	cbRandomMapRotation->setVisible(engine->supportsRandomMapRotation());

	cboModifier->clear();
	gameModifiers.clear();

	const QList<GameCVar>* pEngineGameModifiers = engine->gameModifiers();

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
	Config cfg(filename);

	// General
	QString engineName = cfg.setting("engine")->string();
	int engIndex = Main::enginePlugins->pluginIndexFromName(engineName);
	if (engIndex < 0)
	{
		QMessageBox::critical(this, tr("Doomseeker - load server config"), tr("Plugin for engine \"%1\" is not present!").arg(engineName));
		return false;
	}

	const PluginInfo* prevEngine = currentEngine;

	cboEngine->setCurrentIndex(engIndex);

	if (prevEngine != currentEngine || !cbLockExecutable->isChecked())
	{
		leExecutable->setText(cfg.setting("executable")->string());
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
		QMessageBox::critical(this, errorCapt, tr("No engine selected"));
		return;
	}

	Server* server = currentEngine->pInterface->server(QHostAddress(), spinPort->value());
	HostInfo hi;

	if (createHostInfo(hi, server, offline))
	{
		QString error;

		GameRunner* gameRunner = server->gameRunner();

		MessageResult result = gameRunner->host(hi, offline);

		delete gameRunner;
		delete server;

		if (result.isError)
		{
			QMessageBox::critical(this, result.caption, result.message);
		}
		else
		{
			QString tmpServerConfigPath = Main::dataPaths.programsDataDirectoryPath() + TEMP_SERVER_CONFIG_FILENAME;
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
	Config cfg(filename, false);

	// General
	cfg.setting("engine")->setValue(cboEngine->currentText());
	cfg.setting("executable")->setValue(leExecutable->text());
	cfg.setting("name")->setValue(leServername->text());
	cfg.setting("port")->setValue(spinPort->value());
	cfg.setting("gamemode")->setValue(cboGamemode->currentIndex());
	cfg.setting("map")->setValue(leMap->text());
	cfg.setting("iwad")->setValue(cboIwad->currentText());

	stringList = CommonGUI::listViewStandardItemsToStringList(lstAdditionalFiles);
	cfg.setting("pwads")->setValue(stringList.join(";"));

	cfg.setting("broadcastToLAN")->setValue(cbBroadcastToLAN->isChecked());
	cfg.setting("broadcastToMaster")->setValue(cbBroadcastToMaster->isChecked());

	// Rules
	cfg.setting("difficulty")->setValue(cboDifficulty->currentIndex());
	cfg.setting("modifier")->setValue(cboModifier->currentIndex());
	cfg.setting("maxClients")->setValue(spinMaxClients->value());
	cfg.setting("maxPlayers")->setValue(spinMaxPlayers->value());

	QList<GameLimitWidget*>::iterator it;
	for (it = limitWidgets.begin(); it != limitWidgets.end(); ++it)
	{
		cfg.setting((*it)->limit.consoleCommand)->setValue((*it)->spinBox->value());
	}

	stringList = CommonGUI::listViewStandardItemsToStringList(lstMaplist);
	cfg.setting("maplist")->setValue(stringList.join(";"));
	cfg.setting("randomMapRotation")->setValue(cbRandomMapRotation->isChecked());

	// Misc.
	cfg.setting("URL")->setValue(leURL->text());
	cfg.setting("eMail")->setValue(leEmail->text());
	cfg.setting("connectPassword")->setValue(leConnectPassword->text());
	cfg.setting("joinPassword")->setValue(leJoinPassword->text());
	cfg.setting("RConPassword")->setValue(leRConPassword->text());
	cfg.setting("MOTD")->setValue(pteMOTD->toPlainText());

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
			cfg.setting(name1 + name2)->setValue(p->checkBoxes[i]->isChecked());
		}
	}

	// Custom parameters
	cfg.setting("CustomParams")->setValue(pteCustomParameters->toPlainText());

	return cfg.saveConfig();
}

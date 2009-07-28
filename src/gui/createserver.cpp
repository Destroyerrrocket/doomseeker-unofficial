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
#include "main.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>

CreateServerDlg::CreateServerDlg(QWidget* parent) : QDialog(parent)
{
	currentEngine = NULL;

	setupUi(this);
	connect(btnAddMapToMaplist, SIGNAL( clicked() ), this, SLOT ( btnAddMapToMaplistClicked() ) );
	connect(btnAddPwad, SIGNAL( clicked() ), this, SLOT ( btnAddPwadClicked() ) );
	connect(btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect(btnIwadBrowse, SIGNAL( clicked() ), this, SLOT ( btnIwadBrowseClicked() ) );
	connect(btnLoad, SIGNAL( clicked() ), this, SLOT ( btnLoadClicked() ) );
	connect(btnRemoveMapFromMaplist, SIGNAL( clicked() ), this, SLOT ( btnRemoveMapFromMaplistClicked() ) );
	connect(btnRemovePwad, SIGNAL( clicked() ), this, SLOT ( btnRemovePwadClicked() ) );
	connect(btnSave, SIGNAL( clicked() ), this, SLOT ( btnSaveClicked() ) );
	connect(btnStartServer, SIGNAL( clicked() ), this, SLOT( accept() ) );

	connect(cboEngine, SIGNAL( currentIndexChanged(int) ), this, SLOT( cboEngineSelected(int) ) );
	connect(cboGamemode, SIGNAL( currentIndexChanged(int) ), this, SLOT( cboGamemodeSelected(int) ) );

	cboIwad->setEditable(true);
	lstAdditionalFiles->setModel(new QStandardItemModel(this));
	lstMaplist->setModel(new QStandardItemModel(this));

	initPrimary();
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

		#ifdef Q_WS_WIN
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
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - Add file"));
	addWadPath(strFile);
}

void CreateServerDlg::btnIwadBrowseClicked()
{
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - select IWAD"));
	addIwad(strFile);
}

void CreateServerDlg::btnLoadClicked()
{
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - load server config"), QString(), tr("Config files (*.cfg)"));
	if (!strFile.isEmpty())
	{
		QAbstractItemModel* model;
		QStringList stringList;
		Config cfg(strFile);

		// General
		QString engineName = cfg.setting("engine")->string();
		int engIndex = Main::enginePlugins.pluginIndexFromName(engineName);
		if (engIndex < 0)
		{
			QMessageBox::critical(this, tr("Doomseeker - load server config"), tr("Plugin for engine \"%s\" is not present!").arg(engineName));
			return;
		}

		cboEngine->setCurrentIndex(engIndex);
		leServername->setText(cfg.setting("name")->string());
		spinPort->setValue(cfg.setting("port")->integer());
		cboGamemode->setCurrentIndex(cfg.setting("gamemode")->integer());
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
		cboModifiers->setCurrentIndex(cfg.setting("modifier")->integer());
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
			for (int i = 0; i < p->section->size; ++i)
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
	}
}

void CreateServerDlg::btnRemoveMapFromMaplistClicked()
{
	Main::removeSelectionFromStandardItemView(lstMaplist);
}

void CreateServerDlg::btnRemovePwadClicked()
{
	Main::removeSelectionFromStandardItemView(lstAdditionalFiles);
}

void CreateServerDlg::btnSaveClicked()
{
	QString strFile = QFileDialog::getSaveFileName(this, tr("Doomseeker - save server config"), QString(), tr("Config files (*.cfg)"));
	if (!strFile.isEmpty())
	{
		QFileInfo fi(strFile);
		if (fi.suffix().isEmpty())
			strFile += ".cfg";

		QStringList stringList;
		Config cfg(strFile, false);

		// General
		cfg.setting("engine")->setValue(cboEngine->currentText());
		cfg.setting("name")->setValue(leServername->text());
		cfg.setting("port")->setValue(spinPort->value());
		cfg.setting("gamemode")->setValue(cboGamemode->currentIndex());
		cfg.setting("iwad")->setValue(cboIwad->currentText());

		stringList = Main::listViewStandardItemsToStringList(lstAdditionalFiles);
		cfg.setting("pwads")->setValue(stringList.join(";"));

		cfg.setting("broadcastToLAN")->setValue(cbBroadcastToLAN->isChecked());
		cfg.setting("broadcastToMaster")->setValue(cbBroadcastToMaster->isChecked());

		// Rules
		cfg.setting("difficulty")->setValue(cboDifficulty->currentIndex());
		cfg.setting("modifier")->setValue(cboModifiers->currentIndex());
		cfg.setting("maxClients")->setValue(spinMaxClients->value());
		cfg.setting("maxPlayers")->setValue(spinMaxPlayers->value());

		QList<GameLimitWidget*>::iterator it;
		for (it = limitWidgets.begin(); it != limitWidgets.end(); ++it)
		{
			cfg.setting((*it)->limit.consoleCommand)->setValue((*it)->spinBox->value());
		}

		stringList = Main::listViewStandardItemsToStringList(lstMaplist);
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
			for (int i = 0; i < p->section->size; ++i)
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

		if (!cfg.saveConfig())
		{
			QMessageBox::critical(this, tr("Doomseeker - save server config"), tr("Unable to save server configuration!"));
		}
	}

}

void CreateServerDlg::cboEngineSelected(int index)
{
	if (index >= 0)
	{
		int enginePluginIndex = cboEngine->itemData(index).toInt();
		if (enginePluginIndex < Main::enginePlugins.numPlugins())
		{
			const PluginInfo* nfo = Main::enginePlugins[enginePluginIndex]->info;

			initEngineSpecific(nfo);
		}
	}
}

void CreateServerDlg::cboGamemodeSelected(int index)
{
	if (index >= 0)
	{
		const GameMode* gameModes = currentEngine->pInterface->generalEngineInfo().gameModes;
		initGamemodeSpecific(gameModes[index]);
	}
}

void CreateServerDlg::initDMFlagsTabs()
{
	removeDMFlagsTabs();

	int paramsIndex = tabWidget->indexOf(tabCustomParameters);
	const GeneralEngineInfo& engNfo = currentEngine->pInterface->generalEngineInfo();
	const DMFlagsSection* dmFlagsSec = engNfo.allDMFlags;
	for (int i = 0; i < engNfo.dmFlagsSectionsNum; ++i)
	{
		DMFlagsTabWidget* dmftw = new DMFlagsTabWidget();

		QWidget* flagsTab = new QWidget(this);
		dmftw->widget = flagsTab;
		dmftw->section = &dmFlagsSec[i];

		QHBoxLayout* hLayout = new QHBoxLayout(flagsTab);

		QVBoxLayout* layout = NULL;
		for (int j = 0; j < dmFlagsSec[i].size; ++j)
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
			checkBox->setText(dmFlagsSec[i].flags[j].name);
			dmftw->checkBoxes << checkBox;
			layout->addWidget(checkBox);
		}

		if (layout != NULL)
		{
			layout->addStretch();
		}

		dmFlagsTabs << dmftw;
		tabWidget->insertTab(paramsIndex++, flagsTab, dmFlagsSec[i].name);
	}
}

void CreateServerDlg::initEngineSpecific(const PluginInfo* engineInfo)
{
	if (engineInfo == currentEngine || engineInfo == NULL)
		return;

	currentEngine = engineInfo;
	const GeneralEngineInfo& engNfo = currentEngine->pInterface->generalEngineInfo();

	spinPort->setValue(engNfo.defaultServerPort);

	cboGamemode->clear();

	const GameMode* gameModes = engNfo.gameModes;
	for (int i = 0; i < engNfo.gameModesNum; ++i)
	{
		cboGamemode->addItem(gameModes[i].name(), i);
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
	const GeneralEngineInfo& engNfo = currentEngine->pInterface->generalEngineInfo();

	labelConnectPassword->setVisible(engNfo.allowsConnectPassword);
	leConnectPassword->setVisible(engNfo.allowsConnectPassword);

	labelEmail->setVisible(engNfo.allowsEmail);
	leEmail->setVisible(engNfo.allowsEmail);

	labelJoinPassword->setVisible(engNfo.allowsJoinPassword);
	leJoinPassword->setVisible(engNfo.allowsJoinPassword);

	labelMOTD->setVisible(engNfo.allowsMOTD);
	pteMOTD->setVisible(engNfo.allowsMOTD);

	labelRConPassword->setVisible(engNfo.allowsRConPassword);
	leRConPassword->setVisible(engNfo.allowsRConPassword);

	labelURL->setVisible(engNfo.allowsURL);
	leURL->setVisible(engNfo.allowsURL);
}

void CreateServerDlg::initPrimary()
{
	cboEngine->clear();

	for (int i = 0; i < Main::enginePlugins.numPlugins(); ++i)
	{
		const PluginInfo* nfo = Main::enginePlugins[i]->info;
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
		QString path = pf.findWad(iwads[i]);
		if (!path.isEmpty())
			cboIwad->addItem(path);
	}
}

void CreateServerDlg::initRules()
{
	const GeneralEngineInfo& engNfo = currentEngine->pInterface->generalEngineInfo();

	cbRandomMapRotation->setVisible(engNfo.supportsRandomMapRotation);

	cboModifiers->clear();
	gameModifiers.clear();
	if (engNfo.gameModifiers != NULL)
	{
		cboModifiers->show();
		labelModifiers->show();
		for (int i = 0; i < engNfo.gameModifiersNum; ++i)
		{
			GameModifierEntry gme = {i, engNfo.gameModifiers[i]};
			cboModifiers->addItem(engNfo.gameModifiers[i].name);
			gameModifiers << gme;
		}
	}
	else
	{
		cboModifiers->hide();
		labelModifiers->hide();
	}
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

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
#include <QObject>

CreateServerDlg::CreateServerDlg(QWidget* parent) : QDialog(parent)
{
	currentEngine = NULL;

	setupUi(this);
	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT ( btnClicked(QAbstractButton *) ));
	connect(btnAddMapToMaplist, SIGNAL( clicked() ), this, SLOT ( btnAddMapToMaplistClicked() ) );
	connect(btnAddPwad, SIGNAL( clicked() ), this, SLOT ( btnAddPwadClicked() ) );
	connect(btnIwadBrowse, SIGNAL( clicked() ), this, SLOT ( btnIwadBrowseClicked() ) );
	connect(btnRemoveMapFromMaplist, SIGNAL( clicked() ), this, SLOT ( btnRemoveMapFromMaplistClicked() ) );
	connect(btnRemovePwad, SIGNAL( clicked() ), this, SLOT ( btnRemovePwadClicked() ) );

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

void CreateServerDlg::btnClicked(QAbstractButton *button)
{
	// Figure out what button we pressed and perform its action.
	switch(buttonBox->standardButton(button))
	{
		default:
			break;
		case QDialogButtonBox::Ok:
			this->accept();
			break;

		case QDialogButtonBox::Cancel:
			this->reject();
			break;
	}
}

void CreateServerDlg::btnIwadBrowseClicked()
{
	QString strFile = QFileDialog::getOpenFileName(this, tr("Doomseeker - select IWAD"));
	if (!strFile.isEmpty())
	{
		cboIwad->addItem(strFile);
		cboIwad->setCurrentIndex(cboIwad->count() - 1);
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
	QList<GameLimit> limits = currentEngine->pInterface->limits(gameMode);
	QList<GameLimit>::iterator it;

	int number = 0;
	for (it = limits.begin(); it != limits.end(); ++it, ++number)
	{
		QLabel* label = new QLabel(this);
		label->setText(it->name);
		QSpinBox* spinBox = new QSpinBox(this);
		spinBox->setMaximum(999999);

		limitsLayout->addWidget(label, 1 + (number / 2), 1 + (2 * (number % 2)) );
		limitsLayout->addWidget(spinBox, 1 + (number / 2), 2 + (2 * (number % 2)) );

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

	if (!engNfo.allowsConnectPassword)
	{
		labelConnectPassword->hide();
		leConnectPassword->hide();
	}

	if (!engNfo.allowsEmail)
	{
		labelEmail->hide();
		leEmail->hide();
	}

	if (!engNfo.allowsJoinPassword)
	{
		labelJoinPassword->hide();
		leJoinPassword->hide();
	}

	if (!engNfo.allowsMOTD)
	{
		labelMOTD->hide();
		pteMOTD->hide();
	}

	if (!engNfo.allowsRConPassword)
	{
		labelRConPassword->hide();
		leRConPassword->hide();
	}

	if (!engNfo.allowsURL)
	{
		labelURL->hide();
		leURL->hide();
	}
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

	const QString iwads[] = { "doom.wad", "doom1.wad", "doom2.wad", "tnt.wad", "plutonia.wad", "hexen.wad", "hexdd.wad", "freedoom.wad", "strife1.wad", "" };

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

	if (!engNfo.supportsRandomMapRotation)
	{
		cbRandomMapRotation->hide();
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

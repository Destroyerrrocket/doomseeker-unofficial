//------------------------------------------------------------------------------
// configureDlg.cpp
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

#include "gui/configureDlg.h"
#include "gui/cfgAppearance.h"
#include "gui/cfgcustomservers.h"
#include "gui/cfgFilePaths.h"
#include "gui/cfgip2c.h"
#include "gui/cfgQuery.h"
#include "gui/mainwindow.h"
#include "gui/wadseekerconfigappearance.h"
#include "gui/wadseekerconfiggeneral.h"
#include "gui/wadseekerconfigidgames.h"
#include "gui/wadseekerconfigsites.h"
#include "log.h"
#include "main.h"
#include <Qt>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QAbstractButton>

ConfigureDlg::ConfigureDlg(Config* mainCfg, QWidget* parent) : QDialog(parent)
{
	bAppearanceChanged = false;
	bCustomServersChanged = false;

	mainConfig = mainCfg;
	mainConfig->readConfig();
	setupUi(this);
	initOptionsList();

	tvOptionsList->setHeaderHidden(true);

	currentlyDisplayedCfgBox = NULL;
	connect(tvOptionsList, SIGNAL( clicked(const QModelIndex&) ), this, SLOT( optionListClicked(const QModelIndex&) ) );
	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT ( btnClicked(QAbstractButton *) ));
}

ConfigureDlg::~ConfigureDlg()
{
	for(int i = 0; i < configBoxesList.count(); ++i)
	{
		delete configBoxesList[i];
	}
}

bool ConfigureDlg::addConfigurationBox(QStandardItem* rootItem, ConfigurationBoxInfo* cfgBox, int position)
{
	if (!canConfigurationBoxBeAddedToList(cfgBox))
	{
		return false;
	}

	QStandardItem* item = new QStandardItem(cfgBox->boxName);
	item->setIcon(cfgBox->icon);
	cfgBox->itemOnTheList = item;

	if (rootItem == NULL)
	{
		QStandardItemModel* model = (QStandardItemModel*)tvOptionsList->model();
		rootItem = model->invisibleRootItem();
	}

	if (position < 0)
	{
		rootItem->appendRow(item);
	}
	else
	{
		rootItem->insertRow(position, item);
	}

	configBoxesList.push_back(cfgBox);
	connect(cfgBox->confBox, SIGNAL( appearanceChanged() ), this, SLOT( appearanceChangedSlot() ) );
	connect(cfgBox->confBox, SIGNAL( wantChangeDefaultButton(QPushButton*) ), this, SLOT( wantChangeDefaultButton(QPushButton*) ) );

	return true;
}

bool ConfigureDlg::addEngineConfiguration(ConfigurationBoxInfo* cfgBox)
{
	if (enginesRoot != NULL)
	{
		return addConfigurationBox(enginesRoot, cfgBox);
	}
	return false;
}

void ConfigureDlg::appearanceChangedSlot()
{
	bAppearanceChanged = true;
}

void ConfigureDlg::appendWadseekerConfigurationBoxes(QStandardItemModel* model)
{
	QStandardItem* wadseekerRoot = new QStandardItem(tr("Wadseeker"));
	wadseekerRoot->setIcon(QIcon(":/icons/download.png"));
	model->appendRow(wadseekerRoot);

	ConfigurationBoxInfo* cfgBoxInfo = NULL;

	cfgBoxInfo = WadseekerAppearanceConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(wadseekerRoot, cfgBoxInfo);

	cfgBoxInfo = WadseekerGeneralConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(wadseekerRoot, cfgBoxInfo);

	cfgBoxInfo = WadseekerSitesConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(wadseekerRoot, cfgBoxInfo);

	cfgBoxInfo = WadseekerIdgamesConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(wadseekerRoot, cfgBoxInfo);
}

void ConfigureDlg::btnClicked(QAbstractButton *button)
{
	// Figure out what button we pressed and perform its action.
	switch(buttonBox->standardButton(button))
	{
		default:
			break;

		case QDialogButtonBox::Ok: // Also does the same as Apply
			this->accept();

		case QDialogButtonBox::Apply:
			this->saveSettings();
			break;

		case QDialogButtonBox::Cancel:
			this->reject();
			break;
	}
}

bool ConfigureDlg::canConfigurationBoxBeAddedToList(ConfigurationBoxInfo* cfgBoxInfo)
{
	return isConfigurationBoxInfoValid(cfgBoxInfo) && !cfgBoxInfo->boxName.isEmpty() && cfgBoxInfo->itemOnTheList == NULL;
}

ConfigurationBoxInfo* ConfigureDlg::findConfigurationBoxInfo(const QStandardItem* item)
{
	// Cycle through known engines
	for(int i = 0; i < configBoxesList.count(); ++i)
	{
		ConfigurationBoxInfo* cfgBoxInfo = configBoxesList[i];
		if (item == cfgBoxInfo->itemOnTheList && cfgBoxInfo->confBox != NULL)
		{
			return cfgBoxInfo;
		}
	}

	return NULL;
}

void ConfigureDlg::initOptionsList()
{
	QStandardItemModel* model = new QStandardItemModel(this);

	enginesRoot = new QStandardItem(tr("Engines"));
	model->appendRow(enginesRoot);

	ConfigurationBoxInfo* cfgBoxInfo = NULL;

	cfgBoxInfo = AppearanceConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cfgBoxInfo);

	cfgBoxInfo = CustomServersConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cfgBoxInfo);
	customServersCfgBox = cfgBoxInfo->confBox;
	
	cfgBoxInfo = IP2CConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cfgBoxInfo);

	cfgBoxInfo = QueryConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cfgBoxInfo);

	cfgBoxInfo = FilePathsConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cfgBoxInfo);

	appendWadseekerConfigurationBoxes(model);

	tvOptionsList->setModel(model);
}

bool ConfigureDlg::isConfigurationBoxInfoValid(ConfigurationBoxInfo* cfgBoxInfo)
{
	return cfgBoxInfo != NULL && cfgBoxInfo->confBox != NULL;
}

void ConfigureDlg::optionListClicked(const QModelIndex& index)
{
	QStandardItemModel* model = static_cast<QStandardItemModel*>(tvOptionsList->model());
	QStandardItem* item = model->itemFromIndex(index);

	ConfigurationBoxInfo *cfgBox = findConfigurationBoxInfo(item);

	// Something with sense was selected, display this something
	// and hide previous box.
	if (isConfigurationBoxInfoValid(cfgBox))
	{
		if (!cfgBox->confBox->areSettingsAlreadyRead())
		{
			cfgBox->confBox->read();
		}
		cfgBox->confBox->setAllowSave(true);
		showConfigurationBox(cfgBox->confBox);
	}
}

void ConfigureDlg::saveSettings()
{
	// Iterate through every engine and execute it's saving method
	for (int i = 0; i < configBoxesList.count(); ++i)
	{
		configBoxesList[i]->confBox->save();
	}

	bCustomServersChanged = customServersCfgBox->allowSave();
	mainConfig->saveConfig();
	gLog << tr("Settings saved!");

	// In case the master server addresses changed, notify the master clients.
	reinterpret_cast<MainWindow *> (Main::mainWindow)->updateMasterAddresses();

	if(isVisible())
	{
		// Allow panels such as the one for Wadseeker update their contents.
		for (int i = 0; i < configBoxesList.count(); ++i)
		{
			configBoxesList[i]->confBox->read();
		}
	}
}


void ConfigureDlg::showConfigurationBox(QWidget* widget)
{
	if (currentlyDisplayedCfgBox != NULL)
	{
		currentlyDisplayedCfgBox->hide();
	}
	currentlyDisplayedCfgBox = widget;

	if (widget != NULL)
	{
		mainPanel->layout()->addWidget(widget);
		widget->show();
	}
}

void ConfigureDlg::wantChangeDefaultButton(QPushButton* button)
{
	if (button == NULL)
	{
		buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
	}
	else
	{
		button->setDefault(true);
	}
}

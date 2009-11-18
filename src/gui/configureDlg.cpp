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
#include "gui/cfgQuery.h"
#include "gui/wadseekerconfig.h"
#include "log.h"
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

bool ConfigureDlg::addConfigurationBox(QStandardItem* rootItem, ConfigurationBoxInfo* cfgBox, int pos)
{
	if (cfgBox == NULL || cfgBox->confBox == NULL || cfgBox->boxName.isEmpty() || cfgBox->itemOnTheList != NULL)
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

	if (pos < 0)
	{
		rootItem->appendRow(item);
	}
	else
	{
		rootItem->insertRow(pos, item);
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

ConfigurationBoxInfo* ConfigureDlg::findConfigurationBoxInfo(const QStandardItem* item)
{
	// Cycle through known engines
	for(int i = 0; i < configBoxesList.count(); ++i)
	{
		ConfigurationBoxInfo* ec = configBoxesList[i];
		if (item == ec->itemOnTheList && ec->confBox != NULL)
		{
			return ec;
		}
	}

	return NULL;
}

void ConfigureDlg::initOptionsList()
{
	QStandardItemModel* model = new QStandardItemModel(this);
	QStandardItem* root1;

	root1 = new QStandardItem("Engines");
	model->appendRow(root1);
	enginesRoot = root1;

//	root1 = new QStandardItem("<HIDE>");
//	model->appendRow(root1);
//	hider = root1;

	ConfigurationBoxInfo* cbi;

	cbi = AppearanceConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cbi);

	cbi = CustomServersConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cbi);
	customServersCfgBox = cbi->confBox;

	cbi = QueryConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cbi);

	cbi = FilePathsConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cbi);

	cbi = WadseekerConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cbi);

	tvOptionsList->setModel(model);
}

void ConfigureDlg::optionListClicked(const QModelIndex& index)
{
	QString str = index.data().toString();
	QStandardItemModel* model = static_cast<QStandardItemModel*>(tvOptionsList->model());
	QStandardItem* item = model->itemFromIndex(index);

//	if (item == hider)
//	{
//		showConfigurationBox(NULL);
//		return;
//	}

	ConfigurationBoxInfo *cfgBox = findConfigurationBoxInfo(item);

	// Something with sense was selected, display this something
	// and hide previous box.
	if (cfgBox != NULL && cfgBox->confBox != NULL)
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
	qDebug() << "Saving settings:";
	// Iterate through every engine and execute it's saving method
	for (int i = 0; i < configBoxesList.count(); ++i)
	{
		if (configBoxesList[i]->confBox->save())
		{
			qDebug() << "Box:" << configBoxesList[i]->boxName;
		}
	}

	bCustomServersChanged = customServersCfgBox->allowSave();
	mainConfig->saveConfig();
	Log::logger << tr("Settings saved!");
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

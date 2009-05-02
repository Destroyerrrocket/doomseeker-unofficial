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
#include "gui/cfgFilePaths.h"
#include "gui/wadseekerconfig.h"
#include <Qt>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QAbstractButton>

ConfigureDlg::ConfigureDlg(Config* mainCfg, QWidget* parent) : QDialog(parent)
{
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
/////////////////////////////////////////////////////////
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

	cbi = FilePathsConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cbi);

	cbi = WadseekerConfigBox::createStructure(mainConfig, this);
	addConfigurationBox(model->invisibleRootItem(), cbi);

	tvOptionsList->setModel(model);
}

void ConfigureDlg::saveSettings()
{
	qDebug() << "Saving settings:";
	// Iterate through every engine and execute it's saving method
	for (int i = 0; i < configBoxesList.count(); ++i)
	{
		if (configBoxesList[i]->confBox->save())
			qDebug() << "Box:" << configBoxesList[i]->boxName;
	}
	mainConfig->saveConfig();
	qDebug() << "Saving completed!";
}
/////////////////////////////////////////////////////////

bool ConfigureDlg::addConfigurationBox(QStandardItem* rootItem, ConfigurationBoxInfo* cfgBox, int pos)
{
	if (cfgBox == NULL || cfgBox->confBox == NULL || cfgBox->boxName.isEmpty() || cfgBox->itemOnTheList != NULL)
	{
		return false;
	}

	QStandardItem* item = new QStandardItem(cfgBox->boxName);
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
	return true;
}
// This will hide currently displayed box if NULL is passed
// as w argument.
void ConfigureDlg::showConfigurationBox(QWidget* w)
{
	if (currentlyDisplayedCfgBox != NULL)
	{
		currentlyDisplayedCfgBox->hide();
	}
	currentlyDisplayedCfgBox = w;

	if (w != NULL)
	{
		mainPanel->layout()->addWidget(w);
		w->show();
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
/////////////////////////////////////////////////////////
bool ConfigureDlg::addEngineConfiguration(ConfigurationBoxInfo* cfgBox)
{
	if (enginesRoot != NULL)
	{
		return addConfigurationBox(enginesRoot, cfgBox);
	}
	return false;
}
/////////////////////////////////////////////////////////
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
		cfgBox->confBox->readSettings();
		cfgBox->confBox->setAllowSave(true);
		showConfigurationBox(cfgBox->confBox);
	}
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

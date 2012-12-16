//------------------------------------------------------------------------------
// doomseekerconfigurationdialog.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "doomseekerconfigurationdialog.h"
#include "configuration/doomseekerconfig.h"
#include "gui/configuration/cfgappearance.h"
#include "gui/configuration/cfgautoupdates.h"
#include "gui/configuration/cfgcustomservers.h"
#include "gui/configuration/cfgfilepaths.h"
#include "gui/configuration/cfgip2country.h"
#include "gui/configuration/cfgquery.h"
#include "gui/configuration/cfgwadseekerappearance.h"
#include "gui/configuration/cfgwadseekergeneral.h"
#include "gui/configuration/cfgwadseekeridgames.h"
#include "gui/configuration/cfgwadseekersites.h"
#include "gui/mainwindow.h"
#include "log.h"
#include "main.h"

DoomseekerConfigurationDialog::DoomseekerConfigurationDialog(QWidget* parent)
: ConfigurationDialog(parent)
{
	this->bAppearanceChanged = false;
	this->bCustomServersChanged = false;
}

QStandardItem* DoomseekerConfigurationDialog::addConfigurationBox(QStandardItem* rootItem, ConfigurationBaseBox* pConfigurationBox, int position)
{
	QStandardItem* pItem = ConfigurationDialog::addConfigurationBox(rootItem, pConfigurationBox, position);

	if (pItem != NULL)
	{
		connect(pConfigurationBox, SIGNAL( appearanceChanged() ), 
			SLOT( appearanceChangedSlot() ) );
	}
	
	return pItem;
}

bool DoomseekerConfigurationDialog::addEngineConfiguration(ConfigurationBaseBox* pConfigurationBox)
{
	if (enginesRoot != NULL)
	{
		return addConfigurationBox(enginesRoot, pConfigurationBox);
	}
	return false;
}

void DoomseekerConfigurationDialog::appearanceChangedSlot()
{
	this->bAppearanceChanged = true;
}

void DoomseekerConfigurationDialog::appendWadseekerConfigurationBoxes()
{
	QStandardItem* wadseekerRoot = addLabel(NULL, tr("Wadseeker"));
	wadseekerRoot->setIcon(QIcon(":/icons/download.png"));
	
	ConfigurationBaseBox* pConfigBox = NULL;

	pConfigBox = new CFGWadseekerAppearance(this);
	addConfigurationBox(wadseekerRoot, pConfigBox);

	pConfigBox = new CFGWadseekerGeneral(this);
	addConfigurationBox(wadseekerRoot, pConfigBox);

	pConfigBox = new CFGWadseekerSites(this);
	addConfigurationBox(wadseekerRoot, pConfigBox);

	pConfigBox = new CFGWadseekerIdgames(this);
	addConfigurationBox(wadseekerRoot, pConfigBox);
}

void DoomseekerConfigurationDialog::doSaveSettings()
{
	bCustomServersChanged = customServersCfgBox->allowSave();
	if (gConfig.saveToFile())
	{
		gLog << tr("Settings saved!");
	}
	else
	{
		gLog << tr("Settings save failed!");
	}
}

void DoomseekerConfigurationDialog::initOptionsList()
{
	enginesRoot = addLabel(NULL, tr("Engines"));
	enginesRoot->setIcon(QIcon(":/icons/joystick.png"));

	ConfigurationBaseBox* pConfigBox = NULL;

	pConfigBox = new CFGAppearance(this);
	addConfigurationBox(NULL, pConfigBox);

	pConfigBox = new CFGAutoUpdates(this);
	addConfigurationBox(NULL, pConfigBox);

	pConfigBox = new CFGCustomServers(this);
	addConfigurationBox(NULL, pConfigBox);
	customServersCfgBox = pConfigBox;
	
	pConfigBox = new CFGIP2Country(this);
	addConfigurationBox(NULL, pConfigBox);

	pConfigBox = new CFGQuery(this);
	addConfigurationBox(NULL, pConfigBox);

	pConfigBox = new CFGFilePaths(this);
	addConfigurationBox(NULL, pConfigBox);

	appendWadseekerConfigurationBoxes();
	
	optionsTree()->expandAll();
}

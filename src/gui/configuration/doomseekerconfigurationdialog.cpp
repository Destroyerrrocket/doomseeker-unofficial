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
#include "gui/configuration/cfgappearance.h"
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

	IniSection &wadseekerSection = Main::ini->createSection("Wadseeker");
	pConfigBox = new CFGWadseekerAppearance(wadseekerSection, this);
	addConfigurationBox(wadseekerRoot, pConfigBox);

	pConfigBox = new CFGWadseekerGeneral(wadseekerSection, this);
	addConfigurationBox(wadseekerRoot, pConfigBox);

	pConfigBox = new CFGWadseekerSites(wadseekerSection, this);
	addConfigurationBox(wadseekerRoot, pConfigBox);

	pConfigBox = new CFGWadseekerIdgames(wadseekerSection, this);
	addConfigurationBox(wadseekerRoot, pConfigBox);
}

void DoomseekerConfigurationDialog::doSaveSettings()
{
	bCustomServersChanged = customServersCfgBox->allowSave();
	Main::ini->save();
	gLog << tr("Settings saved!");

	// In case the master server addresses changed, notify the master clients.
	reinterpret_cast<MainWindow *> (Main::mainWindow)->updateMasterAddresses();
}

void DoomseekerConfigurationDialog::initOptionsList()
{
	enginesRoot = addLabel(NULL, tr("Engines"));

	ConfigurationBaseBox* pConfigBox = NULL;

	pConfigBox = new CFGAppearance(Main::config, this);
	addConfigurationBox(NULL, pConfigBox);

	pConfigBox = new CFGCustomServers(Main::config, this);
	addConfigurationBox(NULL, pConfigBox);
	customServersCfgBox = pConfigBox;
	
	pConfigBox = new CFGIP2Country(Main::config, this);
	addConfigurationBox(NULL, pConfigBox);

	pConfigBox = new CFGQuery(Main::config, this);
	addConfigurationBox(NULL, pConfigBox);

	pConfigBox = new CFGFilePaths(Main::config, this);
	addConfigurationBox(NULL, pConfigBox);

	appendWadseekerConfigurationBoxes();
}

//------------------------------------------------------------------------------
// doomseekerconfigurationdialog.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "doomseekerconfigurationdialog.h"
#include "configuration/doomseekerconfig.h"
#include "gui/configuration/cfgappearance.h"
#include "gui/configuration/cfgautoupdates.h"
#include "gui/configuration/cfgcustomservers.h"
#include "gui/configuration/cfgfilepaths.h"
#include "gui/configuration/cfgquery.h"
#include "gui/configuration/cfgserverpasswords.h"
#include "gui/configuration/cfgwadalias.h"
#include "gui/configuration/cfgwadseekerappearance.h"
#include "gui/configuration/cfgwadseekergeneral.h"
#include "gui/configuration/cfgwadseekeridgames.h"
#include "gui/configuration/cfgwadseekersites.h"
#include "gui/configuration/engineconfigpage.h"
#include "gui/mainwindow.h"
#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"
#include "updater/updatechannel.h"
#include "application.h"
#include "log.h"
#include "qtmetapointer.h"
#include <QStandardItem>
#include <QTreeView>

DoomseekerConfigurationDialog::DoomseekerConfigurationDialog(QWidget* parent)
: ConfigurationDialog(parent)
{
	this->bAppearanceChanged = false;
	this->bCustomServersChanged = false;
	this->bRestartNeeded = false;
}

QStandardItem* DoomseekerConfigurationDialog::addConfigPage(QStandardItem* rootItem, ConfigPage* configPage, int position)
{
	QStandardItem* pItem = ConfigurationDialog::addConfigPage(rootItem, configPage, position);

	if (pItem != NULL)
	{
		connect(configPage, SIGNAL( appearanceChanged() ),
			SLOT( appearanceChangedSlot() ) );
		connect(configPage, SIGNAL( restartNeeded() ),
			SLOT( restartNeededSlot() ) );
	}

	return pItem;
}

bool DoomseekerConfigurationDialog::addEngineConfiguration(ConfigPage* configPage)
{
	if (enginesRoot != NULL)
	{
		return addConfigPage(enginesRoot, configPage);
	}
	return false;
}

void DoomseekerConfigurationDialog::appearanceChangedSlot()
{
	this->bAppearanceChanged = true;
	emit appearanceChanged();
}

void DoomseekerConfigurationDialog::restartNeededSlot()
{
	this->bRestartNeeded = true;
}

void DoomseekerConfigurationDialog::appendFilePathsConfigurationBoxes()
{
	QStandardItem *itemFilePaths = addConfigPage(NULL, new CFGFilePaths(this));
	addConfigPage(itemFilePaths, new CFGWadAlias(this));
}

void DoomseekerConfigurationDialog::appendWadseekerConfigurationBoxes()
{
	QStandardItem* wadseekerRoot = addLabel(NULL, tr("Wadseeker"));
	wadseekerRoot->setIcon(QIcon(":/icons/get-wad.png"));

	ConfigPage* configPage = NULL;

	configPage = new CFGWadseekerAppearance(this);
	addConfigPage(wadseekerRoot, configPage);

	configPage = new CFGWadseekerGeneral(this);
	addConfigPage(wadseekerRoot, configPage);

	configPage = new CFGWadseekerSites(this);
	addConfigPage(wadseekerRoot, configPage);

	configPage = new CFGWadseekerIdgames(this);
	addConfigPage(wadseekerRoot, configPage);
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
	enginesRoot = addLabel(NULL, tr("Games"));
	enginesRoot->setIcon(QIcon(":/icons/joystick.png"));

	ConfigPage* configPage = NULL;

	configPage = new CFGAppearance(this);
	addConfigPage(NULL, configPage);

	configPage = new CFGAutoUpdates(this);
	addConfigPage(NULL, configPage);

	configPage = new CFGCustomServers(this);
	addConfigPage(NULL, configPage);
	customServersCfgBox = configPage;

	configPage = new CFGServerPasswords(this);
	addConfigPage(NULL, configPage);

	configPage = new CFGQuery(this);
	addConfigPage(NULL, configPage);

	appendFilePathsConfigurationBoxes();
	appendWadseekerConfigurationBoxes();

	optionsTree()->expandAll();
}

bool DoomseekerConfigurationDialog::isOpen()
{
	if (gApp->mainWindow() == NULL)
		return false;
	foreach(QObject *obj, gApp->mainWindow()->children())
	{
		if (qobject_cast<DoomseekerConfigurationDialog*>(obj) != NULL)
			return true;
	}
	return false;
}

void DoomseekerConfigurationDialog::openConfiguration(QWidget *parent, const EnginePlugin *openPlugin)
{
	DoomseekerConfigurationDialog configDialog(parent);

	MainWindow *mainWindow = gApp->mainWindow();
	if (mainWindow != NULL)
		mainWindow->connect(&configDialog, SIGNAL(appearanceChanged()), SLOT(updateDynamicAppearance()));

	configDialog.initOptionsList();

	for(unsigned i = 0; i < gPlugins->numPlugins(); ++i)
	{
		EnginePlugin* pPluginInfo = gPlugins->info(i);

		// Create the config box.
		ConfigPage* configPage = pPluginInfo->configuration(&configDialog);
		configDialog.addEngineConfiguration(configPage);
	}

	bool bLookupHostsSettingBefore = gConfig.doomseeker.bLookupHosts;
	DoomseekerConfig::AutoUpdates::UpdateMode updateModeBefore = gConfig.autoUpdates.updateMode;
	UpdateChannel updateChannelBefore = UpdateChannel::fromName(gConfig.autoUpdates.updateChannelName);
	// Stop the auto refresh timer during configuration.
	if (mainWindow != NULL)
		mainWindow->stopAutoRefreshTimer();

	if(openPlugin)
		configDialog.showPluginConfiguration(openPlugin);

	configDialog.exec();

	// Do some cleanups after config box finishes.
	if (mainWindow != NULL)
		mainWindow->initAutoRefreshTimer();

	// If update channel or update mode changed then we should discard the
	// current updates.
	UpdateChannel updateChannelAfter = UpdateChannel::fromName(gConfig.autoUpdates.updateChannelName);
	if (updateChannelBefore != updateChannelAfter
		|| updateModeBefore != gConfig.autoUpdates.updateMode)
	{
		if (mainWindow != NULL)
			mainWindow->abortAutoUpdater();
		gConfig.autoUpdates.bPerformUpdateOnNextRun = false;
		gConfig.saveToFile();
	}

	if (mainWindow != NULL)
		mainWindow->finishConfiguration(configDialog, !bLookupHostsSettingBefore && gConfig.doomseeker.bLookupHosts);
}

void DoomseekerConfigurationDialog::showPluginConfiguration(const EnginePlugin *plugin)
{
	// Find plugin page and make it the active page
	for(int i = 0;i < enginesRoot->rowCount();++i)
	{
		QStandardItem *page = enginesRoot->child(i);
		QtMetaPointer metaPointer = page->data(Qt::UserRole).value<QtMetaPointer>();
		void* pointer = metaPointer;
		EngineConfigPage *engineConfig = (EngineConfigPage*)pointer;

		if(engineConfig->plugin() == plugin)
		{
			showConfigPage(engineConfig);
		}
	}
}

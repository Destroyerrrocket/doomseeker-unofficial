//------------------------------------------------------------------------------
// engineConfigBase.cpp
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "engineConfigBase.h"
#include "sdeapi/pluginloader.hpp"

#include <QFileDialog>

BaseEngineConfigBox::BaseEngineConfigBox(const PluginInfo *plugin, IniSection &cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent), plugin(plugin)
{
	setupUi(this);

	setTitle(plugin->name + tr(" Configuration"));
	if(!plugin->pInterface->hasMasterServer())
		masterAddressBox->hide();

	connect(btnBrowseClientBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseClientBinaryClicked() ));
	connect(btnBrowseServerBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseServerBinaryClicked() ));
}

void BaseEngineConfigBox::addWidget(QWidget *widget)
{
	layout()->removeItem(verticalSpacer);
	layout()->addWidget(widget);
	layout()->addItem(verticalSpacer);
}

void BaseEngineConfigBox::browseForBinary(QLineEdit *input, const QString &type)
{
	QString filter;
#if defined(Q_OS_WIN32)
	filter = tr("Binary files (*.exe);;Any files (*)");
#else
	// Other platforms do not have an extension for their binary files.
	filter = tr("Any files(*)");
#endif
	QString strFilepath = QFileDialog::getOpenFileName(this, tr("Doomseeker - choose ") + plugin->name + " " + type, QString(), filter);
	if(!strFilepath.isEmpty()) // don't update if nothing was selected.
		input->setText(strFilepath);
}

void BaseEngineConfigBox::btnBrowseClientBinaryClicked()
{
	browseForBinary(leClientBinaryPath, tr("client binary"));

}

void BaseEngineConfigBox::btnBrowseServerBinaryClicked()
{
	browseForBinary(leServerBinaryPath, tr("server binary"));
}

ConfigurationBoxInfo *BaseEngineConfigBox::createStructure(const PluginInfo *plugin, IniSection &cfg, QWidget *parent)
{
	ConfigurationBoxInfo *pConfigurationBoxInfo = new ConfigurationBoxInfo();
	pConfigurationBoxInfo->boxName = plugin->name;
	pConfigurationBoxInfo->confBox = new BaseEngineConfigBox(plugin, cfg, parent);
	pConfigurationBoxInfo->icon = plugin->pInterface->icon();
	return pConfigurationBoxInfo;
}

void BaseEngineConfigBox::makeClientOnly()
{
	lblClientBinary->setText(tr("Path to executable:"));
	serverBinaryBox->hide();
}

void BaseEngineConfigBox::readSettings()
{
	leClientBinaryPath->setText(config["BinaryPath"]);
	leCustomParameters->setText(config["CustomParameters"]);

	if(plugin->pInterface->hasMasterServer())
		leMasterserverAddress->setText(config["Masterserver"]);

	leServerBinaryPath->setText(config["ServerBinaryPath"]);
}

void BaseEngineConfigBox::saveSettings()
{
	QString strVal;

	strVal = leClientBinaryPath->text();
	config["BinaryPath"] = strVal;

	strVal = leCustomParameters->text();
	config["CustomParameters"] = strVal;

	if(plugin->pInterface->hasMasterServer())
	{
		strVal = leMasterserverAddress->text();
		config["Masterserver"] = strVal;
	}

	strVal = leServerBinaryPath->text();
	config["ServerBinaryPath"] = strVal;
}

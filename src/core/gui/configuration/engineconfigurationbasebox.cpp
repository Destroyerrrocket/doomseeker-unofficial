//------------------------------------------------------------------------------
// engineconfigurationbasebox.cpp
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

#include "engineconfigurationbasebox.h"
#include "sdeapi/pluginloader.hpp"

#include <QFileDialog>

EngineConfigurationBaseBox::EngineConfigurationBaseBox(const PluginInfo *plugin, IniSection &cfg, QWidget *parent) 
: ConfigurationBaseBox(parent), config(cfg), plugin(plugin)
{
	setupUi(this);

	setTitle(plugin->name + tr(" Configuration"));
	if(!plugin->pInterface->hasMasterServer())
		masterAddressBox->hide();

	connect(btnBrowseClientBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseClientBinaryClicked() ));
	connect(btnBrowseServerBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseServerBinaryClicked() ));
}

void EngineConfigurationBaseBox::addWidget(QWidget *widget)
{
	layout()->removeItem(verticalSpacer);
	layout()->addWidget(widget);
	layout()->addItem(verticalSpacer);
}

void EngineConfigurationBaseBox::browseForBinary(QLineEdit *input, const QString &type)
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

void EngineConfigurationBaseBox::btnBrowseClientBinaryClicked()
{
	browseForBinary(leClientBinaryPath, tr("client binary"));

}

void EngineConfigurationBaseBox::btnBrowseServerBinaryClicked()
{
	browseForBinary(leServerBinaryPath, tr("server binary"));
}

QIcon EngineConfigurationBaseBox::icon() const
{
	return plugin->pInterface->icon();
}

void EngineConfigurationBaseBox::makeClientOnly()
{
	lblClientBinary->setText(tr("Path to executable:"));
	serverBinaryBox->hide();
}

QString EngineConfigurationBaseBox::name() const
{
	return plugin->name;
}

void EngineConfigurationBaseBox::readSettings()
{
	leClientBinaryPath->setText(config["BinaryPath"]);
	leCustomParameters->setText(config["CustomParameters"]);

	if(plugin->pInterface->hasMasterServer())
		leMasterserverAddress->setText(config["Masterserver"]);

	leServerBinaryPath->setText(config["ServerBinaryPath"]);
}

void EngineConfigurationBaseBox::saveSettings()
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

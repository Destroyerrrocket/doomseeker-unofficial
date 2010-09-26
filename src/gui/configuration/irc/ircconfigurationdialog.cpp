//------------------------------------------------------------------------------
// ircconfigurationdialog.cpp
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
#include "ircconfigurationdialog.h"
#include "gui/configuration/irc/cfgircnetworks.h"
#include "irc/configuration/ircconfig.h"
#include "log.h"

IRCConfigurationDialog::IRCConfigurationDialog(QWidget* parent)
: ConfigurationDialog(parent)
{
	this->setWindowTitle(tr("Doomseeker - IRC Options"));
}

void IRCConfigurationDialog::doSaveSettings()
{
	if (gIRCConfig.saveToFile())
	{
		gLog << tr("Settings saved!");
	}
	else
	{
		gLog << tr("Settings save failed!");
	}
}

void IRCConfigurationDialog::initOptionsList()
{
	ConfigurationBaseBox* pConfigBox = NULL;
	
	pConfigBox = new CFGIRCNetworks(this);
	this->addConfigurationBox(NULL, pConfigBox);
}



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
#include "gui/configuration/irc/cfgircappearance.h"
#include "gui/configuration/irc/cfgircnetworks.h"
#include "gui/configuration/irc/cfgircsounds.h"
#include "gui/commonGUI.h"
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

	pConfigBox = new CFGIRCAppearance(this);
	this->addConfigurationBox(NULL, pConfigBox);
	
	cfgNetworks = new CFGIRCNetworks(this); 
	pConfigBox = cfgNetworks;
	this->addConfigurationBox(NULL, pConfigBox);
	
	pConfigBox = new CFGIRCSounds(this);
	this->addConfigurationBox(NULL, pConfigBox);
}

bool IRCConfigurationDialog::isNetworkAutojoinEnabled()
{
	QVector<IRCNetworkEntity*> networks = cfgNetworks->networks();
	foreach (IRCNetworkEntity* pNetwork, networks)
	{
		if (pNetwork->bAutojoinNetwork)
		{
			return true;
		}
	}
	
	return false;
}

bool IRCConfigurationDialog::validate()
{
	if (this->isNetworkAutojoinEnabled())
	{
		if (gIRCConfig.personal.nickname.isEmpty())
		{
			QString nick = CommonGUI::askString(tr("Config validation"), tr("You have chosen one or more networks for autojoin startup but you have not defined any nickname. Please define it now."));
			if (nick.isEmpty())
			{
				return false;
			}
			
			gIRCConfig.personal.nickname = nick;
		}
	}
	
	return true;
}



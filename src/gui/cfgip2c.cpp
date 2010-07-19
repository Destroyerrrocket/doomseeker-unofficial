//------------------------------------------------------------------------------
// cfgip2c.cpp
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

#include "cfgip2c.h"

IP2CConfigBox::IP2CConfigBox(Config *cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);
}

ConfigurationBoxInfo *IP2CConfigBox::createStructure(Config *cfg, QWidget *parent)
{
	ConfigurationBoxInfo* pConfigurationBoxInfo = new ConfigurationBoxInfo();
	pConfigurationBoxInfo->confBox = new IP2CConfigBox(cfg, parent);
	pConfigurationBoxInfo->boxName = tr("IP2C");
	return pConfigurationBoxInfo;
}

void IP2CConfigBox::readSettings()
{
	SettingsData *setting;

	setting = config->setting("IP2CUrl");
	leIP2CUrl->setText(setting->string());
	
	setting = config->setting("IP2CAutoUpdate");
	grbAutoUpdate->setChecked(setting->boolean());
	
	setting = config->setting("IP2CMaximumAge");
	spinMaximumAge->setValue(setting->integer());
}

void IP2CConfigBox::saveSettings()
{
	SettingsData *setting;

	setting = config->setting("IP2CUrl");
	setting->setValue(leIP2CUrl->text());
	
	setting = config->setting("IP2CAutoUpdate");
	setting->setValueBoolean(grbAutoUpdate->isChecked());
	
	setting = config->setting("IP2CMaximumAge");
	setting->setValue(spinMaximumAge->value());
}

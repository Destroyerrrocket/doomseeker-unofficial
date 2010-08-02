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

IP2CConfigBox::IP2CConfigBox(IniSection *cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);
}

ConfigurationBoxInfo *IP2CConfigBox::createStructure(IniSection *cfg, QWidget *parent)
{
	ConfigurationBoxInfo* pConfigurationBoxInfo = new ConfigurationBoxInfo();
	pConfigurationBoxInfo->confBox = new IP2CConfigBox(cfg, parent);
	pConfigurationBoxInfo->boxName = tr("IP2C");
	pConfigurationBoxInfo->icon = QIcon(":/icons/emblem-web.png");
	return pConfigurationBoxInfo;
}

void IP2CConfigBox::readSettings()
{
	leIP2CUrl->setText(*config->setting("IP2CUrl"));
	grbAutoUpdate->setChecked(*config->setting("IP2CAutoUpdate"));
	spinMaximumAge->setValue(*config->setting("IP2CMaximumAge"));
}

void IP2CConfigBox::saveSettings()
{
	config->setting("IP2CUrl")->setValue(leIP2CUrl->text());
	config->setting("IP2CAutoUpdate")->setValue(grbAutoUpdate->isChecked());
	config->setting("IP2CMaximumAge")->setValue(spinMaximumAge->value());
}

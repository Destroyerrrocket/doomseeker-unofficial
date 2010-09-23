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

#include "cfgip2country.h"

CFGIP2Country::CFGIP2Country(IniSection &cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);
}

void CFGIP2Country::readSettings()
{
	leIP2CUrl->setText(config["IP2CUrl"]);
	grbAutoUpdate->setChecked(config["IP2CAutoUpdate"]);
	spinMaximumAge->setValue(config["IP2CMaximumAge"]);
}

void CFGIP2Country::saveSettings()
{
	config["IP2CUrl"] = leIP2CUrl->text();
	config["IP2CAutoUpdate"] = grbAutoUpdate->isChecked();
	config["IP2CMaximumAge"] = spinMaximumAge->value();
}

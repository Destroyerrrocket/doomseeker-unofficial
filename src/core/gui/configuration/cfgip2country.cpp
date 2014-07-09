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
#include "configuration/doomseekerconfig.h"

CFGIP2Country::CFGIP2Country(QWidget *parent)
: ConfigurationBaseBox(parent)
{
	setupUi(this);
}

void CFGIP2Country::readSettings()
{
	leIP2CUrl->setText(gConfig.doomseeker.ip2CountryUrl);
	grbAutoUpdate->setChecked(gConfig.doomseeker.bIP2CountryAutoUpdate);
	spinMaximumAge->setValue(gConfig.doomseeker.ip2CountryDatabaseMaximumAge);
}

void CFGIP2Country::saveSettings()
{
	gConfig.doomseeker.ip2CountryUrl = leIP2CUrl->text();
	gConfig.doomseeker.bIP2CountryAutoUpdate = grbAutoUpdate->isChecked();
	gConfig.doomseeker.ip2CountryDatabaseMaximumAge = spinMaximumAge->value();
}

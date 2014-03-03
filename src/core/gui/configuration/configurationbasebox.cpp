//------------------------------------------------------------------------------
// configurationbasebox.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "configurationbasebox.h"

class ConfigurationBaseBox::PrivData
{
	public:
		bool bAllowSave;
		bool bSettingsAlreadyRead;
};

ConfigurationBaseBox::ConfigurationBaseBox(QWidget* parent)
: QWidget(parent)
{
	d = new PrivData();
	d->bAllowSave = false;
	d->bSettingsAlreadyRead = false;
	hide();
}

ConfigurationBaseBox::~ConfigurationBaseBox()
{
	delete d;
}

bool ConfigurationBaseBox::allowSave()
{
	return d->bAllowSave;
}

bool ConfigurationBaseBox::areSettingsAlreadyRead()
{
	return d->bSettingsAlreadyRead;
}

void ConfigurationBaseBox::read()
{
	d->bSettingsAlreadyRead = true;
	readSettings();
}

bool ConfigurationBaseBox::save()
{
	if (d->bAllowSave)
	{
		saveSettings();
		return true;
	}
	else
	{
		return false;
	}
}

void ConfigurationBaseBox::setAllowSave(bool b)
{
	d->bAllowSave = b;
}

QString ConfigurationBaseBox::title() const
{
	return name();
}

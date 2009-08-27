//------------------------------------------------------------------------------
// cfgQueryPaths.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "gui/cfgQuery.h"

QueryConfigBox::QueryConfigBox(Config *cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);
}

ConfigurationBoxInfo *QueryConfigBox::createStructure(Config *cfg, QWidget *parent)
{
	ConfigurationBoxInfo* ec = new ConfigurationBoxInfo();
	ec->confBox = new QueryConfigBox(cfg, parent);
	ec->boxName = tr("Query");
	return ec;
}

void QueryConfigBox::readSettings()
{
	SettingsData *setting;

	setting = config->setting("QueryOnStartup");
	queryOnStartup->setChecked(setting->integer() != 0);

	setting = config->setting("QueryTries");
	triesBox->setValue(setting->integer());

	setting = config->setting("QueryTimeout");
	timeoutBox->setValue(setting->integer());

	setting = config->setting("QueryThreads");
	threadsBox->setValue(setting->integer());
}

void QueryConfigBox::saveSettings()
{
	SettingsData *setting;

	setting = config->setting("QueryOnStartup");
	setting->setValue(queryOnStartup->isChecked());

	setting = config->setting("QueryTries");
	setting->setValue(triesBox->value());

	setting = config->setting("QueryTimeout");
	setting->setValue(timeoutBox->value());

	setting = config->setting("QueryThreads");
	setting->setValue(threadsBox->value());
}

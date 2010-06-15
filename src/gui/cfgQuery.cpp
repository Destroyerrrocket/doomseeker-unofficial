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
	ConfigurationBoxInfo* pConfigurationBoxInfo = new ConfigurationBoxInfo();
	pConfigurationBoxInfo->confBox = new QueryConfigBox(cfg, parent);
	pConfigurationBoxInfo->boxName = tr("Query");
	return pConfigurationBoxInfo;
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

	setting = config->setting("QueryAutoRefreshEnabled");
	grbServerAutoRefresh->setChecked(setting->boolean());

	setting = config->setting("QueryAutoRefreshEverySeconds");
	numAutoRefreshEverySeconds->setValue(setting->integer());

	setting = config->setting("QueryAutoRefreshDontIfActive");
	cbDontRefreshIfActive->setChecked(setting->boolean());
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

	setting = config->setting("QueryAutoRefreshEnabled");
	setting->setValue(grbServerAutoRefresh->isChecked());

	setting = config->setting("QueryAutoRefreshEverySeconds");
	setting->setValue(numAutoRefreshEverySeconds->value());

	setting = config->setting("QueryAutoRefreshDontIfActive");
	setting->setValue(cbDontRefreshIfActive->isChecked());
}

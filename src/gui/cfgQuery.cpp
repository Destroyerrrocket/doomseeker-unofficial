//------------------------------------------------------------------------------
// cfgQuery.cpp
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

QueryConfigBox::QueryConfigBox(IniSection *cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);
}

ConfigurationBoxInfo *QueryConfigBox::createStructure(IniSection *cfg, QWidget *parent)
{
	ConfigurationBoxInfo* pConfigurationBoxInfo = new ConfigurationBoxInfo();
	pConfigurationBoxInfo->confBox = new QueryConfigBox(cfg, parent);
	pConfigurationBoxInfo->boxName = tr("Query");
	pConfigurationBoxInfo->icon = QIcon(":/icons/view-refresh-2.png");
	return pConfigurationBoxInfo;
}

void QueryConfigBox::readSettings()
{
	queryOnStartup->setChecked(config->setting("QueryOnStartup"));
	triesBox->setValue(config->setting("QueryTries"));
	timeoutBox->setValue(config->setting("QueryTimeout"));
	grbServerAutoRefresh->setChecked(config->setting("QueryAutoRefreshEnabled"));
	numAutoRefreshEverySeconds->setValue(config->setting("QueryAutoRefreshEverySeconds"));
	cbDontRefreshIfActive->setChecked(config->setting("QueryAutoRefreshDontIfActive"));
}

void QueryConfigBox::saveSettings()
{
	config->setting("QueryOnStartup") = queryOnStartup->isChecked();
	config->setting("QueryTries") = triesBox->value();
	config->setting("QueryTimeout") = timeoutBox->value();
	config->setting("QueryAutoRefreshEnabled") = grbServerAutoRefresh->isChecked();
	config->setting("QueryAutoRefreshEverySeconds") = numAutoRefreshEverySeconds->value();
	config->setting("QueryAutoRefreshDontIfActive") = cbDontRefreshIfActive->isChecked();
}

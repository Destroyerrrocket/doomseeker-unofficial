//------------------------------------------------------------------------------
// cfgquery.cpp
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
#include "cfgquery.h"
#include "configuration/doomseekerconfig.h"

CFGQuery::CFGQuery(QWidget *parent)
: ConfigurationBaseBox(parent)
{
	setupUi(this);
}

void CFGQuery::readSettings()
{
	queryBeforeLaunch->setChecked(gConfig.doomseeker.bQueryBeforeLaunch);
	queryOnStartup->setChecked(gConfig.doomseeker.bQueryOnStartup);
	triesBox->setValue(gConfig.doomseeker.queryTries);
	timeoutBox->setValue(gConfig.doomseeker.queryTimeout);
	grbServerAutoRefresh->setChecked(gConfig.doomseeker.bQueryAutoRefreshEnabled);
	numAutoRefreshEverySeconds->setValue(gConfig.doomseeker.queryAutoRefreshEverySeconds);
	cbDontRefreshIfActive->setChecked(gConfig.doomseeker.bQueryAutoRefreshDontIfActive);
	batchSizeBox->setValue(gConfig.doomseeker.queryBatchSize);
	batchDelayBox->setValue(gConfig.doomseeker.queryBatchDelay);
}

void CFGQuery::saveSettings()
{
	gConfig.doomseeker.bQueryBeforeLaunch = queryBeforeLaunch->isChecked();
	gConfig.doomseeker.bQueryOnStartup = queryOnStartup->isChecked();
	gConfig.doomseeker.queryTries = triesBox->value();
	gConfig.doomseeker.queryTimeout = timeoutBox->value();
	gConfig.doomseeker.bQueryAutoRefreshEnabled = grbServerAutoRefresh->isChecked();
	gConfig.doomseeker.queryAutoRefreshEverySeconds = numAutoRefreshEverySeconds->value();
	gConfig.doomseeker.bQueryAutoRefreshDontIfActive = cbDontRefreshIfActive->isChecked();
	gConfig.doomseeker.queryBatchSize = batchSizeBox->value();
	gConfig.doomseeker.queryBatchDelay = batchDelayBox->value();
}

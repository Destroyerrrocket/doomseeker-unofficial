//------------------------------------------------------------------------------
// cfgAppearance.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include "gui/cfgAppearance.h"

#include <QColorDialog>
#include <QSystemTrayIcon>

AppearanceConfigBox::AppearanceConfigBox(Config *cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent)
{
	customServersColor = 0;
	setupUi(this);
}

ConfigurationBoxInfo *AppearanceConfigBox::createStructure(Config *cfg, QWidget *parent)
{
	ConfigurationBoxInfo* pConfigurationBoxInfo = new ConfigurationBoxInfo();
	pConfigurationBoxInfo->confBox = new AppearanceConfigBox(cfg, parent);
	pConfigurationBoxInfo->boxName = tr("Appearance");
	return pConfigurationBoxInfo;
}

void AppearanceConfigBox::readSettings()
{
	SettingsData *setting;

	setting = config->setting("SlotStyle");
	slotStyle->setCurrentIndex(setting->integer());

	setting = config->setting("CustomServersColor");
	customServersColor = setting->integer();
	btnCustomServersColor->setColor(customServersColor);

	// Make sure that the tray is available. If it's not, disable tray icon
	// completely and make sure no change can be done to the configuration in
	// this manner.
	if (!QSystemTrayIcon::isSystemTrayAvailable())
	{
		config->setting("UseTrayIcon")->setValue(false);
		config->setting("CloseToTrayIcon")->setValue(false);
		gboUseTrayIcon->setEnabled(false);
	}

	setting = config->setting("UseTrayIcon");
	gboUseTrayIcon->setChecked(setting->boolean());

	setting = config->setting("CloseToTrayIcon");
	cbCloseToTrayIcon->setChecked(setting->boolean());

	// This is not really an appearance option, but it does change how the list
	// appears and thus utilized the fact that the appearance options cause the 
	// list to refresh.  It also doesn't fit into any of the other existing
	// categories at this time.
	setting = config->setting("BotsAreNotPlayers");
	cbBotsNotPlayers->setChecked(setting->boolean());
}

void AppearanceConfigBox::saveSettings()
{
	SettingsData *setting;

	setting = config->setting("SlotStyle");
	setting->setValue(slotStyle->currentIndex());

	setting = config->setting("CustomServersColor");
	setting->setValue(btnCustomServersColor->colorUnsigned());

	setting = config->setting("UseTrayIcon");
	setting->setValue(gboUseTrayIcon->isChecked());

	setting = config->setting("CloseToTrayIcon");
	setting->setValue(cbCloseToTrayIcon->isChecked());

	setting = config->setting("BotsAreNotPlayers");
	setting->setValue(cbBotsNotPlayers->isChecked());

	emit appearanceChanged();
}

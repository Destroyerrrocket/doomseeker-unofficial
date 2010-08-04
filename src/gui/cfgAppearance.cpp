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

AppearanceConfigBox::AppearanceConfigBox(IniSection *cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent)
{
	customServersColor = 0;
	setupUi(this);
}

ConfigurationBoxInfo *AppearanceConfigBox::createStructure(IniSection *cfg, QWidget *parent)
{
	ConfigurationBoxInfo* pConfigurationBoxInfo = new ConfigurationBoxInfo();
	pConfigurationBoxInfo->confBox = new AppearanceConfigBox(cfg, parent);
	pConfigurationBoxInfo->boxName = tr("Appearance");
	pConfigurationBoxInfo->icon = QIcon(":/icons/color-fill.png");
	return pConfigurationBoxInfo;
}

void AppearanceConfigBox::readSettings()
{
	slotStyle->setCurrentIndex(config->setting("SlotStyle"));

	btnCustomServersColor->setColorHtml(config->setting("CustomServersColor"));

	// Make sure that the tray is available. If it's not, disable tray icon
	// completely and make sure no change can be done to the configuration in
	// this manner.
	if (!QSystemTrayIcon::isSystemTrayAvailable())
	{
		config->setting("UseTrayIcon") = false;
		config->setting("CloseToTrayIcon") = false;
		gboUseTrayIcon->setEnabled(false);
	}

	gboUseTrayIcon->setChecked(config->setting("UseTrayIcon"));

	cbCloseToTrayIcon->setChecked(config->setting("CloseToTrayIcon"));

	// This is not really an appearance option, but it does change how the list
	// appears and thus utilized the fact that the appearance options cause the 
	// list to refresh.  It also doesn't fit into any of the other existing
	// categories at this time.
	cbBotsNotPlayers->setChecked(config->setting("BotsAreNotPlayers"));
}

void AppearanceConfigBox::saveSettings()
{
	config->setting("SlotStyle") = slotStyle->currentIndex();
	config->setting("CustomServersColor") = btnCustomServersColor->colorHtml();
	config->setting("UseTrayIcon") = gboUseTrayIcon->isChecked();
	config->setting("CloseToTrayIcon") = cbCloseToTrayIcon->isChecked();
	config->setting("BotsAreNotPlayers") = cbBotsNotPlayers->isChecked();

	emit appearanceChanged();
}

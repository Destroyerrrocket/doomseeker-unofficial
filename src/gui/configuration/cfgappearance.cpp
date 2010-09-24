//------------------------------------------------------------------------------
// cfgappearance.h
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
#include "configuration/doomseekerconfig.h"
#include "cfgappearance.h"
#include <QColorDialog>
#include <QSystemTrayIcon>

CFGAppearance::CFGAppearance(QWidget *parent) 
: ConfigurationBaseBox(parent)
{
	setupUi(this);
}

void CFGAppearance::readSettings()
{
	slotStyle->setCurrentIndex(gConfig.doomseeker.slotStyle);

	btnCustomServersColor->setColorHtml(gConfig.doomseeker.customServersColor);

	// Make sure that the tray is available. If it's not, disable tray icon
	// completely and make sure no change can be done to the configuration in
	// this manner.
	if (!QSystemTrayIcon::isSystemTrayAvailable())
	{
		gConfig.doomseeker.bUseTrayIcon = false;
		gConfig.doomseeker.bCloseToTrayIcon = false;
		gboUseTrayIcon->setEnabled(false);
	}

	gboUseTrayIcon->setChecked(gConfig.doomseeker.bUseTrayIcon);

	cbCloseToTrayIcon->setChecked(gConfig.doomseeker.bCloseToTrayIcon);

	// This is not really an appearance option, but it does change how the list
	// appears and thus utilized the fact that the appearance options cause the 
	// list to refresh.  It also doesn't fit into any of the other existing
	// categories at this time.
	cbBotsNotPlayers->setChecked(gConfig.doomseeker.bBotsAreNotPlayers);
}

void CFGAppearance::saveSettings()
{
	gConfig.doomseeker.slotStyle = slotStyle->currentIndex();
	gConfig.doomseeker.customServersColor = btnCustomServersColor->colorHtml();
	gConfig.doomseeker.bUseTrayIcon = gboUseTrayIcon->isChecked();
	gConfig.doomseeker.bCloseToTrayIcon = cbCloseToTrayIcon->isChecked();
	gConfig.doomseeker.bBotsAreNotPlayers = cbBotsNotPlayers->isChecked();

	emit appearanceChanged();
}

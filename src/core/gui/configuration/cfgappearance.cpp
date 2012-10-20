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
#include "localization.h"
#include "log.h"
#include "main.h"

CFGAppearance::CFGAppearance(QWidget *parent) 
: ConfigurationBaseBox(parent)
{
	setupUi(this);
}

void CFGAppearance::initLanguagesList()
{
	const QList<LocalizationInfo>& localizations = Main::localizations;
	foreach (const LocalizationInfo& obj, localizations)
	{
		const QString& flagName = obj.countryCodeName;
		const QString& translationName = obj.localeName;
		const QString& displayName = obj.niceName;
		
		QPixmap flag = Main::ip2c->flag(flagName);
		cboLanguage->addItem(flag, displayName, translationName);
	}
}

void CFGAppearance::readSettings()
{
	if (cboLanguage->count() == 0)
	{
		initLanguagesList();
	}
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

	cbHidePasswords->setChecked(gConfig.doomseeker.bHidePasswords);

	cbLookupHosts->setChecked(gConfig.doomseeker.bLookupHosts);

	// This is not really an appearance option, but it does change how the list
	// appears and thus utilized the fact that the appearance options cause the 
	// list to refresh.  It also doesn't fit into any of the other existing
	// categories at this time.
	cbBotsNotPlayers->setChecked(gConfig.doomseeker.bBotsAreNotPlayers);
	
	// Set language.
	int idxLanguage = cboLanguage->findData(gConfig.doomseeker.localization);
	if (idxLanguage >= 0)
	{
		cboLanguage->setCurrentIndex(idxLanguage);
	}
	else
	{
		// Display that there is something wrong.
		QString name = gConfig.doomseeker.localization;
		const QPixmap& icon = Main::ip2c->flagUnknown;
		QString str = tr("Unknown language definition \"%1\"").arg(name);
		cboLanguage->addItem(icon, str, name);
		cboLanguage->setCurrentIndex(cboLanguage->count() - 1);
	}
}

void CFGAppearance::saveSettings()
{
	gConfig.doomseeker.slotStyle = slotStyle->currentIndex();
	gConfig.doomseeker.customServersColor = btnCustomServersColor->colorHtml();
	gConfig.doomseeker.bUseTrayIcon = gboUseTrayIcon->isChecked();
	gConfig.doomseeker.bCloseToTrayIcon = cbCloseToTrayIcon->isChecked();
	gConfig.doomseeker.bBotsAreNotPlayers = cbBotsNotPlayers->isChecked();
	gConfig.doomseeker.bHidePasswords = cbHidePasswords->isChecked();
	gConfig.doomseeker.bLookupHosts = cbLookupHosts->isChecked();
	QString localization = cboLanguage->itemData(cboLanguage->currentIndex()).toString();
	if (localization != gConfig.doomseeker.localization)
	{
		// Translation may be strenuous so do it only if the selected
		// value actually changed.
		gConfig.doomseeker.localization = localization;
		gLog << tr("Loading translation \"%1\"").arg(localization);
		Localization::loadTranslation(localization);
	}

	emit appearanceChanged();
}

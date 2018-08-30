//------------------------------------------------------------------------------
// cfgappearance.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "cfgappearance.h"
#include "ui_cfgappearance.h"

#include "gui/helpers/playersdiagram.h"
#include "configuration/doomseekerconfig.h"
#include <QColorDialog>
#include <QSystemTrayIcon>
#include "ip2c/ip2c.h"
#include "localization.h"
#include "log.h"
#include "main.h"

DClass<CFGAppearance> : public Ui::CFGAppearance
{
};

DPointered(CFGAppearance)

CFGAppearance::CFGAppearance(QWidget *parent)
: ConfigPage(parent)
{
	d->setupUi(this);
}

CFGAppearance::~CFGAppearance()
{
}

void CFGAppearance::initLanguagesList()
{
	foreach (const LocalizationInfo& obj, Main::localizations)
	{
		const QString& flagName = obj.countryCodeName;
		const QString& translationName = obj.localeName;
		const QString& displayName = obj.niceName;

		QPixmap flag = IP2C::instance()->flag(flagName);
		d->cboLanguage->addItem(flag, displayName, translationName);
	}
}

void CFGAppearance::initSlotStyles(const QString &selected)
{
	QList<PlayersDiagramStyle> styles = PlayersDiagram::availableSlotStyles();
	d->slotStyle->clear();
	foreach (const PlayersDiagramStyle &style, styles)
	{
		d->slotStyle->addItem(style.displayName, style.name);
		if (style.name == selected)
		{
			d->slotStyle->setCurrentIndex(d->slotStyle->count() - 1);
		}
	}
}

void CFGAppearance::readSettings()
{
	if (d->cboLanguage->count() == 0)
	{
		initLanguagesList();
	}
	initSlotStyles(gConfig.doomseeker.slotStyle);

	d->btnCustomServersColor->setColorHtml(gConfig.doomseeker.customServersColor);
	d->btnBuddyServersColor->setColorHtml(gConfig.doomseeker.buddyServersColor);
	d->btnLanServersColor->setColorHtml(gConfig.doomseeker.lanServersColor);
	d->cbMarkServersWithBuddies->setChecked(gConfig.doomseeker.bMarkServersWithBuddies);

	// Make sure that the tray is available. If it's not, disable tray icon
	// completely and make sure no change can be done to the configuration in
	// this manner.
	if (!QSystemTrayIcon::isSystemTrayAvailable())
	{
		gConfig.doomseeker.bUseTrayIcon = false;
		gConfig.doomseeker.bCloseToTrayIcon = false;
		d->gboUseTrayIcon->setEnabled(false);
	}

	d->gboUseTrayIcon->setChecked(gConfig.doomseeker.bUseTrayIcon);

	d->cbCloseToTrayIcon->setChecked(gConfig.doomseeker.bCloseToTrayIcon);

	d->cbColorizeConsole->setChecked(gConfig.doomseeker.bColorizeServerConsole);
	d->cbDrawGridInServerTable->setChecked(gConfig.doomseeker.bDrawGridInServerTable);

	d->cbHidePasswords->setChecked(gConfig.doomseeker.bHidePasswords);

	d->cbLookupHosts->setChecked(gConfig.doomseeker.bLookupHosts);

	// This is not really an appearance option, but it does change how the list
	// appears and thus utilized the fact that the appearance options cause the
	// list to refresh.  It also doesn't fit into any of the other existing
	// categories at this time.
	d->cbBotsNotPlayers->setChecked(gConfig.doomseeker.bBotsAreNotPlayers);

	// Set language.
	LocalizationInfo bestMatchedLocalization = LocalizationInfo::findBestMatch(
		Main::localizations, gConfig.doomseeker.localization);
	int idxLanguage = -1;
	if (bestMatchedLocalization.isValid())
		idxLanguage = d->cboLanguage->findData(bestMatchedLocalization.localeName);
	if (idxLanguage >= 0)
	{
		d->cboLanguage->setCurrentIndex(idxLanguage);
	}
	else
	{
		// Display that there is something wrong.
		QString name = gConfig.doomseeker.localization;
		const QPixmap& icon = IP2C::instance()->flagUnknown;
		QString str = tr("Unknown language definition \"%1\"").arg(name);
		d->cboLanguage->addItem(icon, str, name);
		d->cboLanguage->setCurrentIndex(d->cboLanguage->count() - 1);
	}
}

void CFGAppearance::saveSettings()
{
	gConfig.doomseeker.slotStyle = d->slotStyle->itemData(d->slotStyle->currentIndex()).toString();
	gConfig.doomseeker.bMarkServersWithBuddies = d->cbMarkServersWithBuddies->isChecked();
	gConfig.doomseeker.buddyServersColor = d->btnBuddyServersColor->colorHtml();
	gConfig.doomseeker.customServersColor = d->btnCustomServersColor->colorHtml();
	gConfig.doomseeker.lanServersColor = d->btnLanServersColor->colorHtml();
	gConfig.doomseeker.bUseTrayIcon = d->gboUseTrayIcon->isChecked();
	gConfig.doomseeker.bCloseToTrayIcon = d->cbCloseToTrayIcon->isChecked();
	gConfig.doomseeker.bColorizeServerConsole = d->cbColorizeConsole->isChecked();
	gConfig.doomseeker.bDrawGridInServerTable = d->cbDrawGridInServerTable->isChecked();
	gConfig.doomseeker.bBotsAreNotPlayers = d->cbBotsNotPlayers->isChecked();
	gConfig.doomseeker.bHidePasswords = d->cbHidePasswords->isChecked();
	gConfig.doomseeker.bLookupHosts = d->cbLookupHosts->isChecked();
	QString localization = d->cboLanguage->itemData(d->cboLanguage->currentIndex()).toString();
	if (localization != gConfig.doomseeker.localization)
	{
		// Translation may be strenuous so do it only if the selected
		// value actually changed.
		gConfig.doomseeker.localization = localization;
		if (QLocale(localization) == QLocale(LocalizationInfo::DEFAULT.localeName))
		{
			// No tr() on purpose.
			gLog << "Return to default translation requires restart of the program";
		}
		else
		{
			gLog << tr("Loading translation \"%1\"").arg(localization);
			Localization::loadTranslation(localization);
			gLog << tr("Program needs to be restarted to fully apply the translation");
		}
	}

	emit appearanceChanged();
}

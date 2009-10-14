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

	connect(btnCustomServersColor, SIGNAL( clicked() ), this, SLOT ( btnCustomServersColor_clicked() ) );

	QPalette p = btnCustomServersColor->palette();
	p.setColor(QPalette::Button, Qt::blue);
	btnCustomServersColor->setPalette(p);
}

ConfigurationBoxInfo *AppearanceConfigBox::createStructure(Config *cfg, QWidget *parent)
{
	ConfigurationBoxInfo* ec = new ConfigurationBoxInfo();
	ec->confBox = new AppearanceConfigBox(cfg, parent);
	ec->boxName = tr("Appearance");
	return ec;
}

void AppearanceConfigBox::btnCustomServersColor_clicked()
{
	QColor c = QColorDialog::getColor(QColor(customServersColor), this);

	if(c.isValid())
	{
		customServersColor = c.rgb();
		setWidgetBackgroundColor(btnCustomServersColor, customServersColor);
	}
}

void AppearanceConfigBox::readSettings()
{
	SettingsData *setting;

	setting = config->setting("SlotStyle");
	slotStyle->setCurrentIndex(setting->integer());

	setting = config->setting("CustomServersColor");
	customServersColor = setting->integer();
	setWidgetBackgroundColor(btnCustomServersColor, customServersColor);

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
}

void AppearanceConfigBox::saveSettings()
{
	SettingsData *setting;

	setting = config->setting("SlotStyle");
	setting->setValue(slotStyle->currentIndex());

	QPalette p = btnCustomServersColor->palette();
	setting = config->setting("CustomServersColor");
	setting->setValue(p.color(QPalette::Button).rgb() & 0x00ffffff); // Remove alpha value

	setting = config->setting("UseTrayIcon");
	setting->setValue(gboUseTrayIcon->isChecked());

	setting = config->setting("CloseToTrayIcon");
	setting->setValue(cbCloseToTrayIcon->isChecked());

	emit appearanceChanged();
}

void AppearanceConfigBox::setWidgetBackgroundColor(QWidget* widget, unsigned color)
{
	const QString COLOR_STYLE("QPushButton { background-color : %1; }");

	// Remove alpha value
	color &= 0x00ffffff;

	// Convert to hexadecimal number
	QString colorString = QString::number(color, 16);
	while (colorString.length() < 6)
	{
		// This string must have six characters. Prepend zeros if length is
		// insufficient.
		colorString.prepend('0');
	}
	widget->setStyleSheet(COLOR_STYLE.arg("#" + colorString));
}

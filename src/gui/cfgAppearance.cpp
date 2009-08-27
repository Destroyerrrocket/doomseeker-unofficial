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

AppearanceConfigBox::AppearanceConfigBox(Config *cfg, QWidget *parent) : ConfigurationBaseBox(cfg, parent)
{
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
	QColor c = QColorDialog::getColor(Qt::white, this);

	QPalette p = btnCustomServersColor->palette();
	p.setColor(QPalette::Button, c);
	btnCustomServersColor->setPalette(p);
}

void AppearanceConfigBox::readSettings()
{
	SettingsData *setting;

	setting = config->setting("SlotStyle");
	slotStyle->setCurrentIndex(setting->integer());

	QPalette p = btnCustomServersColor->palette();
	setting = config->setting("CustomServersColor");
	p.setColor(QPalette::Button, QColor( setting->integer() ));
	btnCustomServersColor->setPalette(p);
}

void AppearanceConfigBox::saveSettings()
{
	SettingsData *setting;

	setting = config->setting("SlotStyle");
	setting->setValue(slotStyle->currentIndex());

	QPalette p = btnCustomServersColor->palette();
	setting = config->setting("CustomServersColor");
	setting->setValue(p.color(QPalette::Button).rgb());

	emit appearanceChanged();
}

//------------------------------------------------------------------------------
// wadseekerconfigidgames.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include "gui/wadseekerconfigidgames.h"
#include "wadseeker/wadseeker.h"
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>

WadseekerIdgamesConfigBox::WadseekerIdgamesConfigBox(IniSection* cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	connect(btnIdgamesURLDefault, SIGNAL( clicked() ), this, SLOT( btnIdgamesURLDefaultClicked() ) );
	connect(cbUseIdgames, SIGNAL( toggled(bool) ), this, SLOT( cbUseIdgamesToggled(bool) ) );

	cboIdgamesPriority->addItem("After all sites");
	cboIdgamesPriority->addItem("After custom site");

	cbUseIdgamesToggled(cbUseIdgames->isChecked());
}

void WadseekerIdgamesConfigBox::btnIdgamesURLDefaultClicked()
{
	leIdgamesURL->setText(Wadseeker::defaultIdgamesUrl());
}

void WadseekerIdgamesConfigBox::cbUseIdgamesToggled(bool checked)
{
	frameWithContent->setEnabled(checked);
}

ConfigurationBoxInfo* WadseekerIdgamesConfigBox::createStructure(IniSection* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* cfgBoxInfo = new ConfigurationBoxInfo();
	cfgBoxInfo->confBox = new WadseekerIdgamesConfigBox(cfg, parent);
	cfgBoxInfo->boxName = tr("Idgames");
	cfgBoxInfo->icon = QIcon(":/icons/arrow-down-double.png");
	return cfgBoxInfo;
}

void WadseekerIdgamesConfigBox::readSettings()
{
	cbUseIdgames->setChecked(*config->setting("SearchInIdgames"));
	cboIdgamesPriority->setCurrentIndex(*config->setting("IdgamesPriority"));
	leIdgamesURL->setText(*config->setting("IdgamesURL"));
}

void WadseekerIdgamesConfigBox::saveSettings()
{
	config->setting("SearchInIdgames")->setValue(cbUseIdgames->isChecked());
	config->setting("IdgamesPriority")->setValue(cboIdgamesPriority->currentIndex());
	config->setting("IdgamesURL")->setValue(leIdgamesURL->text());
}

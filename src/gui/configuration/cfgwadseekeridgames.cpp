//------------------------------------------------------------------------------
// cfgwadseekeridgames.cpp
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
#include "cfgwadseekeridgames.h"
#include "configuration/doomseekerconfig.h"
#include "wadseeker/wadseeker.h"
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>

CFGWadseekerIdgames::CFGWadseekerIdgames(QWidget* parent) 
: ConfigurationBaseBox(parent)
{
	setupUi(this);

	connect(btnIdgamesURLDefault, SIGNAL( clicked() ), this, SLOT( btnIdgamesURLDefaultClicked() ) );
	connect(cbUseIdgames, SIGNAL( toggled(bool) ), this, SLOT( cbUseIdgamesToggled(bool) ) );

	cboIdgamesPriority->addItem("After all sites");
	cboIdgamesPriority->addItem("After custom site");

	cbUseIdgamesToggled(cbUseIdgames->isChecked());
}

void CFGWadseekerIdgames::btnIdgamesURLDefaultClicked()
{
	leIdgamesURL->setText(Wadseeker::defaultIdgamesUrl());
}

void CFGWadseekerIdgames::cbUseIdgamesToggled(bool checked)
{
	frameWithContent->setEnabled(checked);
}

void CFGWadseekerIdgames::readSettings()
{
	cbUseIdgames->setChecked(gConfig.wadseeker.bSearchInIdgames);
	cboIdgamesPriority->setCurrentIndex(gConfig.wadseeker.idgamesPriority);
	leIdgamesURL->setText(gConfig.wadseeker.idgamesURL);
}

void CFGWadseekerIdgames::saveSettings()
{
	gConfig.wadseeker.bSearchInIdgames = cbUseIdgames->isChecked();
	gConfig.wadseeker.idgamesPriority = cboIdgamesPriority->currentIndex();
	gConfig.wadseeker.idgamesURL = leIdgamesURL->text();
}

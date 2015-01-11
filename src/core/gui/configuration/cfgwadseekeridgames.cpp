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
#include "ui_cfgwadseekeridgames.h"
#include "configuration/doomseekerconfig.h"
#include "wadseeker/wadseeker.h"
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>

class CFGWadseekerIdgames::PrivData : public Ui::CFGWadseekerIdgames
{
};

CFGWadseekerIdgames::CFGWadseekerIdgames(QWidget* parent)
: ConfigurationBaseBox(parent)
{
	d = new PrivData;
	d->setupUi(this);

	connect(d->btnIdgamesURLDefault, SIGNAL( clicked() ), this, SLOT( btnIdgamesURLDefaultClicked() ) );
	connect(d->cbUseIdgames, SIGNAL( toggled(bool) ), this, SLOT( cbUseIdgamesToggled(bool) ) );

	cbUseIdgamesToggled(d->cbUseIdgames->isChecked());
}

CFGWadseekerIdgames::~CFGWadseekerIdgames()
{
	delete d;
}

void CFGWadseekerIdgames::btnIdgamesURLDefaultClicked()
{
	d->leIdgamesURL->setText(Wadseeker::defaultIdgamesUrl());
}

void CFGWadseekerIdgames::cbUseIdgamesToggled(bool checked)
{
	d->frameWithContent->setEnabled(checked);
}

void CFGWadseekerIdgames::readSettings()
{
	d->cbUseIdgames->setChecked(gConfig.wadseeker.bSearchInIdgames);
	d->leIdgamesURL->setText(gConfig.wadseeker.idgamesURL);
}

void CFGWadseekerIdgames::saveSettings()
{
	gConfig.wadseeker.bSearchInIdgames = d->cbUseIdgames->isChecked();
	gConfig.wadseeker.idgamesURL = d->leIdgamesURL->text();
}

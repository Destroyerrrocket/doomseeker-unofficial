//------------------------------------------------------------------------------
// aboutDlg.cpp
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "aboutDlg.h"
#include "global.h"
#include "main.h"
#include "wadseeker/wadseeker.h"
#include <QPixmap>

AboutDlg::AboutDlg(QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT( close() ));

	// Doomseeker
	versionNumber->setText(VERSION);
	logo->setPixmap(QPixmap(":/logo.png"));

	// Wadseeker
	wadseekerAuthor->setText(Wadseeker::author());
	wadseekerDescription->setText(Wadseeker::description());
	wadseekerVersion->setText(Wadseeker::version());
	wadseekerYearSpan->setText(Wadseeker::yearSpan());

	// Populate plugins dialog
	for(int i = 0;i < Main::enginePlugins->numPlugins();i++)
	{
		pluginBox->addItem( (*Main::enginePlugins)[i]->info->name);
	}
	connect(pluginBox, SIGNAL( currentIndexChanged(int) ), this, SLOT( changePlugin(int) ));
	changePlugin(0);
}

AboutDlg::~AboutDlg()
{
}

void AboutDlg::changePlugin(int plugin)
{
	if(plugin >= Main::enginePlugins->numPlugins())
		return; // Invalid plugin.

	const Plugin* plug = (*Main::enginePlugins)[plugin];

	pluginAuthor->setText(plug->info->author);
	pluginVersion->setText(QString("%1.%2.%3.%4").arg(plug->info->version[0]).arg(plug->info->version[1]).arg(plug->info->version[2]).arg(plug->info->version[3]));
	pluginDescription->setText(plug->info->description);
}

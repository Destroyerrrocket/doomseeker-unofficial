//------------------------------------------------------------------------------
// rconpassworddialog.cpp
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
#include "configuration/doomseekerconfig.h"
#include "rconpassworddialog.h"
#include "plugins/engineplugin.h"
#include "main.h"

RconPasswordDialog::RconPasswordDialog(QWidget *parent, bool connection)
: QDialog(parent)
{
	setupUi(this);

	if (connection)
	{
		// Populate engines box.
		engines->clear();
		for(unsigned int i = 0;i < Main::enginePlugins->numPlugins();i++)
		{
			const EnginePlugin* info = (*Main::enginePlugins)[i]->info;
			engines->addItem(info->icon(), info->data()->name, i);
		}
	}
	else
	{
		connectionBox->hide();
	}

	if(gConfig.doomseeker.bHidePasswords)
		lePassword->setEchoMode(QLineEdit::Password);

	// Adjust the size and prevent resizing.
	adjustSize();
	setMinimumHeight(height());
	setMaximumHeight(height());
}

QString RconPasswordDialog::connectPassword() const
{
	return lePassword->text();
}

const EnginePlugin *RconPasswordDialog::selectedEngine() const
{
	const PluginLoader::Plugin *plugin = (*Main::enginePlugins)[engines->currentIndex()];
	if(plugin == NULL)
		return NULL;

	return plugin->info;
}

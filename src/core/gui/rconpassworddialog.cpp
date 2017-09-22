//------------------------------------------------------------------------------
// rconpassworddialog.cpp
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#include "configuration/doomseekerconfig.h"
#include "rconpassworddialog.h"
#include "ui_rconpassworddialog.h"
#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"

DClass<RconPasswordDialog> : public Ui::RconPasswordDialog
{
};

DPointered(RconPasswordDialog)

RconPasswordDialog::RconPasswordDialog(QWidget *parent, bool connection)
: QDialog(parent)
{
	d->setupUi(this);

	if (connection)
	{
		// Populate engines box.
		d->engines->clear();
		for(unsigned int i = 0;i < gPlugins->numPlugins();i++)
		{
			const EnginePlugin* info = gPlugins->plugin(i)->info();
			d->engines->addItem(info->icon(), info->data()->name, i);
		}
	}
	else
	{
		d->connectionBox->hide();
	}

	if(gConfig.doomseeker.bHidePasswords)
		d->lePassword->setEchoMode(QLineEdit::Password);

	// Adjust the size and prevent resizing.
	adjustSize();
	setMinimumHeight(height());
	setMaximumHeight(height());
}

RconPasswordDialog::~RconPasswordDialog()
{
}

QString RconPasswordDialog::connectPassword() const
{
	return d->lePassword->text();
}

const EnginePlugin *RconPasswordDialog::selectedEngine() const
{
	const PluginLoader::Plugin *plugin = gPlugins->plugin(d->engines->currentIndex());
	if(plugin == NULL)
		return NULL;

	return plugin->info();
}

QString RconPasswordDialog::serverAddress() const
{
	return d->leServerAddress->text();
}

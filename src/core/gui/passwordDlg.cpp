//------------------------------------------------------------------------------
// passwordDlg.cpp
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
#include "passwordDlg.h"
#include "plugins/engineplugin.h"
#include "main.h"

PasswordDlg::PasswordDlg(QWidget *parent, bool rcon, bool connection) : QDialog(parent), rcon(rcon)
{
	setupUi(this);

	if(rcon)
	{
		remember->hide();
		label->setText(tr("Please enter your remote console password."));
	}
	else
	{
		bool bRememberConnectPassword = gConfig.doomseeker.bRememberConnectPassword;
		remember->setChecked(bRememberConnectPassword);
		if(bRememberConnectPassword)
		{
			password->setText(gConfig.doomseeker.connectPassword);
		}
	}

	if(!connection)
	{
		connectionBox->hide();
	}
	else
	{
		// Populate engines box.
		engines->clear();
		for(unsigned int i = 0;i < Main::enginePlugins->numPlugins();i++)
		{
			const EnginePlugin* info = (*Main::enginePlugins)[i]->info;
			engines->addItem(info->icon(), info->data()->name, i);
		}
	}

	if(gConfig.doomseeker.bHidePasswords)
		password->setEchoMode(QLineEdit::Password);

	// Adjust the size and prevent resizing.
	adjustSize();
	setMinimumHeight(height());
	setMaximumHeight(height());

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

PasswordDlg::~PasswordDlg()
{
	if(rcon)
		return;

	gConfig.doomseeker.bRememberConnectPassword = remember->isChecked();
	if(remember->isChecked())
	{
		gConfig.doomseeker.connectPassword = password->text();
	}
}

const EnginePlugin *PasswordDlg::selectedEngine() const
{
	const Plugin *plugin = (*Main::enginePlugins)[engines->currentIndex()];
	if(plugin == NULL)
		return NULL;

	return plugin->info;
}

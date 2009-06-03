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

#include "passwordDlg.h"
#include "main.h"

PasswordDlg::PasswordDlg(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	Main::config->createSetting("RememberConnectPassword", 1);

	SettingsData *rememberConnectPassword = Main::config->setting("RememberConnectPassword");
	remember->setChecked(rememberConnectPassword->integer() != 0);
	if(rememberConnectPassword->integer() != 0)
	{
		SettingsData *storedPassword = Main::config->setting("ConnectPassword");
		password->setText(storedPassword->string());
	}

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

PasswordDlg::~PasswordDlg()
{
	Main::config->setting("RememberConnectPassword")->setValue(remember->isChecked());
	if(remember->isChecked())
		Main::config->setting("ConnectPassword")->setValue(password->text());
}

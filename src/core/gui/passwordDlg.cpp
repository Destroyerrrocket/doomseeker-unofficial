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
#include "configuration/passwordscfg.h"
#include "configuration/serverpassword.h"
#include "passwordDlg.h"
#include "plugins/engineplugin.h"
#include "main.h"

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
	return s1.toLower() < s2.toLower();
}

PasswordDlg::PasswordDlg(QWidget *parent, bool rcon, bool connection) : QDialog(parent), rcon(rcon)
{
	setupUi(this);

	if(rcon)
	{
		btnDelMemorizedConnectPassword->hide();
		remember->hide();
		label->setText(tr("Please enter your remote console password."));
	}
	else
	{
		bool bRememberConnectPassword = gConfig.doomseeker.bRememberConnectPassword;
		remember->setChecked(bRememberConnectPassword);
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
		cboPassword->lineEdit()->setEchoMode(QLineEdit::Password);

	// Adjust the size and prevent resizing.
	adjustSize();
	setMinimumHeight(height());
	setMaximumHeight(height());
}

void PasswordDlg::accept()
{
	if (!rcon)
	{
		gConfig.doomseeker.bRememberConnectPassword = remember->isChecked();
		if (remember->isChecked())
		{
			gConfig.doomseeker.connectPassword = cboPassword->currentText();
		}
	}
	QDialog::accept();
}

QStringList PasswordDlg::allConnectPasswords() const
{
	QStringList items;
	for (int i = 0; i < cboPassword->count(); ++i)
	{
		items << cboPassword->itemText(i);
	}
	return items;
}

QString PasswordDlg::connectPassword() const
{
	return cboPassword->currentText();
}

void PasswordDlg::removeCurrentConnectPassword()
{
	PasswordsCfg cfg;
	cfg.removeServerPhrase(cboPassword->currentText());

	int idx = cboPassword->findText(cboPassword->currentText());
	if (idx >= 0)
	{
		// Simply removing current index won't give proper results
		// if user edits the contents of the combo box.
		cboPassword->removeItem(idx);
	}
	else
	{
		cboPassword->clearEditText();
		// It's intended to have focus change only in case if
		// password wasn't present in the persistence.
		cboPassword->setFocus();
	}
}

const EnginePlugin *PasswordDlg::selectedEngine() const
{
	const PluginLoader::Plugin *plugin = (*Main::enginePlugins)[engines->currentIndex()];
	if(plugin == NULL)
		return NULL;

	return plugin->info;
}

void PasswordDlg::setCurrentConnectPassword(const QString& password)
{
	int idx = cboPassword->findText(password);
	if (idx >= 0)
	{
		cboPassword->setCurrentIndex(idx);
	}
	else
	{
		cboPassword->insertItem(0, gConfig.doomseeker.connectPassword);
		cboPassword->setCurrentIndex(0);
	}
}

void PasswordDlg::setPasswords(const QStringList& passwords)
{
	QStringList passwordsSorted = passwords;
	qSort(passwordsSorted.begin(), passwordsSorted.end(), caseInsensitiveLessThan);
	foreach (const QString& pass, passwordsSorted)
	{
		if (cboPassword->findText(pass) < 0)
		{
			cboPassword->addItem(pass);
		}
	}
}

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
#include "configuration/passwordscfg.h"
#include "configuration/serverpassword.h"
#include "serverapi/server.h"
#include "passwordDlg.h"

#include <QLineEdit>

bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
{
	return s1.toLower() < s2.toLower();
}

class PasswordDlg::PrivData
{
	public:
		const Server* server;
};

PasswordDlg::PasswordDlg(const Server* server, QWidget *parent)
: QDialog(parent)
{
	setupUi(this);
	d = new PrivData();
	d->server = server;

	// Adjust the size and prevent resizing.
	adjustSize();
	setMinimumHeight(height());
	setMaximumHeight(height());

	loadConfiguration();
}

void PasswordDlg::accept()
{
	saveConfiguration();
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

void PasswordDlg::loadConfiguration()
{
	PasswordsCfg cfg;
	if(cfg.isHidingPasswords())
	{
		cboPassword->lineEdit()->setEchoMode(QLineEdit::Password);
	}
	remember->setChecked(cfg.isRememberingConnectPhrase());
	if (cfg.isRememberingConnectPhrase())
	{
		setCurrentConnectPassword(cfg.lastUsedConnectPhrase());
	}
	setPasswords(cfg.serverPhrases());
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

void PasswordDlg::saveConfiguration()
{
	PasswordsCfg cfg;
	cfg.setRememberConnectPhrase(remember->isChecked());
	if (remember->isChecked())
	{
		cfg.saveServerPhrase(connectPassword(), d->server->name(),
			d->server->engineName());
		cfg.setLastUsedConnectPhrase(connectPassword());
	}
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
		cboPassword->insertItem(0, password);
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

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

#include "configuration/passwordscfg.h"
#include "configuration/serverpassword.h"
#include "gui/helpers/comboboxex.h"
#include "serverapi/server.h"

#include <QLineEdit>

class PasswordDlg::PrivData
{
	public:
		ServerCPtr server;
		ComboBoxEx *cboConnectPassEx;
		ComboBoxEx *cboIngamePassEx;
};

PasswordDlg::PasswordDlg(ServerCPtr server, QWidget *parent)
: QDialog(parent)
{
	setupUi(this);
	d = new PrivData();
	d->cboConnectPassEx = new ComboBoxEx(*cboConnectPassword);
	d->cboIngamePassEx = new ComboBoxEx(*cboIngamePassword);
	d->server = server;

	applyInputsVisibility();

	// Adjust the size and prevent resizing.
	adjustSize();
	setMinimumHeight(height());
	setMaximumHeight(height());

	loadConfiguration();
}

PasswordDlg::~PasswordDlg()
{
	delete d->cboConnectPassEx;
	delete d;
}

void PasswordDlg::accept()
{
	saveConfiguration();
	QDialog::accept();
}

void PasswordDlg::applyInputsVisibility()
{
	connectPasswordWidget->setVisible(d->server->isLocked());
	ingamePasswordWidget->setVisible(d->server->isLockedInGame());
}

QString PasswordDlg::connectPassword() const
{
	return cboConnectPassword->currentText();
}

QString PasswordDlg::inGamePassword() const
{
	return cboIngamePassword->currentText();
}

void PasswordDlg::loadConfiguration()
{
	PasswordsCfg cfg;
	if(cfg.isHidingPasswords())
	{
		cboConnectPassword->lineEdit()->setEchoMode(QLineEdit::Password);
		cboIngamePassword->lineEdit()->setEchoMode(QLineEdit::Password);
	}
	remember->setChecked(cfg.isRememberingConnectPhrase());
	setPasswords(cfg.serverPhrases());
	setCurrentConnectPassword(cfg.suggestPassword(d->server.data()).phrase());
	setCurrentIngamePassword(cfg.suggestPassword(d->server.data()).phrase());
}

void PasswordDlg::removeCurrentConnectPassword()
{
	PasswordsCfg cfg;
	QString phrase = cboConnectPassword->currentText();
	cfg.removeServerPhrase(cboConnectPassword->currentText());

	d->cboIngamePassEx->removeItem(phrase);
	if (!d->cboConnectPassEx->removeCurrentItem())
	{
		cboConnectPassword->clearEditText();
		cboConnectPassword->setFocus();
	}
}

void PasswordDlg::removeCurrentIngamePassword()
{
	PasswordsCfg cfg;
	QString phrase = cboIngamePassword->currentText();
	cfg.removeServerPhrase(cboIngamePassword->currentText());

	d->cboConnectPassEx->removeItem(phrase);
	if (!d->cboIngamePassEx->removeCurrentItem())
	{
		cboIngamePassword->clearEditText();
		cboIngamePassword->setFocus();
	}
}

void PasswordDlg::saveConfiguration()
{
	PasswordsCfg cfg;
	cfg.setRememberConnectPhrase(remember->isChecked());
	if (remember->isChecked())
	{
		cfg.saveServerPhrase(connectPassword(), d->server.data());
		cfg.saveServerPhrase(inGamePassword(), d->server.data());
	}
}

void PasswordDlg::setCurrentConnectPassword(const QString& password)
{
	d->cboConnectPassEx->setCurrentOrAddNewAndSelect(password);
}

void PasswordDlg::setCurrentIngamePassword(const QString& password)
{
	d->cboIngamePassEx->setCurrentOrAddNewAndSelect(password);
}

void PasswordDlg::setPasswords(const QStringList& passwords)
{
	d->cboConnectPassEx->setItemsSorted(passwords);
	d->cboIngamePassEx->setItemsSorted(passwords);
}

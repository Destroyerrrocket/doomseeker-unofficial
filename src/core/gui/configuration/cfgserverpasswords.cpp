//------------------------------------------------------------------------------
// cfgserverpasswords.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "cfgserverpasswords.h"

#include "configuration/passwordscfg.h"
#include "configuration/serverpassword.h"
#include <QMap>

const int COL_PASSWORD = 0;
const int COL_LAST_GAME = 1;
const int COL_LAST_SERVER = 2;
const int COL_LAST_TIME = 3;

const QString HIDDEN_PASS = "***";

class CFGServerPasswords::PrivData
{
	public:
		bool bHidingPasswordsMode;
};


CFGServerPasswords::CFGServerPasswords(QWidget* parent)
: ConfigurationBaseBox(parent)
{
	setupUi(this);
	d = new PrivData();
	d->bHidingPasswordsMode = true;
	hidePasswords();
	tablePasswords->sortItems(COL_LAST_TIME, Qt::DescendingOrder);
}

CFGServerPasswords::~CFGServerPasswords()
{
	delete d;
}

void CFGServerPasswords::addPasswordFromLineEdit()
{
	QString phrase = lePassword->text();
	lePassword->clear();
	if (!phrase.isEmpty() && !isPassphraseInTable(phrase))
	{
		ServerPassword password;
		password.setPhrase(phrase);
		addServerPasswordToTable(password);
	}
}

void CFGServerPasswords::addServerPasswordToTable(const ServerPassword& password)
{
	int rowIndex = tablePasswords->rowCount();
	tablePasswords->insertRow(rowIndex);

	QString phrase;
	if (d->bHidingPasswordsMode)
	{
		phrase = HIDDEN_PASS;
	}
	else
	{
		phrase = password.phrase();
	}
	// Disable sorting or bad things may happen.
	tablePasswords->setSortingEnabled(false);

	QTableWidgetItem* phraseItem = new QTableWidgetItem(phrase);
	phraseItem->setData(Qt::UserRole, password.serializeQVariant());
	tablePasswords->setItem(rowIndex, COL_PASSWORD, phraseItem);

	tablePasswords->setItem(rowIndex, COL_LAST_GAME, new QTableWidgetItem(password.lastGame()));
	tablePasswords->setItem(rowIndex, COL_LAST_SERVER, new QTableWidgetItem(password.lastServerName()));

	QTableWidgetItem* timeItem = new QTableWidgetItem(
		password.lastTime().toString("yyyy-MM-dd hh:mm:ss"));
	// Maintain proper date sorting.
	timeItem->setData(Qt::EditRole, password.lastTime());
	tablePasswords->setItem(rowIndex, COL_LAST_TIME, timeItem);

	// Re-enable sorting.
	tablePasswords->setSortingEnabled(true);
}

int CFGServerPasswords::findPassphraseInTable(const QString& phrase)
{
	for (int i = 0; i < tablePasswords->rowCount(); ++i)
	{
		ServerPassword password = serverPasswordFromRow(i);
		if (password.phrase() == phrase)
		{
			return i;
		}
	}
	return -1;
}

void CFGServerPasswords::hidePasswords()
{
	btnRevealHideToggle->setText(tr("Reveal"));
	d->bHidingPasswordsMode = true;
	lePassword->setEchoMode(QLineEdit::Password);
	for (int i = 0; i < tablePasswords->rowCount(); ++i)
	{
		QTableWidgetItem* item = tablePasswords->item(i, COL_PASSWORD);
		item->setText(HIDDEN_PASS);
	}
}

bool CFGServerPasswords::isPassphraseInTable(const QString& phrase)
{
	return findPassphraseInTable(phrase) >= 0;
}

void CFGServerPasswords::saveSettings()
{
	PasswordsCfg cfg;
	QList<ServerPassword> passwords;
	for (int i = 0; i < tablePasswords->rowCount(); ++i)
	{
		passwords << serverPasswordFromRow(i);
	}
	cfg.setServerPasswords(passwords);
}

ServerPassword CFGServerPasswords::serverPasswordFromRow(int row)
{
	if (row < 0 || row >= tablePasswords->rowCount())
	{
		return ServerPassword();
	}
	return ServerPassword::deserializeQVariant(
		tablePasswords->item(row, COL_PASSWORD)->data(Qt::UserRole));
}

void CFGServerPasswords::readSettings()
{
	PasswordsCfg cfg;
	foreach (const ServerPassword& pass, cfg.serverPasswords())
	{
		addServerPasswordToTable(pass);
	}
}

void CFGServerPasswords::removeSelectedPasswords()
{
	// Rows can't be just deleted with items from selectedItems()
	// because the program will crash. This solution is so stupid
	// that there must be another one, but nobody knows...
	QMap<int, QTableWidgetItem*> uniqueRowsItems;
	foreach (QTableWidgetItem* item, tablePasswords->selectedItems())
	{
		uniqueRowsItems.insert(item->row(), item);
	}
	foreach (QTableWidgetItem* item, uniqueRowsItems.values())
	{
		int row = tablePasswords->row(item);
		if (row >= 0)
		{
			tablePasswords->removeRow(row);
		}
	}
}

void CFGServerPasswords::revealPasswords()
{
	btnRevealHideToggle->setText(tr("Hide"));
	lePassword->setEchoMode(QLineEdit::Normal);
	d->bHidingPasswordsMode = false;
	for (int i = 0; i < tablePasswords->rowCount(); ++i)
	{
		QTableWidgetItem* item = tablePasswords->item(i, COL_PASSWORD);
		item->setText(serverPasswordFromRow(i).phrase());
	}
}

void CFGServerPasswords::toggleRevealHide()
{
	if (d->bHidingPasswordsMode)
	{
		revealPasswords();
	}
	else
	{
		hidePasswords();
	}
}

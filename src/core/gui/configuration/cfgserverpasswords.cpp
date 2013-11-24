//------------------------------------------------------------------------------
// cfgserverpasswords.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "cfgserverpasswords.h"

#include "configuration/passwordscfg.h"
#include "configuration/serverpassword.h"
#include "gui/helpers/datetablewidgetitem.h"
#include <QMap>

const int COL_PASS_PASSWORD = 0;
const int COL_PASS_LAST_GAME = 1;
const int COL_PASS_LAST_SERVER = 2;
const int COL_PASS_LAST_TIME = 3;

const int COL_SERV_GAME = 0;
const int COL_SERV_NAME = 1;
const int COL_SERV_ADDRESS = 2;
const int COL_SERV_LAST_TIME = 3;

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
	tablePasswords->sortItems(COL_PASS_LAST_TIME, Qt::DescendingOrder);
	tablePasswords->setColumnWidth(COL_PASS_PASSWORD, 90);
	tableServers->sortItems(COL_SERV_GAME, Qt::AscendingOrder);
	tableServers->setColumnWidth(COL_SERV_GAME, 90);
	lblServerLossWarning->setVisible(false);
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
	setPasswordInRow(rowIndex, password);
}

void CFGServerPasswords::clearTable(QTableWidget* table)
{
	while (table->rowCount() > 0)
	{
		table->removeRow(0);
	}
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
		QTableWidgetItem* item = tablePasswords->item(i, COL_PASS_PASSWORD);
		item->setText(HIDDEN_PASS);
		item->setToolTip(HIDDEN_PASS);
	}
}

bool CFGServerPasswords::isPassphraseInTable(const QString& phrase)
{
	return findPassphraseInTable(phrase) >= 0;
}

void CFGServerPasswords::onPasswordTableCellChange(int currentRow, int currentColumn,
	int previousRow, int previousColumn)
{
	if (currentRow != previousRow)
	{
		// Setting an invalid password will clear the table which is
		// what we want.
		setServersInTable(serverPasswordFromRow(currentRow));
	}
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
	cfg.setMaxNumberOfServersPerPassword(spinMaxPasswords->value());
}

ServerPassword CFGServerPasswords::serverPasswordFromRow(int row)
{
	if (row < 0 || row >= tablePasswords->rowCount())
	{
		return ServerPassword();
	}
	return ServerPassword::deserializeQVariant(
		tablePasswords->item(row, COL_PASS_PASSWORD)->data(Qt::UserRole));
}

void CFGServerPasswords::setPasswordInRow(int row, const ServerPassword& password)
{
	// Disable sorting or bad things may happen.
	bool wasSortingEnabled = tablePasswords->isSortingEnabled();
	tablePasswords->setSortingEnabled(false);

	QString phrase;
	if (d->bHidingPasswordsMode)
	{
		phrase = HIDDEN_PASS;
	}
	else
	{
		phrase = password.phrase();
	}

	QTableWidgetItem* phraseItem = toolTipItem(phrase);
	phraseItem->setData(Qt::UserRole, password.serializeQVariant());
	tablePasswords->setItem(row, COL_PASS_PASSWORD, phraseItem);

	tablePasswords->setItem(row, COL_PASS_LAST_GAME, toolTipItem(password.lastGame()));
	tablePasswords->setItem(row, COL_PASS_LAST_SERVER, toolTipItem(password.lastServerName()));

	DateTableWidgetItem* dateItem = new DateTableWidgetItem(password.lastTime());
	dateItem->setToolTip(dateItem->displayedText());
	tablePasswords->setItem(row, COL_PASS_LAST_TIME, dateItem);

	// Re-enable sorting if was enabled before.
	tablePasswords->setSortingEnabled(wasSortingEnabled);
}

void CFGServerPasswords::setServersInTable(const ServerPassword& password)
{
	clearTable(tableServers);
	// Disable sorting or bad things may happen.
	tableServers->setSortingEnabled(false);
	foreach (const ServerPassword::Server& server, password.servers())
	{
		int rowIndex = tableServers->rowCount();
		tableServers->insertRow(rowIndex);

		QTableWidgetItem* gameItem = toolTipItem(server.game());
		gameItem->setData(Qt::UserRole, server.serializeQVariant());
		tableServers->setItem(rowIndex, COL_SERV_GAME, gameItem);

		tableServers->setItem(rowIndex, COL_SERV_NAME, toolTipItem(server.name()));
		QString address = QString("%1:%2").arg(server.address()).arg(server.port());
		tableServers->setItem(rowIndex, COL_SERV_ADDRESS, toolTipItem(address));

		DateTableWidgetItem* dateItem = new DateTableWidgetItem(server.time());
		dateItem->setToolTip(dateItem->displayedText());
		tableServers->setItem(rowIndex, COL_SERV_LAST_TIME, dateItem);
	}
	// Re-enable sorting.
	tableServers->setSortingEnabled(true);
}

void CFGServerPasswords::showServerLossWarningIfNecessary()
{
	PasswordsCfg cfg;
	lblServerLossWarning->setVisible(spinMaxPasswords->value() <
		cfg.maxNumberOfServersPerPassword());
}

void CFGServerPasswords::readSettings()
{
	clearTable(tablePasswords);
	clearTable(tableServers);

	PasswordsCfg cfg;
	foreach (const ServerPassword& pass, cfg.serverPasswords())
	{
		addServerPasswordToTable(pass);
	}
	spinMaxPasswords->setValue(cfg.maxNumberOfServersPerPassword());
}

void CFGServerPasswords::removeSelected(QTableWidget* table)
{
	// Rows can't be just deleted with items from selectedItems()
	// because the program will crash. This solution is so stupid
	// that there must be another one, but nobody knows...
	QMap<int, QTableWidgetItem*> uniqueRowsItems;
	foreach (QTableWidgetItem* item, table->selectedItems())
	{
		uniqueRowsItems.insert(item->row(), item);
	}
	foreach (QTableWidgetItem* item, uniqueRowsItems.values())
	{
		int row = table->row(item);
		if (row >= 0)
		{
			table->removeRow(row);
		}
	}
}

void CFGServerPasswords::removeSelectedPasswords()
{
	removeSelected(tablePasswords);
}

void CFGServerPasswords::removeSelectedServers()
{
	QList<ServerPassword::Server> servers;
	foreach (QTableWidgetItem* item, tableServers->selectedItems())
	{
		if (item->column() == COL_SERV_GAME)
		{
			servers << ServerPassword::Server::deserializeQVariant(item->data(Qt::UserRole));
		}
	}
	ServerPassword currentPassword = serverPasswordFromRow(tablePasswords->currentRow());
	foreach (const ServerPassword::Server& server, servers)
	{
		currentPassword.removeServer(server.game(), server.address(), server.port());
	}
	updatePassword(currentPassword);
	removeSelected(tableServers);
}

void CFGServerPasswords::revealPasswords()
{
	btnRevealHideToggle->setText(tr("Hide"));
	lePassword->setEchoMode(QLineEdit::Normal);
	d->bHidingPasswordsMode = false;
	for (int i = 0; i < tablePasswords->rowCount(); ++i)
	{
		QTableWidgetItem* item = tablePasswords->item(i, COL_PASS_PASSWORD);
		ServerPassword password = serverPasswordFromRow(i);
		item->setText(password.phrase());
		item->setToolTip(password.phrase());
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

QTableWidgetItem* CFGServerPasswords::toolTipItem(const QString& contents)
{
	QTableWidgetItem* item = new QTableWidgetItem(contents);
	item->setToolTip(contents);
	return item;
}

void CFGServerPasswords::updatePassword(const ServerPassword& password)
{
	int row = findPassphraseInTable(password.phrase());
	if (row >= 0)
	{
		setPasswordInRow(row, password);
	}
	else
	{
		addServerPasswordToTable(password);
	}
}

//------------------------------------------------------------------------------
// ircignoresmanager.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ircignoresmanager.h"
#include "ui_ircignoresmanager.h"

#include <QKeyEvent>
#include "irc/configuration/chatnetworkscfg.h"
#include "irc/entities/ircnetworkentity.h"
#include "patternlist.h"

class IRCIgnoresManager::PrivData : public Ui::IRCIgnoresManager
{
public:
	QString networkDescription;
};


IRCIgnoresManager::IRCIgnoresManager(QWidget *parent, const QString &networkDescription)
: QDialog(parent)
{
	d = new PrivData();
	d->networkDescription = networkDescription;
	d->setupUi(this);

	loadItems();
}

IRCIgnoresManager::~IRCIgnoresManager()
{
	delete d;
}

void IRCIgnoresManager::done(int result)
{
	if (result == Accepted)
	{
		saveItems();
	}
	QDialog::done(result);
}

void IRCIgnoresManager::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Delete)
	{
		deleteSelected();
	}
	QDialog::keyPressEvent(event);
}

void IRCIgnoresManager::loadItems()
{
	ChatNetworksCfg cfg;
	IRCNetworkEntity network = cfg.network(d->networkDescription);
	foreach (const QRegExp &pattern, network.ignoredUsers())
	{
		QListWidgetItem *item = new QListWidgetItem(pattern.pattern(), d->list);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		d->list->addItem(item);
	}
}

void IRCIgnoresManager::saveItems()
{
	ChatNetworksCfg cfg;
	IRCNetworkEntity network = cfg.network(d->networkDescription);
	network.setIgnoredUsers(patterns());
	cfg.replaceNetwork(network.description(), network, this);
}

PatternList IRCIgnoresManager::patterns() const
{
	PatternList result;
	for (int row = 0; row < d->list->count(); ++row)
	{
		QString text = d->list->item(row)->text();
		if (!text.trimmed().isEmpty())
		{
			result << QRegExp(text, Qt::CaseInsensitive, QRegExp::Wildcard);
		}
	}
	return result;
}

void IRCIgnoresManager::deleteSelected()
{
	QList<QListWidgetItem*> selection = d->list->selectedItems();
	foreach (QListWidgetItem *item, selection)
	{
		d->list->removeItemWidget(item);
	}
	qDeleteAll(selection);
}

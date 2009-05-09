//------------------------------------------------------------------------------
// dockBuddiesList.cpp
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

#include "dockBuddiesList.h"
#include <QMessageBox>
#include <QRegExp>

DockBuddiesList::BuddyInfo &DockBuddiesList::BuddyInfo::operator= (const DockBuddiesList::BuddyInfo &other)
{
	player = other.buddy();
	server = other.location();
}

DockBuddiesList::DockBuddiesList(QWidget *parent) : QDockWidget(parent), mc(NULL)
{
	setupUi(this);

	// Set up the model
	buddiesTableModel = new QStandardItemModel(buddiesTable);

	// Not working yet.
	/*QStringList columns;
	columns << tr("Buddy");
	columns << tr("Location");
	buddiesTableModel->setHorizontalHeaderLabels(columns);*/

	buddiesTable->setModel(buddiesTableModel);

	connect(addButton, SIGNAL( clicked() ), this, SLOT( addBuddy() ));
}

void DockBuddiesList::addBuddy()
{
	AddBuddyDlg dlg;
	int result = dlg.exec();
	if(result != QDialog::Accepted)
		return;

	QRegExp pattern(dlg.pattern(), Qt::CaseSensitive, dlg.patternType() == AddBuddyDlg::PT_BASIC ? QRegExp::Wildcard : QRegExp::RegExp);

	if(pattern.isValid())
	{
		pBuddies << pattern;
		scan(mc);
	}

	patternsList->addItem(dlg.pattern());
}

void DockBuddiesList::scan(const MasterClient *master)
{
	mc = master;
	if(mc == NULL)
		return;

	foreach(Server *server, master->serverList())
	{
		if(server->numPlayers() <= 0)
			continue;

		for(int i = 0;i < server->numPlayers();i++)
		{
			const Player player = server->player(i);
			foreach(QRegExp pattern, pBuddies)
			{
				if(pattern.exactMatch(player.nameColorTagsStripped()))
				{
					buddies << BuddyInfo(player, server);
				}
			}
		}
	}

	// Populate list
	buddiesTableModel->clear();
	foreach(BuddyInfo info, buddies)
	{
		QList<QStandardItem *> columns;

		for(int i = 0;i < HOW_MANY_BUDDIESLIST_COLUMNS;i++)
		{
			switch(i)
			{
				case BLCID_BUDDY:
					columns << new QStandardItem(info.buddy().nameColorTagsStripped());
					break;
				case BLCID_LOCATION:
					columns << new QStandardItem(info.location()->name());
					break;
				default:
					columns << new QStandardItem();
					break;
			}
		}

		buddiesTableModel->appendRow(columns);
	}
}

////////////////////////////////////////////////////////////////////////////////

AddBuddyDlg::AddBuddyDlg(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT( buttonBoxClicked(QAbstractButton *) ));
}

void AddBuddyDlg::buttonBoxClicked(QAbstractButton *button)
{
	if(buttonBox->standardButton(button) == QDialogButtonBox::Ok)
	{
		if(patternType() == PT_ADVANCED)
		{
			QRegExp test(pattern());
			if(!test.isValid())
			{
				QMessageBox::information(this, tr("Invalid Pattern"), tr("The pattern you have specified is not a valid regular expression."));
				return;
			}
		}
		accept();
	}
	else
		reject();
}

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
#include <QMenu>
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
	buddiesTableModel = new QStandardItemModel();

	// Not working yet.
	QStringList columns;
	columns << tr("Buddy");
	columns << tr("Location");
	buddiesTableModel->setHorizontalHeaderLabels(columns);

	buddiesTable->setModel(buddiesTableModel);

	connect(addButton, SIGNAL( clicked() ), this, SLOT( addBuddy() ));
	connect(deleteButton, SIGNAL( clicked() ), this, SLOT( deleteBuddy() ));
	connect(patternsList, SIGNAL( customContextMenuRequested(const QPoint &) ), this, SLOT( patternsListContextMenu(const QPoint &) ));
}

void DockBuddiesList::addBuddy()
{
	AddBuddyDlg dlg;
	int result = dlg.exec();
	if(result != QDialog::Accepted)
		return;

	QRegExp pattern(dlg.pattern(), Qt::CaseInsensitive, dlg.patternType() == AddBuddyDlg::PT_BASIC ? QRegExp::Wildcard : QRegExp::RegExp);

	if(pattern.isValid())
	{
		pBuddies << pattern;
		scan(mc);
	}

	patternsList->addItem(dlg.pattern());
}

void DockBuddiesList::deleteBuddy()
{
	// Do nothing if nothing is selected.
	if(patternsList->selectionModel()->selectedIndexes().size() <= 0)
		return;

	if(QMessageBox::warning(this, tr("Delete Buddy"), tr("Are you sure you want to delete this pattern?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)
		!= QMessageBox::Yes)
		return;

	QModelIndexList selection = patternsList->selectionModel()->selectedIndexes();
	// We have to remove the bottom most row until they are all gone.
	while(selection.size() > 0)
	{
		int largestIndex = 0;
		for(int i = 1;i < selection.size();i++)
		{
			if(selection[i].row() > selection[largestIndex].row())
				largestIndex = i;
		}

		pBuddies.removeAt(selection[largestIndex].row());
		delete patternsList->item(selection[largestIndex].row());
		selection.removeAt(largestIndex); // remove index
	}

	scan(mc);
}

void DockBuddiesList::patternsListContextMenu(const QPoint &pos)
{
	// First lets get the selection since that will determine the menu.
	QModelIndexList selection = patternsList->selectionModel()->selectedIndexes();

	QMenu context;

	// Always have the add function.
	QAction *addAction = context.addAction(tr("Add"));
	connect(addAction, SIGNAL( triggered() ), this, SLOT( addBuddy() ));

	// if anything is selected allow the user to delete them.
	if(selection.size() > 0)
	{
		QAction *deleteAction = context.addAction(tr("Delete"));
		connect(deleteAction, SIGNAL( triggered() ), this, SLOT( deleteBuddy() ));
	}

	// Finally show our menu.
	context.exec(patternsList->mapToGlobal(pos));
}

void DockBuddiesList::scan(const MasterClient *master)
{
	if(master == NULL && mc == NULL)
		return;
	else if(master != mc && master != NULL)
	{ // If the master is new
		mc = master;
		connect(mc, SIGNAL( listUpdated() ), this, SLOT( scan() ));
	}

	buddies.clear(); //empty list
	foreach(Server *server, mc->serverList())
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
	buddiesTableModel->removeRows(0, buddiesTableModel->rowCount());
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

	// Fits rows to contents
	buddiesTable->resizeRowsToContents();
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

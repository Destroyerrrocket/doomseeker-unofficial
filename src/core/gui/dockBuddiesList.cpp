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
#include "configuration/doomseekerconfig.h"
#include "serverapi/mastermanager.h"
#include "serverapi/playerslist.h"
#include "serverapi/server.h"
#include "strings.h"
#include <QMenu>
#include <QMessageBox>
#include <QRegExp>

DockBuddiesList::DockBuddiesList(QWidget *parent)
: QDockWidget(parent), masterClient(NULL), save(false)
{
	setupUi(this);
	this->toggleViewAction()->setIcon(QIcon(":/icons/buddies.png"));

	// Set up the model
	buddiesTableModel = new QStandardItemModel();

	// Not working yet.
	QStringList columns;
	columns << tr("ID");
	columns << tr("Buddy");
	columns << tr("Location");
	buddiesTableModel->setHorizontalHeaderLabels(columns);

	buddiesTable->setModel(buddiesTableModel);
	buddiesTable->setColumnHidden(0, true); // Hide the ID

	// Read config
	const QVector<BuddyInfo>& buddies = gConfig.doomseeker.buddiesList;

	foreach (const BuddyInfo& buddy, buddies)
	{
		pBuddies << buddy.pattern;
		patternsList->addItem(buddy.pattern.pattern());
	}

	connect(addButton, SIGNAL( clicked() ), this, SLOT( addBuddy() ));
	connect(buddiesTable, SIGNAL( doubleClicked(const QModelIndex &) ), this, SLOT( followBuddy(const QModelIndex &) ));
	connect(deleteButton, SIGNAL( clicked() ), this, SLOT( deleteBuddy() ));
	connect(patternsList, SIGNAL( customContextMenuRequested(const QPoint &) ), this, SLOT( patternsListContextMenu(const QPoint &) ));
}

DockBuddiesList::~DockBuddiesList()
{
	// See if we made any modification since modifying the setting will cause a
	// write cycle.
	if(!save)
	{
		return;
	}

	gConfig.doomseeker.buddiesList.clear();
	foreach (QRegExp pattern, pBuddies)
	{
		BuddyInfo buddyInfo;

		buddyInfo.pattern = pattern;
		buddyInfo.patternType = pattern.patternSyntax() == QRegExp::Wildcard ? BuddyInfo::PT_BASIC : BuddyInfo::PT_ADVANCED;

		gConfig.doomseeker.buddiesList << buddyInfo;
	}
}

void DockBuddiesList::addBuddy()
{
	AddBuddyDlg dlg;
	int result = dlg.exec();
	if(result != QDialog::Accepted)
		return;

	QRegExp pattern(dlg.pattern(), Qt::CaseInsensitive, dlg.patternType() == BuddyInfo::PT_BASIC ? QRegExp::Wildcard : QRegExp::RegExp);

	if(pattern.isValid())
	{
		pBuddies << pattern;
		scan(masterClient);
	}

	patternsList->addItem(dlg.pattern());

	save = true;
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

	scan(masterClient);

	save = true;
}

void DockBuddiesList::followBuddy(const QModelIndex &index)
{
	// Folow the buddy into the server.
	QString error;

	ServerPtr server = buddies[buddiesTableModel->item(index.row(), BLCID_ID)->data().toInt()].location();
	emit joinServer(server);
}

void DockBuddiesList::patternsListContextMenu(const QPoint &pos) const
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

void DockBuddiesList::scan(const MasterManager *master)
{
	if(master == NULL && masterClient == NULL)
		return;
	else if(master != masterClient && master != NULL)
	{ // If the master is new
		masterClient = master;
	}

	buddies.clear(); //empty list
	foreach(ServerPtr server, masterClient->allServers())
	{
		for(int i = 0; i < server->players().numClients(); ++i)
		{
			const Player player = server->player(i);
			foreach(QRegExp pattern, pBuddies)
			{
				if(pattern.exactMatch(player.nameColorTagsStripped()))
				{
					buddies << BuddyLocationInfo(player, server);
				}
			}
		}
	}

	// Populate list
	buddiesTableModel->removeRows(0, buddiesTableModel->rowCount());
	for(int i = 0;i < buddies.size();i++)
	{
		const BuddyLocationInfo &info = buddies[i];
		QList<QStandardItem *> columns;

		for(int j = 0;j < HOW_MANY_BUDDIESLIST_COLUMNS;j++)
		{
			switch(j)
			{
				case BLCID_ID:
				{
					QStandardItem *item = new QStandardItem();
					item->setData(i);
					columns << item;
					break;
				}
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
		if(patternType() == BuddyInfo::PT_ADVANCED)
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

////////////////////////////////////////////////////////////////////////////////

class DockBuddiesList::BuddyLocationInfo::PrivData
{
	public:
		Player buddy;
		ServerPtr server;
};

COPYABLE_D_POINTERED_INNER_DEFINE(DockBuddiesList::BuddyLocationInfo, BuddyLocationInfo);

DockBuddiesList::BuddyLocationInfo::BuddyLocationInfo(const Player &buddy, ServerPtr location)
{
	d = new PrivData();
	d->buddy = buddy;
	d->server = location;
}

DockBuddiesList::BuddyLocationInfo::~BuddyLocationInfo()
{
	delete d;
}

const Player &DockBuddiesList::BuddyLocationInfo::buddy() const
{
	return d->buddy;
}

ServerPtr DockBuddiesList::BuddyLocationInfo::location() const
{
	return d->server;
}

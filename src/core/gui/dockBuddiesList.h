//------------------------------------------------------------------------------
// dockBuddiesList.h
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

#ifndef __DOCKBUDDIESLIST_H__
#define __DOCKBUDDIESLIST_H__

#include "global.h"
#include "serverapi/buddyinfo.h"
#include "serverapi/serverptr.h"
#include "patternlist.h"
#include "ui_dockBuddiesList.h"
#include "ui_addBuddyDlg.h"

#include <QList>
#include <QRegExp>
#include <QStandardItemModel>

class MasterManager;
class Player;
class Server;

class DockBuddiesList : public QDockWidget, private Ui::DockBuddiesList
{
	Q_OBJECT

	public:
		enum ColumnId
		{
			BLCID_ID,
			BLCID_BUDDY,
			BLCID_LOCATION,

			HOW_MANY_BUDDIESLIST_COLUMNS
		};

		DockBuddiesList(QWidget *parent=NULL);
		~DockBuddiesList();

	public slots:
		void addBuddy();
		void scan(const MasterManager *master=NULL);

	signals:
		void joinServer(const ServerPtr &server);

	protected:
		class BuddyLocationInfo
		{
			public:
				BuddyLocationInfo(const Player &buddy, ServerPtr location);
				COPYABLE_D_POINTERED_DECLARE(BuddyLocationInfo);
				~BuddyLocationInfo();

				const Player &buddy() const;
				ServerPtr location() const;

				bool operator==(const BuddyLocationInfo &other) const;

			private:
				class PrivData;
				PrivData *d;
		};

		QList<DockBuddiesList::BuddyLocationInfo> buddies;
		QStandardItemModel *buddiesTableModel;
		PatternList pBuddies;

	protected slots:
		void deleteBuddy();
		void followBuddy(const QModelIndex &index);
		void patternsListContextMenu(const QPoint &pos) const;

	private:
		const MasterManager *masterClient;
		bool save;
};

class AddBuddyDlg : public QDialog, private Ui::AddBuddyDlg
{
	Q_OBJECT

	public:
		AddBuddyDlg(QWidget *parent=NULL);

		BuddyInfo::PatternType patternType() const { return basicPattern->isChecked() ? BuddyInfo::PT_BASIC : BuddyInfo::PT_ADVANCED; }
		QString pattern() const { return patternBox->text(); }

	protected slots:
		void buttonBoxClicked(QAbstractButton *button);
};

#endif /* __DOCKBUDDIESLIST_H__ */

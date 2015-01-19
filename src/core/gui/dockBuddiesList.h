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

#include "serverapi/buddyinfo.h"
#include "serverapi/serverptr.h"

#include <QDialog>
#include <QDockWidget>

class MasterManager;
class QAbstractButton;
class QModelIndex;

class DockBuddiesList : public QDockWidget
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
		class BuddyLocationInfo;

	protected slots:
		void deleteBuddy();
		void followBuddy(const QModelIndex &index);
		void patternsListContextMenu(const QPoint &pos) const;

	private:
		class PrivData;
		PrivData *d;

		const MasterManager *masterClient;
		bool save;
};

class AddBuddyDlg : public QDialog
{
	Q_OBJECT

	public:
		AddBuddyDlg(QWidget *parent=NULL);
		~AddBuddyDlg();

		BuddyInfo::PatternType patternType() const;
		QString pattern() const;

	protected slots:
		void buttonBoxClicked(QAbstractButton *button);

	private:
		class PrivData;
		PrivData *d;
};

#endif /* __DOCKBUDDIESLIST_H__ */

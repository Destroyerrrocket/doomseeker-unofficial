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

#include "server.h"
#include "masterclient.h"
#include "ui_dockBuddiesList.h"
#include "ui_addBuddyDlg.h"

#include <QList>
#include <QRegExp>
#include <QStandardItemModel>

class DockBuddiesList : public QDockWidget, private Ui::DockBuddiesList
{
	Q_OBJECT

	public:
		enum ColumnId
		{
			BLCID_BUDDY,
			BLCID_LOCATION,

			HOW_MANY_BUDDIESLIST_COLUMNS
		};

		DockBuddiesList(QWidget *parent=NULL);

	public slots:
		void	addBuddy();
		void	deleteBuddy();
		void	patternsListContextMenu(const QPoint &pos);
		void	scan(const MasterClient *master=NULL);

	protected:
		struct BuddyInfo
		{
			public:
				BuddyInfo(const Player &buddy, const Server *location) : player(buddy), server(location) {}

				const Player	&buddy() const { return player; }
				const Server	*location() const { return server; }
				BuddyInfo		&operator= (const BuddyInfo &other);

			private:
				Player			player;
				const Server	*server;
		}; 

		QList<DockBuddiesList::BuddyInfo>	buddies;
		QStandardItemModel					*buddiesTableModel;
		QList<QRegExp>						pBuddies;

	private:
		const MasterClient					*mc;
};

class AddBuddyDlg : public QDialog, private Ui::AddBuddyDlg
{
	Q_OBJECT

	public:
		enum PatternType
		{
			PT_BASIC,
			PT_ADVANCED
		};

		AddBuddyDlg(QWidget *parent=NULL);

		PatternType	patternType() { return basicPattern->isChecked() ? PT_BASIC : PT_ADVANCED; }
		QString		pattern() { return patternBox->text(); }

	protected slots:
		void		buttonBoxClicked(QAbstractButton *button);
};

#endif /* __DOCKBUDDIESLIST_H__ */

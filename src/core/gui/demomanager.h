//------------------------------------------------------------------------------
// demomanager.h
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
// Copyright (C) 2011 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#ifndef __DEMOMANAGER_H__
#define __DEMOMANAGER_H__

#include "ui_demomanager.h"
#include "global.h"

#include <QDateTime>
#include <QList>

class QLabel;
class QModelIndex;
class QStandardItemModel;

/**
 *	@brief Dialog for managing demos recorded through Doomseeker.
 */
class DemoManagerDlg : public QDialog, private Ui::DemoManagerDlg
{
	Q_OBJECT

	public:
		DemoManagerDlg();

	protected:
		class Demo
		{
			public:
				QString		filename;
				QString		port;
				QDateTime	time;
				QStringList	wads;
		};

		void	adjustDemoList();
		bool	doRemoveDemo(const QString &file);

		Demo					*selectedDemo;
		QStandardItemModel		*demoModel;
		QList<QList<Demo> >		demoTree;

	protected slots:
		void	performAction(QAbstractButton *button);
		void	updatePreview(const QModelIndex &index);
};

#endif

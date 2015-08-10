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

#include "global.h"
#include "dptr.h"

#include <QDialog>

class QAbstractButton;
class QModelIndex;

/**
 *	@brief Dialog for managing demos recorded through Doomseeker.
 */
class DemoManagerDlg : public QDialog
{
	Q_OBJECT

	public:
		DemoManagerDlg();
		~DemoManagerDlg();

	private slots:
		void deleteSelected();
		void exportSelected();
		void playSelected();
		void performAction(QAbstractButton *button);
		void updatePreview(const QModelIndex &index);

	private:
		void adjustDemoList();
		bool doRemoveDemo(const QString &file);

		DPtr<DemoManagerDlg> d;
};

#endif

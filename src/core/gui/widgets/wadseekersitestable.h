//------------------------------------------------------------------------------
// wadseekersitestable.h
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
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __WADSEEKERSITESTABLE_H__
#define __WADSEEKERSITESTABLE_H__

#include <QTableWidget>

class WadseekerSitesTable : public QTableWidget
{
	Q_OBJECT;

	public:
		static const int IDX_URL_COLUMN = 0;
		static const int IDX_PROGRESS_COLUMN = 1;

		WadseekerSitesTable(QWidget* pParent = NULL);

		void addUrl(const QUrl& url);
		void removeUrl(const QUrl& url);

	public slots:
		void setUrlProgress(const QUrl& url, qint64 current, qint64 total);

	protected:
		void showEvent(QShowEvent* pEvent);

	private:
		class PrivData
		{
			public:
				bool bAlreadyShownOnce;
		};

		PrivData d;

		int findUrlRow(const QUrl& url);
};

#endif

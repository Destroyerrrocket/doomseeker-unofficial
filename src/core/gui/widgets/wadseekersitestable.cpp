//------------------------------------------------------------------------------
// wadseekersitestable.cpp
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
#include "wadseekersitestable.h"

#include <QHeaderView>
#include <QProgressBar>
#include <QUrl>

WadseekerSitesTable::WadseekerSitesTable(QWidget* pParent)
: QTableWidget(pParent)
{
	d.bAlreadyShownOnce = false;
}

void WadseekerSitesTable::addUrl(const QUrl& url)
{
	// Add new row to table, but only if URL is not yet added.
	if (findUrlRow(url) < 0)
	{
		insertRow(rowCount());
		int rowIndex = rowCount() - 1;

		// Create the row contents.
		setSortingEnabled(false);

		QProgressBar* pBar = new QProgressBar();
		pBar->setMinimum(0);
		pBar->setMaximum(0);

		setItem(rowIndex, IDX_URL_COLUMN, new QTableWidgetItem(url.toString()));
		setCellWidget(rowIndex, IDX_PROGRESS_COLUMN, pBar);

		setSortingEnabled(true);
	}
}

int WadseekerSitesTable::findUrlRow(const QUrl& url)
{
	QList<QTableWidgetItem *> list = findItems(url.toString(), Qt::MatchFixedString);
	if (!list.isEmpty())
	{
		return list.first()->row();
	}

	return -1;
}

void WadseekerSitesTable::removeUrl(const QUrl& url)
{
	int row = findUrlRow(url);

	if (row >= 0)
	{
		this->removeRow(row);
	}
}

void WadseekerSitesTable::setUrlProgress(const QUrl& url, qint64 current, qint64 total)
{
	int row = findUrlRow(url);

	if (row >= 0)
	{
		QProgressBar* pBar = (QProgressBar*) this->cellWidget(row, IDX_PROGRESS_COLUMN);
		pBar->setMaximum(total);
		pBar->setValue(current);
	}
}

void WadseekerSitesTable::showEvent(QShowEvent* pEvent)
{
	if (!d.bAlreadyShownOnce)
	{
		// Events in this block must occur after the widget has been
		// constructed, but only once.
		QHeaderView* pHeader = horizontalHeader();

		// Setup resizing
		pHeader->setResizeMode(IDX_URL_COLUMN, QHeaderView::Stretch);

		pHeader->resizeSection(IDX_PROGRESS_COLUMN, 85);
	}
}

//------------------------------------------------------------------------------
// wadseekerwadstable.cpp
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
#include "wadseekerwadstable.h"

#include <QHeaderView>
#include <QProgressBar>

WadseekerWadsTable::WadseekerWadsTable(QWidget* pParent)
: QTableWidget(pParent)
{
	d.bAlreadyShownOnce = false;
}

void WadseekerWadsTable::addFile(const QString& filename)
{
	// Add new row to table, but only if file is not yet added.
	if (findFileRow(filename) < 0)
	{
		insertRow(rowCount());
		int rowIndex = rowCount() - 1;

		// Create the row contents.
		setSortingEnabled(false);

		setItem(rowIndex, IDX_NAME_COLUMN, new QTableWidgetItem(filename));
		setItem(rowIndex, IDX_URL_COLUMN, new QTableWidgetItem());
		setCellWidget(rowIndex, IDX_PROGRESS_COLUMN, new QProgressBar());

		setSortingEnabled(true);
	}
}

int WadseekerWadsTable::findFileRow(const QString& filename)
{
	QList<QTableWidgetItem *> list = findItems(filename, Qt::MatchFixedString);
	if (!list.isEmpty())
	{
		return list.first()->row();
	}

	return -1;
}

void WadseekerWadsTable::setFileProgress(const QString& filename, qint64 current, qint64 total)
{
	// TODO
	// Implement
}

void WadseekerWadsTable::setFileUrl(const QString& filename, const QUrl& url)
{
	// TODO
	// Implement
}

void WadseekerWadsTable::showEvent(QShowEvent* pEvent)
{
	if (!d.bAlreadyShownOnce)
	{
		// Events in this block must occur after the widget has been
		// constructed, but only once.
		QHeaderView* pHeader = horizontalHeader();

		pHeader->setResizeMode(IDX_URL_COLUMN, QHeaderView::Stretch);

		pHeader->resizeSection(IDX_NAME_COLUMN, 140);
		pHeader->resizeSection(IDX_PROGRESS_COLUMN, 85);
	}

}

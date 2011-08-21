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

#include "speedcalculator.h"
#include "strings.h"

#include <QHeaderView>
#include <QProgressBar>
#include <QUrl>

WadseekerWadsTable::WadseekerWadsTable(QWidget* pParent)
: QTableWidget(pParent)
{
	d.bAlreadyShownOnce = false;
}

WadseekerWadsTable::~WadseekerWadsTable()
{
	QMap<QString, SpeedCalculator* >::iterator it;
	for (it = d.speedCalculators.begin(); it != d.speedCalculators.end(); ++it)
	{
		delete it.value();
	}
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

		QProgressBar* pBar = new QProgressBar();

		setItem(rowIndex, IDX_NAME_COLUMN, new QTableWidgetItem(filename));
		setItem(rowIndex, IDX_URL_COLUMN, new QTableWidgetItem());
		setCellWidget(rowIndex, IDX_PROGRESS_COLUMN, pBar);
		setItem(rowIndex, IDX_SPEED_COLUMN, new QTableWidgetItem());
		setItem(rowIndex, IDX_ETA_COLUMN, new QTableWidgetItem(tr("N/A")));
		setItem(rowIndex, IDX_SIZE_COLUMN, new QTableWidgetItem(tr("N/A")));

		SpeedCalculator* pCalculator = new SpeedCalculator();
		pCalculator->setMaxResolution(10);
		pCalculator->start();

		d.speedCalculators.insert(filename, pCalculator);

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

void WadseekerWadsTable::setFileDownloadFinished(const QString& filename)
{
	int row = findFileRow(filename);

	if (row >= 0)
	{
		QProgressBar* pBar = (QProgressBar*) this->cellWidget(row, IDX_PROGRESS_COLUMN);
		pBar->setMaximum(1);
		pBar->setValue(1);

		// Update ETA
		// ETA will be changed to DONE if wad is installed successfully.
		item(row, IDX_ETA_COLUMN)->setText(tr("N/A"));
		item(row, IDX_SPEED_COLUMN)->setText(tr("N/A"));

		item(row, IDX_URL_COLUMN)->setText(tr("Awaiting URLs"));
		item(row, IDX_URL_COLUMN)->setToolTip(tr("Awaiting URLs"));
	}
}

void WadseekerWadsTable::setFileProgress(const QString& filename, qint64 current, qint64 total)
{
	int row = findFileRow(filename);

	if (row >= 0)
	{
		QProgressBar* pBar = (QProgressBar*) this->cellWidget(row, IDX_PROGRESS_COLUMN);
		pBar->setMaximum(total);
		pBar->setValue(current);

		// Update size
		QString strCurrent = Strings::formatDataAmount(current);
		QString strTotal = Strings::formatDataAmount(total);

		QString size = QString("%1 / %2").arg(strCurrent, strTotal);
		item(row, IDX_SIZE_COLUMN)->setText(size);

		// Update ETA and speed
		SpeedCalculator* pCalculator = d.speedCalculators.value(filename);
		pCalculator->setExpectedDataSize(total);
		pCalculator->registerDataAmount(current);

		QString eta = QString("%1s").arg(pCalculator->estimatedTimeUntilArrival());
		item(row, IDX_ETA_COLUMN)->setText(eta);

		QString speed = Strings::formatDataSpeed(pCalculator->getSpeed());
		item(row, IDX_SPEED_COLUMN)->setText(speed);
	}
}

void WadseekerWadsTable::setFileSuccessful(const QString& filename)
{
	int row = findFileRow(filename);

	if (row >= 0)
	{
		// Set progress bar to 100%.
		QProgressBar* pBar = (QProgressBar*) this->cellWidget(row, IDX_PROGRESS_COLUMN);
		pBar->setMaximum(100);
		pBar->setValue(100);

		item(row, IDX_URL_COLUMN)->setText("");

		item(row, IDX_ETA_COLUMN)->setText(tr("DONE"));
		item(row, IDX_SPEED_COLUMN)->setText("");
	}
}

void WadseekerWadsTable::setFileUrl(const QString& filename, const QUrl& url)
{
	int row = findFileRow(filename);

	if (row >= 0)
	{
		QTableWidgetItem* pItem = this->item(row, IDX_URL_COLUMN);
		pItem->setText(url.toString());
		pItem->setToolTip(url.toString());
	}
}

void WadseekerWadsTable::showEvent(QShowEvent* pEvent)
{
	if (!d.bAlreadyShownOnce)
	{
		// Events in this block must occur after the widget has been
		// constructed, but only once.
		QHeaderView* pHeader = horizontalHeader();

		// Setup resizing
		pHeader->setResizeMode(IDX_ETA_COLUMN, QHeaderView::ResizeToContents);
		pHeader->setResizeMode(IDX_SIZE_COLUMN, QHeaderView::ResizeToContents);
		pHeader->setResizeMode(IDX_SPEED_COLUMN, QHeaderView::ResizeToContents);
		pHeader->setResizeMode(IDX_PROGRESS_COLUMN, QHeaderView::ResizeToContents);
		pHeader->setResizeMode(IDX_URL_COLUMN, QHeaderView::Stretch);

		pHeader->resizeSection(IDX_NAME_COLUMN, 140);
		//pHeader->resizeSection(IDX_PROGRESS_COLUMN, 85);

	}
}

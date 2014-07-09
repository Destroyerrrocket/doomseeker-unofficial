//------------------------------------------------------------------------------
// tablewidgetmouseaware.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "tablewidgetmouseaware.h"

#include <QMouseEvent>
#include <QModelIndex>

TableWidgetMouseAware::TableWidgetMouseAware(QWidget* parent)
: QTableWidget(parent)
{
	bEmitSignalsEvenIfIndexIsInvalid = false;
}

void TableWidgetMouseAware::mouseReleaseEvent(QMouseEvent* event)
{
	QModelIndex index = indexAt(event->pos());
	switch (event->button())
	{
		case Qt::MidButton:
			if (bEmitSignalsEvenIfIndexIsInvalid || index.isValid())
			{
				emit middleMouseClick(index, event->pos());
			}
			break;

		case Qt::RightButton:
			if (bEmitSignalsEvenIfIndexIsInvalid || index.isValid())
			{
				emit rightMouseClick(index, event->pos());
			}
			break;

		default:
			QTableView::mouseReleaseEvent(event);
			break;
	}
}

void TableWidgetMouseAware::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
	{
		QTableView::mouseDoubleClickEvent(event);
	}
	else
	{
		QModelIndex index = indexAt(event->pos());
		if (bEmitSignalsEvenIfIndexIsInvalid || index.isValid())
		{
			emit leftMouseDoubleClicked(index, event->pos());
		}
	}
}

QList<int> TableWidgetMouseAware::selectedRows() const
{
	QItemSelectionModel* pSelectionModel = this->selectionModel();

	QList<int> rowIndices;
	QModelIndexList rows = pSelectionModel->selectedRows();
	foreach(const QModelIndex& index, rows)
	{
		rowIndices << index.row();
	}

	return rowIndices;
}

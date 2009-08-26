//------------------------------------------------------------------------------
// serverlistview.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include "gui/widgets/serverlistview.h"
#include <QDebug>
#include <QItemDelegate>
#include <QPainter>

/**
 * Custom delegate class.  This allows us to right align the decoration images.
 * To do this the UserRole needs to be set to USERROLE_RIGHTALIGNDECORATION.
 */
class CustomItemDelegate : public QItemDelegate
{
	public:
		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
		{
			QStyleOptionViewItem opt = option;

			QVariant userRole = index.data(Qt::UserRole);
			if(userRole.isValid() && userRole.type() == QVariant::Int && userRole.toInt() == USERROLE_RIGHTALIGNDECORATION)
			{
				opt.decorationAlignment = Qt::AlignRight|Qt::AlignVCenter;
			}

			QItemDelegate::paint(painter, opt, index);
		}

	protected:
		void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const
		{
			if(pixmap.isNull() || !rect.isValid())
				return;

			if(option.decorationAlignment == (Qt::AlignRight|Qt::AlignVCenter)) // Special handling.
			{
				QPoint p = QStyle::alignedRect(option.direction, option.decorationAlignment, pixmap.size(), option.rect).topLeft();
				painter->drawPixmap(p, pixmap);
			}
			else
				QItemDelegate::drawDecoration(painter, option, rect, pixmap);
		}
};

////////////////////////////////////////////////////////////////////////////////

ServerListView::ServerListView(QWidget* parent) : QTableView(parent)
{
	bAllowAllRowsRefresh = true;
	setItemDelegate(new CustomItemDelegate());
}

void ServerListView::updateRowVisuals(int row)
{
	resizeRowToContents(row);
}

void ServerListView::updateAllRows()
{
	if (bAllowAllRowsRefresh)
	{
		for (int i = 0; i < model()->rowCount(); ++i)
		{
			updateRowVisuals(i);
		}
	}
}

void ServerListView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() != Qt::RightButton)
	{
		QTableView::mouseReleaseEvent(event);
	}
	else
	{
		QModelIndex index = indexAt(event->pos());
		if (index.isValid())
		{
			emit rightMouseClick(index, event->pos());
		}
	}
}

void ServerListView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
	{
		QTableView::mouseDoubleClickEvent(event);
	}
	else
	{
		QModelIndex index = indexAt(event->pos());
		if (index.isValid())
		{
			emit leftMouseDoubleClicked(index, event->pos());
		}
	}
}

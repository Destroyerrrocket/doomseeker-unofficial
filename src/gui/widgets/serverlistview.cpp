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

void ServerListView::updateRowVisuals(int row)
{
	resizeRowToContents(row);
}

void ServerListView::updateAllRows()
{
	for (int i = 0; i < model()->rowCount(); ++i)
	{
		updateRowVisuals(i);
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
		emit rightMouseClick(index);
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
		emit leftMouseDoubleClicked(index);
	}
}

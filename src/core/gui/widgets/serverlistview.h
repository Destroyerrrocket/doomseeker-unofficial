//------------------------------------------------------------------------------
// serverlistview.h
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

#ifndef __SERVERLISTVIEW_H_
#define __SERVERLISTVIEW_H_

#include <QTableView>
#include <QMouseEvent>

#define USERROLE_RIGHTALIGNDECORATION 1

class ServerListView : public QTableView
{
	Q_OBJECT

	public:
		ServerListView(QWidget* parent = 0);

		/**
		 *	Enables or disables update of row visuals.
		 */
		void setAllowAllRowsRefresh(bool b) { bAllowAllRowsRefresh = b; }

	public slots:
		void updateRowVisuals(int row);
		void updateAllRows();

	protected:
		bool			bAllowAllRowsRefresh;

		virtual void 	mouseReleaseEvent(QMouseEvent* event);
		virtual void 	mouseDoubleClickEvent(QMouseEvent* event);

	signals:
		void			leftMouseDoubleClicked(const QModelIndex& index, const QPoint& cursorPosition);
		void			middleMouseClick(const QModelIndex& index, const QPoint& cursorPosition);
		void			rightMouseClick(const QModelIndex& index, const QPoint& cursorPosition);
};

#endif

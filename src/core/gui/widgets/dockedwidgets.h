//------------------------------------------------------------------------------
// dockedwidgets.h
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
#ifndef __DOCKEDWIDGETS_H_
#define __DOCKEDWIDGETS_H_

#include <QWidget>

/**
 * Every child of QDockWidget should be an instance
 * of this class.
 */
class DockWidgetChild : public QWidget
{
	Q_OBJECT

	public:
		DockWidgetChild(QWidget* parent = NULL) : QWidget(parent) {}
		virtual ~DockWidgetChild() {}

		/**
		 * Set both params to negative to invalidate the hint size.
		 */
		virtual void setHintSize(int w, int h)
		{
			if (w < 0 && h < 0)
				hintSize = QSize();
			else
				hintSize = QSize(w, h);
		}

		/**
		 * Set both params to negative to invalidate the minimum hint size.
		 */
		virtual void setMinimumHintSize(int w, int h)
		{
			if (w < 0 && h < 0)
				minimumHintSize = QSize();
			else
				minimumHintSize = QSize(w, h);
		}

		virtual QSize sizeHint() const
		{
			return hintSize;
		}

		virtual QSize sizeMinimumHint() const
		{
			return minimumHintSize;
		}

	protected:
		QSize hintSize;
		QSize minimumHintSize;
};

#endif

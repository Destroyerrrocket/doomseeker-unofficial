//------------------------------------------------------------------------------
// taskbarbutton.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "taskbarbutton.h"

#include "gui/helpers/taskbarprogress.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) && defined(Q_OS_WIN32)
#define WIN_TASKBAR
#endif

#ifdef WIN_TASKBAR
#include <QWinTaskbarButton>
#endif

#include <QIcon>
#include <QString>

DClass<TaskbarButton>
{
public:
#ifdef WIN_TASKBAR
	QWinTaskbarButton *button;
#endif
	TaskbarProgress *progress;
};
DPointered(TaskbarButton)

TaskbarButton::TaskbarButton(QObject *parent)
: QObject(parent)
{
#ifdef WIN_TASKBAR
	d->button = new QWinTaskbarButton(this);
	d->progress = new TaskbarProgress(d->button->progress(), this);
#else
	d->progress = new TaskbarProgress(this);
#endif
}

QString TaskbarButton::overlayAccessibleDescription() const
{
#ifdef WIN_TASKBAR
	return d->button->overlayAccessibleDescription();
#else
	return QString();
#endif
}

QIcon TaskbarButton::overlayIcon() const
{
#ifdef WIN_TASKBAR
	return d->button->overlayIcon();
#else
	return QIcon();
#endif
}

TaskbarProgress *TaskbarButton::progress() const
{
	return d->progress;
}

void TaskbarButton::setWindow(QWindow *window)
{
#ifdef WIN_TASKBAR
	d->button->setWindow(window);
#endif
}

QWindow *TaskbarButton::window() const
{
#ifdef WIN_TASKBAR
	return d->button->window();
#else
	return NULL;
#endif
}

void TaskbarButton::clearOverlayIcon()
{
#ifdef WIN_TASKBAR
	d->button->clearOverlayIcon();
#endif
}

void TaskbarButton::setOverlayAccessibleDescription(const QString &description)
{
#ifdef WIN_TASKBAR
	d->button->setOverlayAccessibleDescription(description);
#endif
}

void TaskbarButton::setOverlayIcon(const QIcon &icon)
{
#ifdef WIN_TASKBAR
	d->button->setOverlayIcon(icon);
#endif
}

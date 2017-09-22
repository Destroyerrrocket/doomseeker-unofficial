//------------------------------------------------------------------------------
// taskbarbutton.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
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
#include <QSysInfo>

DClass<TaskbarButton>
{
public:
#ifdef WIN_TASKBAR
	QWinTaskbarButton *button;
#endif
	TaskbarProgress *progress;

	bool isAllowedOsVersion() const
	{
#ifdef WIN_TASKBAR
		return QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7;
#else
		return false;
#endif
	}
};
DPointered(TaskbarButton)

TaskbarButton::TaskbarButton(QObject *parent)
: QObject(parent)
{
#ifdef WIN_TASKBAR
	if (d->isAllowedOsVersion())
	{
		d->button = new QWinTaskbarButton(this);
		d->progress = new TaskbarProgress(d->button->progress(), this);
	}
	else
	{
		d->button = NULL;
		d->progress = new TaskbarProgress(this);
	}
#else
	d->progress = new TaskbarProgress(this);
#endif
}

QString TaskbarButton::overlayAccessibleDescription() const
{
#ifdef WIN_TASKBAR
	if (d->button != NULL)
	{
		return d->button->overlayAccessibleDescription();
	}
#endif
	return QString();
}

QIcon TaskbarButton::overlayIcon() const
{
#ifdef WIN_TASKBAR
	if (d->button != NULL)
	{
		return d->button->overlayIcon();
	}
#endif
	return QIcon();
}

TaskbarProgress *TaskbarButton::progress() const
{
	return d->progress;
}

void TaskbarButton::setWindow(QWindow *window)
{
#ifdef WIN_TASKBAR
	if (d->button != NULL)
	{
		d->button->setWindow(window);
	}
#endif
}

QWindow *TaskbarButton::window() const
{
#ifdef WIN_TASKBAR
	if (d->button != NULL)
	{
		return d->button->window();
	}
#endif
	return NULL;
}

void TaskbarButton::clearOverlayIcon()
{
#ifdef WIN_TASKBAR
	if (d->button != NULL)
	{
		d->button->clearOverlayIcon();
	}
#endif
}

void TaskbarButton::setOverlayAccessibleDescription(const QString &description)
{
#ifdef WIN_TASKBAR
	if (d->button != NULL)
	{
		d->button->setOverlayAccessibleDescription(description);
	}
#endif
}

void TaskbarButton::setOverlayIcon(const QIcon &icon)
{
#ifdef WIN_TASKBAR
	if (d->button != NULL)
	{
		d->button->setOverlayIcon(icon);
	}
#endif
}

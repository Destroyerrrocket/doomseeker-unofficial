//------------------------------------------------------------------------------
// taskbarprogress.cpp
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
#include "taskbarprogress.h"

#include <cassert>
#include <QSysInfo>

// [Zalewa] I suppose the #ifdef checks could be a bit more abstract
// just in case if we ever have another platform that supports similar
// functionality, but let's worry about that when it comes to that.

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) && defined(Q_OS_WIN32)
#define WIN_TASKBAR
#endif

#ifdef WIN_TASKBAR
#include <QWinTaskbarProgress>
#endif

DClass<TaskbarProgress>
{
public:
#ifdef WIN_TASKBAR
	QWinTaskbarProgress *progress;
#endif

	bool isAllowedOsVersion() const
	{
#ifdef WIN_TASKBAR
		return QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7;
#else
		return false;
#endif
	}
};
DPointered(TaskbarProgress)

TaskbarProgress::TaskbarProgress(QWinTaskbarProgress *progress, QObject *parent)
: QObject(parent)
{
#ifdef WIN_TASKBAR
	d->progress = NULL;
	assert(d->isAllowedOsVersion());
	if (d->isAllowedOsVersion())
	{
		d->progress = progress;
	}
#endif
	construct();
}

TaskbarProgress::TaskbarProgress(QObject *parent)
: QObject(parent)
{
#ifdef WIN_TASKBAR
	d->progress = NULL;
	if (d->isAllowedOsVersion())
	{
		d->progress = new QWinTaskbarProgress(this);
	}
#endif
	construct();
}

void TaskbarProgress::construct()
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		this->connect(d->progress, SIGNAL(maximumChanged(int)), SIGNAL(maximumChanged(int)));
		this->connect(d->progress, SIGNAL(minimumChanged(int)), SIGNAL(minimumChanged(int)));
		this->connect(d->progress, SIGNAL(valueChanged(int)), SIGNAL(valueChanged(int)));
		this->connect(d->progress, SIGNAL(visibilityChanged(bool)), SIGNAL(visibilityChanged(bool)));
	}
#endif
}

bool TaskbarProgress::isPaused() const
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		return d->progress->isPaused();
	}
#endif
	return false;
}

bool TaskbarProgress::isStopped() const
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		return d->progress->isStopped();
	}
#endif
	return false;
}

bool TaskbarProgress::isVisible() const
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		return d->progress->isVisible();
	}
#endif
	return false;
}

int TaskbarProgress::maximum() const
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		return d->progress->maximum();
	}
#endif
	return 0;
}

int TaskbarProgress::minimum() const
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		return d->progress->minimum();
	}
#endif
	return 0;
}

int TaskbarProgress::value() const
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		return d->progress->value();
	}
#endif
	return 0;
}

void TaskbarProgress::hide()
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->hide();
	}
#endif
}

void TaskbarProgress::pause()
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->pause();
	}
#endif
}

void TaskbarProgress::reset()
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->reset();
	}
#endif
}

void TaskbarProgress::resume()
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->resume();
	}
#endif
}

void TaskbarProgress::setMaximum(int maximum)
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->setMaximum(maximum);
	}
#endif
}

void TaskbarProgress::setMinimum(int minimum)
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->setMinimum(minimum);
	}
#endif
}

void TaskbarProgress::setPaused(bool paused)
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->setPaused(paused);
	}
#endif
}

void TaskbarProgress::setRange(int minimum, int maximum)
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->setRange(minimum, maximum);
	}
#endif
}

void TaskbarProgress::setValue(int value)
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->setValue(value);
	}
#endif
}

void TaskbarProgress::setVisible(bool visible)
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->setVisible(visible);
	}
#endif
}

void TaskbarProgress::show()
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->show();
	}
#endif
}

void TaskbarProgress::stop()
{
#ifdef WIN_TASKBAR
	if (d->progress != NULL)
	{
		d->progress->stop();
	}
#endif
}

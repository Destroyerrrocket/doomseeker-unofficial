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
};
DPointered(TaskbarProgress)

TaskbarProgress::TaskbarProgress(QWinTaskbarProgress *progress, QObject *parent)
: QObject(parent)
{
#ifdef WIN_TASKBAR
	d->progress = progress;
#endif
	construct();
}

TaskbarProgress::TaskbarProgress(QObject *parent)
: QObject(parent)
{
#ifdef WIN_TASKBAR
	d->progress = new QWinTaskbarProgress(this);
#endif
	construct();
}

void TaskbarProgress::construct()
{
#ifdef WIN_TASKBAR
	this->connect(d->progress, SIGNAL(maximumChanged(int)), SIGNAL(maximumChanged(int)));
	this->connect(d->progress, SIGNAL(minimumChanged(int)), SIGNAL(minimumChanged(int)));
	this->connect(d->progress, SIGNAL(valueChanged(int)), SIGNAL(valueChanged(int)));
	this->connect(d->progress, SIGNAL(visibilityChanged(bool)), SIGNAL(visibilityChanged(bool)));
#endif
}

bool TaskbarProgress::isPaused() const
{
#ifdef WIN_TASKBAR
	return d->progress->isPaused();
#else
	return false;
#endif
}

bool TaskbarProgress::isStopped() const
{
#ifdef WIN_TASKBAR
	return d->progress->isStopped();
#else
	return false;
#endif
}

bool TaskbarProgress::isVisible() const
{
#ifdef WIN_TASKBAR
	return d->progress->isVisible();
#else
	return false;
#endif
}

int TaskbarProgress::maximum() const
{
#ifdef WIN_TASKBAR
	return d->progress->maximum();
#else
	return 0;
#endif
}

int TaskbarProgress::minimum() const
{
#ifdef WIN_TASKBAR
	return d->progress->minimum();
#else
	return 0;
#endif
}

int TaskbarProgress::value() const
{
#ifdef WIN_TASKBAR
	return d->progress->value();
#else
	return 0;
#endif
}

void TaskbarProgress::hide()
{
#ifdef WIN_TASKBAR
	d->progress->hide();
#endif
}

void TaskbarProgress::pause()
{
#ifdef WIN_TASKBAR
	d->progress->pause();
#endif
}

void TaskbarProgress::reset()
{
#ifdef WIN_TASKBAR
	d->progress->reset();
#endif
}

void TaskbarProgress::resume()
{
#ifdef WIN_TASKBAR
	d->progress->resume();
#endif
}

void TaskbarProgress::setMaximum(int maximum)
{
#ifdef WIN_TASKBAR
	d->progress->setMaximum(maximum);
#endif
}

void TaskbarProgress::setMinimum(int minimum)
{
#ifdef WIN_TASKBAR
	d->progress->setMinimum(minimum);
#endif
}

void TaskbarProgress::setPaused(bool paused)
{
#ifdef WIN_TASKBAR
	d->progress->setPaused(paused);
#endif
}

void TaskbarProgress::setRange(int minimum, int maximum)
{
#ifdef WIN_TASKBAR
	d->progress->setRange(minimum, maximum);
#endif
}

void TaskbarProgress::setValue(int value)
{
#ifdef WIN_TASKBAR
	d->progress->setValue(value);
#endif
}

void TaskbarProgress::setVisible(bool visible)
{
#ifdef WIN_TASKBAR
	d->progress->setVisible(visible);
#endif
}

void TaskbarProgress::show()
{
#ifdef WIN_TASKBAR
	d->progress->show();
#endif
}

void TaskbarProgress::stop()
{
#ifdef WIN_TASKBAR
	d->progress->stop();
#endif
}

//------------------------------------------------------------------------------
// log.cpp
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
#include "log.h"
#include "strings.h"
#include <QDateTime>
#include <QMutexLocker>
#include <cstdio>

DClass<Log>
{
	public:
		QString logContent;
		QMutex mutex;
		bool printToStdout;
		bool timestamps;
};

DPointeredNoCopy(Log)

Log gLog;

Log::Log()
{
	d->timestamps = true;
	d->printToStdout = true;
}

Log::~Log()
{
}

void Log::addEntry(const QString& string)
{
	QString timestampString;
	if (areTimestampsEnabled())
	{
		timestampString = Strings::timestamp("[hh:mm:ss] ");
	}

	QString entry = timestampString + string + "\n";
	addUnformattedEntry(entry);
}

void Log::addUnformattedEntry(const QString& string)
{
	QMutexLocker locker(&d->mutex);

	if (isPrintingToStdout())
	{
		printf("%s", string.toAscii().constData());
	}

	d->logContent += string;
	emit newEntry(string);
}

bool Log::areTimestampsEnabled() const
{
	return d->timestamps;
}

void Log::clearContent()
{
	d->logContent.clear();
}

const QString& Log::content() const
{
	return d->logContent;
}

bool Log::isPrintingToStdout() const
{
	return d->printToStdout;
}

Log& Log::operator<<(const QString& string)
{
	addEntry(string);
	return *this;
}

void Log::setPrintingToStdout(bool b)
{
	d->printToStdout = b;
}

void Log::setTimestampsEnabled(bool b)
{
	d->timestamps = b;
}

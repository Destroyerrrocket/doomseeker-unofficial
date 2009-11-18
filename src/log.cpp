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
#include <QDateTime>

Log Log::logger;

Log::Log()
{
	timestamps = true;
	printToStdout = true;
}

void Log::addEntry(const QString& string)
{
	QString timestampString;
	if (timestamps)
	{
		timestampString = QString("[%1] ").arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
	}

	QString entry = timestampString + string + "\n";

	if (printToStdout)
	{
		printf(entry.toAscii().constData());
	}

	logContent += entry;
	emit newEntry(entry);
}

Log& Log::operator<<(const QString& string)
{
	addEntry(string);
	return *this;
}

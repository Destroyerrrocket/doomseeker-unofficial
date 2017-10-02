//------------------------------------------------------------------------------
// buddyinfo.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __BUDDYINFO_H__
#define __BUDDYINFO_H__

#include <QRegExp>
#include <QString>
#include <QVector>

class BuddyInfo
{
	public:
		enum PatternType
		{
			PT_BASIC,
			PT_ADVANCED
		};

		static QString createConfigEntry(const QVector<BuddyInfo>& buddies);
		static void readConfigEntry(const QString& configEntry, QVector<BuddyInfo>& outVector);

		QRegExp pattern;
		PatternType patternType;

};

#endif

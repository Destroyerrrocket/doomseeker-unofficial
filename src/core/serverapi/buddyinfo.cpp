//------------------------------------------------------------------------------
// buddyinfo.cpp
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
#include "buddyinfo.h"
#include "scanner.h"
#include "strings.hpp"

QString BuddyInfo::createConfigEntry(const QVector<BuddyInfo>& buddies)
{
	QString settingValue;

	foreach(const BuddyInfo& buddyInfo, buddies)
	{
		QString pattern = buddyInfo.pattern.pattern();
		settingValue.append((buddyInfo.patternType == PT_BASIC ? "basic \"" : "advanced \"") + Strings::escape(pattern) + "\";");
	}

	return settingValue;
}
#include <QDebug>
void BuddyInfo::readConfigEntry(const QString& configEntry, QVector<BuddyInfo>& outVector)
{
	outVector.clear();

	Scanner listReader(configEntry.toUtf8().constData(), configEntry.length());
	// Syntax: {basic|advanced} "pattern";...
	while(listReader.tokensLeft())
	{
		if(!listReader.checkToken(TK_Identifier))
		{
			break; // Invalid so lets just use what we have.
		}

		BuddyInfo::PatternType type = BuddyInfo::PT_BASIC;
		if(listReader->str().compare("basic") == 0)
		{
			type = PT_BASIC;
		}
		else if(listReader->str().compare("advanced") == 0)
		{
			type = PT_ADVANCED;
		}

		if(!listReader.checkToken(TK_StringConst))
		{
			break;
		}

		QRegExp pattern(listReader->str(), Qt::CaseInsensitive, type == PT_BASIC ? QRegExp::Wildcard : QRegExp::RegExp);
		if(pattern.isValid())
		{
			BuddyInfo buddyInfo;

			buddyInfo.pattern = pattern;
			buddyInfo.patternType = type;

			outVector.append(buddyInfo);
		}

		if(!listReader.checkToken(';'))
		{
			break;
		}
	}
}

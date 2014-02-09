//------------------------------------------------------------------------------
// buddyinfo.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "buddyinfo.h"
#include "scanner.h"
#include "strings.h"

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

	Scanner listReader(configEntry.toAscii().constData(), configEntry.length());
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

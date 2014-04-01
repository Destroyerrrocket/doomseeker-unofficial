//------------------------------------------------------------------------------
// buddyinfo.h
//
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

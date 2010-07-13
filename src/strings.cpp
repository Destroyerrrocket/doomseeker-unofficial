//------------------------------------------------------------------------------
// strings.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "strings.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <QDateTime>
#include <QStringList>
#include <QUrl>

const char Strings::RANDOM_CHAR_POOL[RANDOM_CHAR_POOL_SIZE] =
{
	'a', 'b', 'c', 'd', 'e', 'f', 'g',
	'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u',
	'v', 'w', 'x', 'y', 'z', '0', '1',
	'2', '3', '4', '5', '6', '7', '8',
	'9'
};

QString Strings::createRandomAlphaNumericString(unsigned numChars)
{
	QDateTime currentTime = QDateTime::currentDateTime();
	srand(clock() + currentTime.toTime_t());
	
	QString generatedString = "";
	for (unsigned i = 0; i < numChars; ++i)
	{
		unsigned index = rand() % RANDOM_CHAR_POOL_SIZE;
		generatedString += RANDOM_CHAR_POOL[index];
	}
	
	return generatedString;
}

QString Strings::formatDataAmount(int bytes)
{
	DataUnit dataUnit;

	float fBytes = (float)bytes;
	fBytes = scaleDataUnit(fBytes, dataUnit);
	
	QString formattedString = QString("%1 ").arg(fBytes, 0, 'f', 2);
	switch(dataUnit)
	{
		case Byte:
			formattedString += "B";
			break;

		case Kilobyte:
			formattedString += "kB";
			break;

		case Megabyte:
			formattedString += "MB";
			break;

		case Gigabyte:
			formattedString += "GB";
			break;

		default:
			// Shouldn't really happen.
			return "#ERR: Formatting data amount error.";
	}

	return formattedString;
}

QString Strings::formatDataSpeed(float speedInBytesPerSecond)
{
	DataUnit dataUnit;

	speedInBytesPerSecond = scaleDataUnit(speedInBytesPerSecond, dataUnit);

	QString formattedString = QString("%1 ").arg(speedInBytesPerSecond, 0, 'f', 2);
	switch(dataUnit)
	{
		case Byte:
			formattedString += "B/s";
			break;

		case Kilobyte:
			formattedString += "kB/s";
			break;

		case Megabyte:
			formattedString += "MB/s";
			break;

		case Gigabyte:
			formattedString += "GB/s";
			break;

		default:
			// Shouldn't really happen.
			return "#ERR: Formatting speed error.";
	}

	return formattedString;
}

QString Strings::formatTime(float seconds)
{
	if (seconds < 0.0f)
	{
		return "#ERR: Formatting time error.";
	}

	seconds = ceil(seconds);

	// QTime is a 24-hour clock. It cannot be used here since seconds input
	// can be larger than that.

	int hours = 0;
	int minutes = 0;
	int remainingSeconds = 0;

	if (seconds >= 3600.0f)
	{
		// An hour or more.
		hours = seconds / 3600.0f;
		seconds -= hours * 3600.0f;
	}

	if (seconds >= 60.0f)
	{
		// A minute or more.
		minutes = seconds / 60.0f;
		seconds -= minutes * 60.0f;
	}

	remainingSeconds = (int)seconds;

	QString formattedString;
	if (hours > 0)
	{
		formattedString += QString("%1h ").arg(hours);
	}

	if (hours > 0 || minutes > 0)
	{
		formattedString += QString("%1min. ").arg(minutes);
	}

	formattedString += QString("%1s").arg(remainingSeconds);

	return formattedString;
}

bool Strings::isCharOnCharList(char c, const QString& charList)
{
	for (int i = 0; i < charList.length(); ++i)
	{
		if (charList[i] == c)
			return true;
	}

	return false;
}

bool Strings::isUrlSafe(const QString& url)
{
	QUrl urlObject = url;
	
	QString scheme = urlObject.scheme();
	
	bool bIsSafe1 = scheme.isEmpty();
	bool bIsSafe2 = (scheme.compare("http", Qt::CaseInsensitive) == 0);
	bool bIsSafe3 = (scheme.compare("ftp", Qt::CaseInsensitive) == 0);

	
	return bIsSafe1 || bIsSafe2 || bIsSafe3;
}

float Strings::scaleDataUnit(float bytes, DataUnit& outUnit)
{
	const static float UPPER_BOUNDARY = 900.0f;	
	outUnit = Byte;

	while (bytes > UPPER_BOUNDARY && outUnit != Gigabyte)
	{
		bytes /= 1024.0f;
		outUnit = (DataUnit)((int)outUnit + 1);
	}	
	
	return bytes;
}

void Strings::translateServerAddress(const QString& addressString, QString& hostname, unsigned short& port, const QString& defaultHostname, const unsigned short defaultPort)
{
	port = 0;
	QStringList addressAndPort = addressString.split(":");

	if (addressAndPort.size() == 0 || addressAndPort.size() > 2)
	{ // if something is not right set default settings
		hostname = defaultHostname;
	}
	else
	{
		hostname = addressAndPort[0];
		if (addressAndPort.size() == 2)
		{
			port = addressAndPort[1].toShort();
		}
	}

	if (port == 0)
	{
		port = defaultPort;
	}
}

QString& Strings::trimr(QString& str, const QString& charList)
{
	int i;
	for (i = str.length() - 1; i >= 0; --i)
	{
		if (!isCharOnCharList(str[i].toAscii(), charList))
			break;
	}
	++i;

	return str.remove(i, str.length() - i);
}

QString& Strings::triml(QString& str, const QString& charList)
{
	int i;
	for (i = 0; i < str.length(); ++i)
	{
		if (!isCharOnCharList(str[i].toAscii(), charList))
			break;
	}

	return str.remove(0, i);
}

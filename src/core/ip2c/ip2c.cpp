//------------------------------------------------------------------------------
// ip2c.cpp
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#include "ip2c.h"

#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QResource>
#include <QTime>
#include <QTimeLine>

#include "log.h"

IP2C::IP2C()
: flagLan(":flags/lan-small"), flagLocalhost(":flags/localhost-small"),
  flagUnknown(":flags/unknown-small")
{
}

IP2C::~IP2C()
{
}

void IP2C::appendEntryToDatabase(const IP2CData& entry)
{
	QMutexLocker dataAccessMutexLocker(&dataAccessMutex);

	if (database.isEmpty() || entry.ipStart > database.back().ipStart)
	{
		database << entry;
	}
	else if (entry.ipStart < database.first().ipStart)
	{
		database.insert(0, entry);
	}
	else
	{
		QList<IP2CData>::iterator it;
		for (it = database.begin(); it != database.end(); ++it)
		{
			if (entry.ipStart < it->ipStart)
			{
				database.insert(it, entry);
				break;
			}
		}
	}
}

const QPixmap &IP2C::flag(unsigned int ipaddress, const QString& countryShortName)
{
	if (flags.contains(countryShortName))
	{
		return flags[countryShortName];
	}

	QString resName = ":flags/" + countryShortName;
	QResource res(resName);

	if (!res.isValid())
	{
		gLog << tr("No flag for country: %1").arg(countryShortName);
		flags[countryShortName] = flagUnknown;
		return flagUnknown;
	}

	flags[countryShortName] = QPixmap(resName);
	return flags[countryShortName];
}

const IP2C::IP2CData& IP2C::lookupIP(unsigned int ipaddress)
{
	QMutexLocker dataAccessMutexLocker(&dataAccessMutex);

	if(database.empty())
	{
		return invalidData;
	}

	unsigned int upper = database.size()-1;
	unsigned int lower = 0;
	unsigned int index = database.size()/2;
	unsigned int lastIndex = 0xFFFFFFFF;
	while(index != lastIndex) // Infinite loop protection.
	{
		lastIndex = index;

		if(ipaddress < database[index].ipStart)
		{
			upper = index;
			index -= (index-lower)>>1; // If we're concerning ourselves with speed >>1 is the same as /2, but should be faster.
			continue;
		}
		else if(ipaddress > database[index].ipEnd)
		{
			lower = index;
			index += (upper-index)>>1;
			continue;
		}
		return database[index];
	}

	return invalidData;
}

IP2CCountryInfo IP2C::obtainCountryInfo(unsigned int ipaddress)
{
	if (isLocalhostAddress(ipaddress))
	{
		return IP2CCountryInfo(&flagLocalhost, tr("Localhost"));
	}

	if (isLANAddress(ipaddress))
	{
		return IP2CCountryInfo(&flagLan, tr("LAN"));
	}

	const IP2CData& data = lookupIP(ipaddress);

	if (data.country.isEmpty())
	{
		char buffer[1024];

		sprintf(buffer, "Unrecognized IP address: %s (DEC: %u / HEX: %X)", QHostAddress(ipaddress).toString().toAscii().constData(), ipaddress, ipaddress);
		gLog << buffer;

		return IP2CCountryInfo(&flagUnknown, tr("Unknown"));
	}

	if (!data.isValid())
	{
		return IP2CCountryInfo();
	}

	const QPixmap* pFlag = &flag(ipaddress, data.country);
	return IP2CCountryInfo(pFlag, data.countryFullName);
}

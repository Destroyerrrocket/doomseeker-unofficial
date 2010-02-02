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

#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QResource>
#include <QTime>
#include <QTimeLine>
#include <zlib.h>

#include "log.h"
#include "ip2c.h"
#include "sdeapi/pluginloader.hpp"
#include "sdeapi/scanner.hpp"

IP2C::IP2C(const QStringList &baseDirectories, QString file, QUrl netLocation)
: downloadProgressWidget(NULL), flagLan(":flags/lan-small"),
  flagLocalhost(":flags/localhost-small"), flagUnknown(":flags/unknown-small"),
  netLocation(netLocation)
{
	// By default use the first directory.
	this->file = baseDirectories[0] + '/' + file;

	foreach(QString baseDir, baseDirectories) // Search for existing database
	{
		QFileInfo fileinfo(baseDir, file);
		if(fileinfo.exists())
		{
			this->file = baseDir + '/' + file;
			break;
		}
	}

	read = readDatabase();
	www = new WWW();
	www->setUserAgent(QString("Doomseeker/") + QString(VERSION));
}

IP2C::~IP2C()
{
	if(www != NULL)
		delete www;
	if(downloadProgressWidget != NULL)
		delete downloadProgressWidget;
}

void IP2C::appendEntryToDatabase(const IP2CData& entry)
{
	if (database.isEmpty() || entry.ipStart > database[database.count() - 1].ipStart)
	{
		database << entry;
	}
	else if (entry.ipStart < database[0].ipStart)
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

bool IP2C::convertAndSaveDatabase(QByteArray& downloadedData)
{
	QTime time;
	time.start();

	if (downloadedData.isEmpty())
		return false;

	Countries countries;
	readTextDatabase(downloadedData, countries);

	QByteArray binaryData;
	convertCountriesIntoBinaryData(countries, binaryData);

	QFile out(file);
    if(out.open(QIODevice::WriteOnly) && out.isWritable())
    {
		out.write(binaryData);
		pLog << tr("Database converted in %1 ms").arg(time.elapsed());
    }
    else
    {
    	return false;
    }

    return true;
}

void IP2C::convertCountriesIntoBinaryData(const Countries& countries, QByteArray& output)
{
	output.clear();
	unsigned fileId = MAKEID('I', 'P', '2', 'C');
	unsigned short version = 2;
	output += QByteArray((const char*)&fileId, 4);
	output += QByteArray((const char*)&version, sizeof(unsigned short));

	CountriesConstIt it;
	for (it = countries.constBegin(); it != countries.constEnd(); ++it)
	{
		// Read the first IP2CData entry for country info
		if (!it.value().empty())
		{
			const IP2CData& val = it.value()[0];
			unsigned ipBlocksNum = it.value().count();

			output += val.countryFullName;
			output += QByteArray(1, 0); // array with one null character
			output += val.country;
			output += QByteArray(1, 0); // array with one null character
			output += QByteArray((const char*)&ipBlocksNum, sizeof(ipBlocksNum));
		}

		foreach(IP2CData val, it.value())
		{
			const char ipStart[4] = { WRITEINT32_DIRECT(val.ipStart) };
			const char ipEnd[4] = { WRITEINT32_DIRECT(val.ipEnd) };

			output += QByteArray(ipStart, 4);
			output += QByteArray(ipEnd, 4);
		}
	}
}

void IP2C::downloadDatabase(QStatusBar *statusbar)
{
	if(downloadProgressWidget != NULL)
		delete downloadProgressWidget;
	downloadProgressWidget = new QProgressBar();
	downloadProgressWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	if(statusbar != NULL)
		statusbar->addPermanentWidget(downloadProgressWidget);

	connect(www, SIGNAL( fileDone(QByteArray&, const QString&) ), this, SLOT( processHttp(QByteArray&, const QString&) ));
	connect(www, SIGNAL( downloadProgress(int, int) ), this, SLOT( downloadProgress(int, int) ));
	www->getUrl(netLocation);
}

void IP2C::downloadProgress(int value, int max)
{
	if(downloadProgressWidget == NULL)
		return;

	downloadProgressWidget->setMaximum(max);
	downloadProgressWidget->setValue(value);
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
		pLog << tr("No flag for country: %1").arg(countryShortName);
		flags[countryShortName] = flagUnknown;
		return flagUnknown;
	}

	flags[countryShortName] = QPixmap(resName);
	return flags[countryShortName];
}

const IP2C::IP2CData& IP2C::lookupIP(unsigned int ipaddress) const
{
	if(!read)
		return invalidData;

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

bool IP2C::needsUpdate()
{
	if (file.isEmpty())
		return false;

	QFileInfo fi(file);
	if (fi.exists())
	{
		// Currently there are no other criteria, if file exists
		// it doesn't need to be downloaded.
		return false;
	}

	return true;
}

CountryInfo IP2C::obtainCountryInfo(unsigned int ipaddress)
{
	if (isLocalhostAddress(ipaddress))
	{
		CountryInfo ci = { true, &flagLocalhost, tr("Localhost") };
		return ci;
	}

	if (isLANAddress(ipaddress))
	{
		CountryInfo ci = { true, &flagLan, tr("LAN") };
		return ci;
	}

	const IP2CData& data = lookupIP(ipaddress);

	if (data.country.isEmpty())
	{
		char buffer[1024];
		sprintf(buffer, "Unrecognized IP address: %s (DEC: %u / HEX: %X)", QHostAddress(ipaddress).toString().toAscii().constData(), ipaddress, ipaddress);
		pLog << buffer;
		CountryInfo ci = { true, &flagUnknown, tr("Unknown") };
		return ci;
	}

	if (!data.isValid())
	{
		CountryInfo ci = {false, NULL, QString() };
		return ci;
	}

	CountryInfo ci = {true, &flag(ipaddress, data.country), data.countryFullName };
	return ci;
}

void IP2C::processHttp(QByteArray& data, const QString& filename)
{
	// First we need to write it to a temporary file
	QFile tmp(file + ".gz");
	if(tmp.open(QIODevice::WriteOnly) && tmp.isWritable())
	{
		tmp.write(data);

		QByteArray uncompressedData;
		gzFile gz = gzopen((file + ".gz").toAscii().constData(), "rb");
		if(gz != NULL)
		{
			char chunk[131072]; // 128k
			int bytesRead = 0;
			while((bytesRead = gzread(gz, chunk, 131072)) != 0)
				uncompressedData.append(QByteArray(chunk, bytesRead));
			gzclose(gz);

			// write it to a new file.
			// but ignore if data failed to uncompress
			if (uncompressedData.size() > 0)
			{
				convertAndSaveDatabase(uncompressedData);
			}
		}

		tmp.remove();
	}

	read = readDatabase();
	// clear the progress bar
	if(downloadProgressWidget != NULL)
	{
		delete downloadProgressWidget;
		downloadProgressWidget = NULL;
	}
}

bool IP2C::readDatabase()
{
	QFile db(file);
	if(!db.exists() || !db.open(QIODevice::ReadOnly) || !db.isReadable())
		return false;

	// We need to check whether this is a text file or Doomseeker's IP2C
	// compacted database. To determine this we see if first four bytes are
	// equal to IP2C. If not, we perform the conversion.

	QString signature = db.read(4);
	db.seek(0);
	if (signature.compare("IP2C") != 0)
	{
		pLog << tr("IP2C database is not in compacted format. Performing conversion!");
		QByteArray contents = db.readAll();

		if (!convertAndSaveDatabase(contents))
		{
			pLog << tr("Conversion failed");
			return false;
		}
	}
	db.close();

	db.setFileName(file);
	if (!db.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QTime time;
	time.start();

	QByteArray dataArray = db.readAll();

	// Read version.
	int pos = 4;
	if (pos >= dataArray.size())
	{
		return false;
	}

	const char* data = dataArray.constData();
	unsigned short version = READINT16(&data[pos]);

	bool wasReadSuccessful = false;
	switch (version)
	{
		case 1:
			wasReadSuccessful = readDatabaseVersion1(dataArray);
			break;

		case 2:
			wasReadSuccessful = readDatabaseVersion2(dataArray);
			break;

		default:
			wasReadSuccessful = false;
			break;
	}

	if (!wasReadSuccessful)
	{
		return false;
	}

//	foreach(IP2CData data, database)
//	{
//		printf("%s %s: %u %X / %u %X\n", data.countryFullName.toAscii().constData(), data.country.toAscii().constData(), data.ipStart, data.ipStart, data.ipEnd, data.ipEnd);
//	}

	pLog << tr("IP2C Database read in %1 ms. Entries read: %2").arg(time.elapsed()).arg(database.size());

	emit databaseUpdated();
	return true;
}

bool IP2C::readDatabaseVersion1(const QByteArray& dataArray)
{
	int pos = 6; // skip file tag and version number
	const char* data = dataArray.constData();

	while (pos < dataArray.size())
	{
		IP2CData entry;

		// Perform error checks at each point. We don't want the app to crash
		// due to corrupted database.
		if (pos + 4 > dataArray.size()) return false;
		entry.ipStart = READINT32(&data[pos]);
		pos += 4;

		if (pos + 4 > dataArray.size()) return false;
		entry.ipEnd = READINT32(&data[pos]);
		pos += 4;

		entry.country = &data[pos];
		pos += entry.country.size() + 1;

		// Entries in the file are already sorted.
		database << entry;
	}

	return true;
}

bool IP2C::readDatabaseVersion2(const QByteArray& dataArray)
{
	int pos = 6; // skip file tag and version number
	const char* data = dataArray.constData();

	// We need to store the addresses in such hash table to make sure they
	// are ordered in proper, ascending order. Otherwise the whole library
	// will not work!
	QMap<unsigned, IP2CData> hashTable;

	while (pos < dataArray.size())
	{
		// Base entry for each IP read from the file
		IP2CData baseEntry;

		baseEntry.countryFullName = &data[pos];
		pos += baseEntry.countryFullName.size() + 1;

		baseEntry.country = &data[pos];
		pos += baseEntry.country.size() + 1;

		if (pos + 4 > dataArray.size())	return false;
		unsigned numOfIpBlocks = READINT32(&data[pos]);
		pos += 4;

		for (unsigned x = 0; x < numOfIpBlocks; ++x)
		{
			// Create new entry from the base.
			IP2CData entry = baseEntry;

			// Perform error checks at each point. We don't want the app to crash
			// due to corrupted database.
			if (pos + 4 > dataArray.size()) return false;
			entry.ipStart = READINT32(&data[pos]);
			pos += 4;

			if (pos + 4 > dataArray.size()) return false;
			entry.ipEnd = READINT32(&data[pos]);
			pos += 4;

			hashTable[entry.ipStart] = entry;
		}
	}

	database = hashTable.values();

	return true;
}

void IP2C::readTextDatabase(QByteArray& textDatabase, Countries& countries)
{
	// Skip over the header
	int indexOfNewLine = -1;
	while(textDatabase[indexOfNewLine + 1] == '#')
	{
		indexOfNewLine = textDatabase.indexOf('\n', indexOfNewLine + 1);
	}

	// Trim the header
	textDatabase = textDatabase.right(textDatabase.size() - indexOfNewLine);

	Scanner sc = Scanner(textDatabase.constData(), textDatabase.count());
	countries.clear();
	while(sc.tokensLeft())
	{
		IP2CData entry;
		bool ok = true;

		if(!sc.checkToken(TK_StringConst)) break; // ipStart
		entry.ipStart = sc.str.toUInt(&ok);
		if(!ok || !sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // ipEnd
		entry.ipEnd = sc.str.toUInt(&ok);
		if(!ok || !sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // Register
		if(!sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // date assigned
		if(!sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // 2 char country
		if(!sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // 3 char country
		entry.country = sc.str;
		if(!sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // country string
		entry.countryFullName = sc.str;


		if (countries.contains(entry.country))
		{
			countries[entry.country].append(entry);
		}
		else
		{
			QList<IP2CData> list;
			list.append(entry);
			countries[entry.country] = list;
		}
	}
}

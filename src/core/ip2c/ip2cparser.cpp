//------------------------------------------------------------------------------
// ip2cparser.cpp
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
#include "ip2cparser.h"

#include "global.h"
#include "log.h"
#include "scanner.h"
#include <QFile>
#include <QMap>
#include <QMutexLocker>
#include <QTime>

IP2CParser::IP2CParser(IP2C* pTargetDatabase) 
{
	currentParsingThread = NULL;
	this->pTargetDatabase = pTargetDatabase;
	bIsParsing = false;
}

bool IP2CParser::convertAndSaveDatabase(QByteArray& downloadedData, const QString& outFilePath)
{
	if (downloadedData.isEmpty())
		return false;

	Countries countries;
	readTextDatabase(downloadedData, countries);

	QByteArray binaryData;
	convertCountriesIntoBinaryData(countries, binaryData);

	QFile out(outFilePath);
	if(out.open(QIODevice::WriteOnly) && out.isWritable())
	{
		out.write(binaryData);
	}
	else
	{
		return false;
	}

	return true;
}

void IP2CParser::convertCountriesIntoBinaryData(const Countries& countries, QByteArray& output)
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
			const IP2C::IP2CData& val = it.value()[0];
			unsigned ipBlocksNum = it.value().count();

			output += val.countryFullName;
			output += QByteArray(1, 0); // array with one null character
			output += val.country;
			output += QByteArray(1, 0); // array with one null character
			output += QByteArray((const char*)&ipBlocksNum, sizeof(ipBlocksNum));
		}

		foreach(IP2C::IP2CData val, it.value())
		{
			const char ipStart[4] = { WRITEINT32_DIRECT(char,val.ipStart) };
			const char ipEnd[4] = { WRITEINT32_DIRECT(char,val.ipEnd) };

			output += QByteArray(ipStart, 4);
			output += QByteArray(ipEnd, 4);
		}
	}
}

bool IP2CParser::doReadDatabase(const QString& filePath)
{
	QMutexLocker mutexLocker(&thisLock);

	// This will set proper state whenever and wherever this method finishes.
	ConstructorDestructorParserStateSetter stateSetter(this);
	
	QFile dataBase(filePath);
	gLog << tr("Parsing IP2C database: %1").arg(dataBase.fileName());
	
	if (!dataBase.exists() 
	||  !dataBase.open(QIODevice::ReadOnly) 
	||  !dataBase.isReadable())
	{
		gLog << tr("Unable to open IP2C file.");
		return false;
	}

	// We need to check whether this is a text file or Doomseeker's IP2C
	// compacted database. To determine this we see if first four bytes are
	// equal to IP2C. If not, we perform the conversion.

	QString signature = dataBase.read(4);
	dataBase.seek(0);
	if (signature.compare("IP2C") != 0)
	{
		gLog << tr("IP2C database is not in compacted format. Performing conversion!");
		QByteArray contents = dataBase.readAll();

		QTime time;
		time.start();
		if (!convertAndSaveDatabase(contents, filePath))
		{
			gLog << tr("IP2C database conversion failed");
			return false;
		}
		else
		{
			gLog << tr("IP2C database converted in %1 ms").arg(time.elapsed());
			gLog << tr("Parsing now compacted IP2C database");
		}
	}
	dataBase.close();

	dataBase.setFileName(filePath);
	if (!dataBase.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QTime time;
	time.start();

	QByteArray dataArray = dataBase.readAll();

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

	gLog << tr("IP2C database read in %1 ms. Entries read: %2").arg(time.elapsed()).arg(pTargetDatabase->numKnownEntries());
	return true;
}

void IP2CParser::parsingThreadFinished()
{
	bool bSuccessState = currentParsingThread->bSuccessState;
	gLog << tr("IP2C parsing thread has finished.");
	
	delete currentParsingThread;
	currentParsingThread = NULL;
	
	emit parsingFinished(bSuccessState);
}

bool IP2CParser::readDatabase(const QString& filePath)
{
	bool bSuccess = doReadDatabase(filePath);
	emit parsingFinished(bSuccess);
	
	return bSuccess;
}

void IP2CParser::readDatabaseThreaded(const QString& filePath)
{
	if (currentParsingThread != NULL)
	{
		return;
	}
	
	ParsingThread* pParsingThread = new ParsingThread(this, filePath);
	connect(pParsingThread, SIGNAL( finished() ), this, SLOT( parsingThreadFinished() ) );
	
	currentParsingThread = pParsingThread;
	
	pParsingThread->start();
}

bool IP2CParser::readDatabaseVersion1(const QByteArray& dataArray)
{
	int pos = 6; // skip file tag and version number
	const char* data = dataArray.constData();

	while (pos < dataArray.size())
	{
		IP2C::IP2CData entry;

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

		pTargetDatabase->appendEntryToDatabase(entry);
	}

	return true;
}

bool IP2CParser::readDatabaseVersion2(const QByteArray& dataArray)
{
	int pos = 6; // skip file tag and version number
	const char* data = dataArray.constData();

	// We need to store the addresses in such hash table to make sure they
	// are ordered in proper, ascending order. Otherwise the whole library
	// will not work!
	QMap<unsigned, IP2C::IP2CData> hashTable;

	while (pos < dataArray.size())
	{
		// Base entry for each IP read from the file
		IP2C::IP2CData baseEntry;

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
			IP2C::IP2CData entry = baseEntry;

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

	pTargetDatabase->setDatabase(hashTable.values());

	return true;
}

void IP2CParser::readTextDatabase(QByteArray& textDatabase, Countries& countries)
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
		IP2C::IP2CData entry;
		bool ok = true;

		if(!sc.checkToken(TK_StringConst)) break; // ipStart
		entry.ipStart = sc->str.toUInt(&ok);
		if(!ok || !sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // ipEnd
		entry.ipEnd = sc->str.toUInt(&ok);
		if(!ok || !sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // Register
		if(!sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // date assigned
		if(!sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // 2 char country
		if(!sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // 3 char country
		entry.country = sc->str;
		if(!sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // country string
		entry.countryFullName = sc->str;


		if (countries.contains(entry.country))
		{
			countries[entry.country].append(entry);
		}
		else
		{
			QList<IP2C::IP2CData> list;
			list.append(entry);
			countries[entry.country] = list;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

IP2CParser::ConstructorDestructorParserStateSetter::ConstructorDestructorParserStateSetter(IP2CParser* pParser)
{
	this->pParser = pParser;
	pParser->bIsParsing = true;
}

IP2CParser::ConstructorDestructorParserStateSetter::~ConstructorDestructorParserStateSetter()
{
	pParser->bIsParsing = false;
}

////////////////////////////////////////////////////////////////////////////////

void IP2CParser::ParsingThread::run()
{
	bSuccessState = pParser->doReadDatabase(filePath);
}

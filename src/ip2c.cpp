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
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QResource>
#include <QTime>
#include <QTimeLine>
#include <zlib.h>

#include "ip2c.h"
#include "sdeapi/pluginloader.hpp"
#include "sdeapi/scanner.hpp"

IP2C::IP2C(QString file, QUrl netLocation) : file(file), netLocation(netLocation), downloadProgressWidget(NULL)
{
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

bool IP2C::convertAndSaveDatabase(QByteArray& downloadedData)
{
	QTime time;
	time.start();

	if (downloadedData.isEmpty())
		return false;

	// Skip over the header
	int indexOfNewLine = -1;
	while(downloadedData[indexOfNewLine + 1] == '#')
	{
		indexOfNewLine = downloadedData.indexOf('\n', indexOfNewLine + 1);
	}

	// Trim the header
	downloadedData = downloadedData.right(downloadedData.size() - indexOfNewLine);

	Scanner sc = Scanner(downloadedData.constData(), downloadedData.count());
	QByteArray binaryData;

	unsigned fileId = MAKEID('I', 'P', '2', 'C');
	unsigned short version = 1;
	binaryData += QByteArray((const char*)&fileId, 4);
	binaryData += QByteArray((const char*)&version, sizeof(unsigned short));

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

		binaryData += QByteArray((const char*)&entry.ipStart, sizeof(entry.ipStart));
		binaryData += QByteArray((const char*)&entry.ipEnd, sizeof(entry.ipEnd));
		binaryData += entry.country;
		binaryData += QByteArray(1, 0); // array with one null character
	}

	QFile out(file);
    if(out.open(QIODevice::WriteOnly) && out.isWritable())
    {
		out.write(binaryData);
		qDebug("Database converted in %d ms", time.elapsed());
    }
    else
    {
    	return false;
    }

    return true;
}

void IP2C::downloadDatabase(QStatusBar *statusbar)
{
	if(downloadProgressWidget != NULL)
		delete downloadProgressWidget;
	downloadProgressWidget = new QProgressBar();
	downloadProgressWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	if(statusbar != NULL)
		statusbar->addPermanentWidget(downloadProgressWidget);

//	qDebug() << "Downloading";
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

QPixmap IP2C::flag(unsigned int ipaddress) const
{
	const static unsigned LOCALHOST_BEGIN = QHostAddress("127.0.0.0").toIPv4Address();
	const static unsigned LOCALHOST_END = QHostAddress("127.255.255.255").toIPv4Address();
	const static unsigned LAN_1_BEGIN = QHostAddress("10.0.0.0").toIPv4Address();
	const static unsigned LAN_1_END = QHostAddress("10.255.255.255").toIPv4Address();
	const static unsigned LAN_2_BEGIN = QHostAddress("172.16.0.0").toIPv4Address();
	const static unsigned LAN_2_END = QHostAddress("127.31.255.255").toIPv4Address();
	const static unsigned LAN_3_BEGIN = QHostAddress("192.168.0.0").toIPv4Address();
	const static unsigned LAN_3_END = QHostAddress("192.168.255.255").toIPv4Address();

	const QString unknown = ":flags/unknown";

	if (ipaddress >= LOCALHOST_BEGIN && ipaddress <= LOCALHOST_END)
	{
		return QPixmap(":flags/localhost-small");
	}

	if (ipaddress >= LAN_1_BEGIN && ipaddress <= LAN_1_END
	||	ipaddress >= LAN_2_BEGIN && ipaddress <= LAN_2_END
	||	ipaddress >= LAN_3_BEGIN && ipaddress <= LAN_3_END)
	{
		return QPixmap(":flags/lan-small");
	}

	QString country = lookupIP(ipaddress);
	if (country.isEmpty())
	{
		printf("Unrecognized IP address: %s (%u)\n", QHostAddress(ipaddress).toString().toAscii().constData(), ipaddress);
		return QPixmap(":flags/unknown-small");
	}

	QString resName = ":flags/" + country;
	QResource res(resName);

	if (!res.isValid())
	{
		printf("No flag for country: %s\n", country.toAscii().constData());
		return QPixmap(":flags/unknown-small");
	}

	return QPixmap(resName);
}

QString IP2C::lookupIP(unsigned int ipaddress) const
{
	if(!read)
		return QString();

	foreach(const IP2CData entry, database)
	{
		if(ipaddress >= entry.ipStart && ipaddress <= entry.ipEnd)
		{
			return entry.country;
		}
	}

	return QString();
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
		qDebug() << "IP2C database is not in compacted format. Performing conversion!";
		QByteArray contents = db.readAll();

		if (!convertAndSaveDatabase(contents))
		{
			qDebug() << "Conversion failed";
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
	const char* data = dataArray.constData();

	// Currently Doomseeker ignores the IP2C database version.
	int pos = 6; // skip the signature and the version
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

		database << entry;
	}
	qDebug("IP2C Database read in %d ms. Entries read: %d", time.elapsed(), database.size());

	emit databaseUpdated();
	return true;
}

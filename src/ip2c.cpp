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
#include <zlib.h>

#include "ip2c.h"
#include "sdeapi/scanner.hpp"

IP2C::IP2C(QString file, QUrl netLocation) : file(file), netLocation(netLocation)
{
	read = readDatabase();
	www = new WWW();
	www->setUserAgent(QString("Doomseeker/") + QString(VERSION));
}

IP2C::~IP2C()
{
	if(www != NULL)
		delete www;
}

void IP2C::downloadDatabase()
{
	connect(www, SIGNAL( fileDone(QByteArray&, const QString&) ), this, SLOT( processHttp(QByteArray&, const QString&) ));
	www->getUrl(netLocation);
}

QPixmap IP2C::flag(unsigned int ipaddress) const
{
	const QString unknown = ":flags/unknown.gif";

	QString country = lookupIP(ipaddress);
	if (country.isEmpty())
	{
		printf("Unrecognized IP address: %s (%u)\n", QHostAddress(ipaddress).toString().toAscii().constData(), ipaddress);
		return QPixmap(unknown);
	}

	QString resName = ":flags/" + country + ".gif";
	QResource res(resName);

	if (!res.isValid())
	{
		printf("No flag for country: %s\n", country.toAscii().constData());
		return QPixmap(unknown);
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
	// Get the data and uncompress it.
	qDebug() << data.size();
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
			QFile out(file);
			if(out.open(QIODevice::WriteOnly) && out.isWritable())
				out.write(uncompressedData);
		}

		tmp.remove();
	}

	readDatabase();
}

bool IP2C::readDatabase()
{
	QTime time;
	time.start();

	QFile db(file);
	if(!db.exists() || !db.open(QIODevice::ReadOnly|QIODevice::Text) || !db.isReadable())
		return false;

	// Skip over the header
	char firstChar = 0;
	while(db.getChar(&firstChar) && firstChar == '#')
		db.readLine();

	// Put the character back
	db.seek(db.pos()-1);

	QByteArray dbData = db.readAll();
	Scanner sc = Scanner(dbData.constData(), dbData.count());
	while(sc.tokensLeft())
	{
		IP2CData entry;
		bool ok = true;

		if(!sc.checkToken(TK_StringConst)) break; // ipStart
		entry.ipStart = sc.str.toInt(&ok);
		if(!ok || !sc.checkToken(',')) break;
		if(!sc.checkToken(TK_StringConst)) break; // ipEnd
		entry.ipEnd = sc.str.toInt(&ok);
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

		database << entry;
	}
	qDebug("IP2C Database read in %d ms. Entries read: %d", time.elapsed(), database.size());

	emit databaseUpdated();
	return true;
}

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
#include <QHttp>
#include <QTime>
#include <zlib.h>

#include "ip2c.h"
#include "sdeapi/scanner.hpp"

IP2C::IP2C(QString file, QUrl netLocation) : file(file), netLocation(netLocation), http(NULL)
{
	read = readDatabase();
}

IP2C::~IP2C()
{
	if(http != NULL)
		delete http;
}

void IP2C::downloadDatabase()
{
	if(http != NULL)
		delete http;
	http = new QHttp(netLocation.host(), netLocation.port(80));
	http->get(netLocation.encodedPath() + "?" + netLocation.encodedQuery());
	connect(http, SIGNAL( done(bool) ), this, SLOT( processHttp(bool) ));
}

QString IP2C::lookupIP(unsigned int ipaddress) const
{
	if(!read)
		return QString();

	foreach(const IP2CData entry, database)
	{
		if(ipaddress >= entry.ipStart && ipaddress <= entry.ipEnd)
			return entry.country;
	}

	return QString();
}

void IP2C::processHttp(bool error)
{
	if(http == NULL)
		return;

	if(!error)
	{
		// Get the data and uncompress it.
		QByteArray data = http->readAll();
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
	}

	delete http;
	http = NULL;

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
	qDebug("IP2C Database read in %d ms.", time.elapsed());

	emit databaseUpdated();
	return true;
}

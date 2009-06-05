//------------------------------------------------------------------------------
// ip2c.h
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

#ifndef __IP2C_H__
#define __IP2C_H__

#include <QHostAddress>
#include <QHttp>
#include <QList>
#include <QString>
#include <QUrl>

#include "global.h"

class MAIN_EXPORT IP2C : public QObject
{
	Q_OBJECT

	public:
		struct IP2CData
		{
			public:
				unsigned int	ipStart;
				unsigned int	ipEnd;
				QString			country;
		};

		IP2C(QString file, QUrl netLocation);
		~IP2C();

		void	downloadDatabase();
		bool	isRead() const { return read; }
		QString	lookupIP(unsigned int ipaddress) const;
		QString	lookupIP(const QHostAddress &ipaddress) const { return lookupIP(ipaddress.toIPv4Address()); }

	public slots:
		bool	readDatabase();

	signals:
		void	databaseUpdated();

	private:
		QList<IP2CData>	database;
		QHttp			*http;
		QString			file;
		QUrl			netLocation;
		bool			read;

	private slots:
		void	processHttp(bool error);
};

#endif /* __IP2C_H__ */

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
#include <QList>
#include <QPixmap>
#include <QProgressBar>
#include <QStatusBar>
#include <QString>
#include <QUrl>

#include "global.h"
#include "wadseeker/www.h"

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

		void			downloadDatabase(QStatusBar *statusbar=NULL);
		const QString& 	filename() { return file; }
		QPixmap			flag(unsigned int ipaddress) const;
		QPixmap 		flag(const QHostAddress& ipaddress) const { return flag(ipaddress.toIPv4Address()); }
		bool			isRead() const { return read; }
		QString			lookupIP(unsigned int ipaddress) const;
		QString			lookupIP(const QHostAddress &ipaddress) const { return lookupIP(ipaddress.toIPv4Address()); }
		bool			needsUpdate();

	public slots:
		bool	readDatabase();

	signals:
		void	databaseUpdated();

	protected:
		/**
		 *	Converts downloaded text database to a compacted binary file.
		 *	The name of the new file is IP2C::file
		 *
		 *	The format of the compacted file is:
		 *	(all strings are null terminated)
		 *	Header:
		 *	@code
		 *	TYPE			LENGTH		DESCRIPTION
		 *  -----------------------------------------------------
		 *	unsigned long	4			'I' 'P' '2' 'C' bytes
		 *	unsigned short	2			Version
		 *	@endcode
		 *
		 *	Block repeated until EOF:
		 *	@code
		 *	TYPE			LENGTH		DESCRIPTION
		 *  -----------------------------------------------------
		 *	unsigned long	4			Beginning of an IP range
		 *	unsigned long	4			End of an IP range
		 *	string			N/A			Country name abbreviation
		 *	@endcode
		 */
		bool	convertAndSaveDatabase(QByteArray& downloadedData);

	private:
		QList<IP2CData>	database;
		QString			file;
		QUrl			netLocation;
		bool			read;
		QProgressBar	*downloadProgressWidget;
		WWW*			www;

	private slots:
		void	downloadProgress(int value, int max);
		void	processHttp(QByteArray& data, const QString& filename);
};

#endif /* __IP2C_H__ */

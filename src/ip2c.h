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

#include <QHash>
#include <QHostAddress>
#include <QList>
#include <QPixmap>
#include <QProgressBar>
#include <QStatusBar>
#include <QString>
#include <QUrl>

#include "global.h"

/**
 *	@brief Flag and name of the country.
 */
struct MAIN_EXPORT CountryInfo
{
	bool			valid;
	const QPixmap*	flag;
	QString			name;

	bool isFlagOk() const
	{
		return valid && flag != NULL && !flag->isNull();
	}
};

/**
 *	@brief IP to Country database handler.
 *
 *	IP2C class provides an interface for translating IP addresses into country
 *	names they are in. Additional methods allow to retrieve country's flag
 *	picture for given IP.
 *	@see IP2CParser
 */
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
				QString			countryFullName;

				IP2CData()
				{
					ipStart = ipEnd = 0;
				}

				/**
				 *	IP2CData struct is valid when ipStart is different from
				 *	ipEnd.
				 */
				bool	isValid() const
				{
					return ipStart != ipEnd;
				}
		};

		IP2C();
		~IP2C();
		

		/**
		 *	@brief Adds new country entry to the database.
		 *
		 *	Makes sure the database is sorted in ascending order.
		 */
		void			appendEntryToDatabase(const IP2CData& entry);		

		bool			isRead() const { return read; }

		/**
		 *	Returns a reference to the structure describing the country.
		 */
		const IP2CData&	lookupIP(unsigned int ipaddress) const;
		const IP2CData&	lookupIP(const QHostAddress &ipaddress) const { return lookupIP(ipaddress.toIPv4Address()); }
		
		int				numKnownEntries() const { return database.size(); }

		/**
		 *	Returns country information based on given IP.
		 */
		CountryInfo		obtainCountryInfo(unsigned int ipaddress);
		CountryInfo		obtainCountryInfo(const QHostAddress& ipaddress) { return obtainCountryInfo(ipaddress.toIPv4Address()); }
		
		/**
		 *	@brief Sets database contents to the list specified.
		 *
		 *	To avoid performance issues it is already assumed that the specified
		 *	list is sorted.
		 */
		void			setDatabase(const QList<IP2CData>& sortedCountryData) { database = sortedCountryData; }

	protected:
		const QPixmap&	flag(unsigned int ipaddress, const QString& countryShortName);

		inline bool		isLANAddress(unsigned ipv4Address)
		{
			const static unsigned LAN_1_BEGIN = QHostAddress("10.0.0.0").toIPv4Address();
			const static unsigned LAN_1_END = QHostAddress("10.255.255.255").toIPv4Address();
			const static unsigned LAN_2_BEGIN = QHostAddress("172.16.0.0").toIPv4Address();
			const static unsigned LAN_2_END = QHostAddress("172.31.255.255").toIPv4Address();
			const static unsigned LAN_3_BEGIN = QHostAddress("192.168.0.0").toIPv4Address();
			const static unsigned LAN_3_END = QHostAddress("192.168.255.255").toIPv4Address();

			return (
					(ipv4Address >= LAN_1_BEGIN && ipv4Address <= LAN_1_END)
				||	(ipv4Address >= LAN_2_BEGIN && ipv4Address <= LAN_2_END)
				||	(ipv4Address >= LAN_3_BEGIN && ipv4Address <= LAN_3_END)
					);
		}

		inline bool		isLocalhostAddress(unsigned ipv4Address)
		{
			const static unsigned LOCALHOST_BEGIN = QHostAddress("127.0.0.0").toIPv4Address();
			const static unsigned LOCALHOST_END = QHostAddress("127.255.255.255").toIPv4Address();

			return (ipv4Address >= LOCALHOST_BEGIN && ipv4Address <= LOCALHOST_END);
		}

	private:
		QList<IP2CData>				database;
		const QPixmap				flagLan;
		const QPixmap				flagLocalhost;
		const QPixmap				flagUnknown;
		QHash<QString, QPixmap>		flags;
		const IP2CData				invalidData;
		bool						read;
};

#endif /* __IP2C_H__ */

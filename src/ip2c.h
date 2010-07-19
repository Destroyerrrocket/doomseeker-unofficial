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
 *
 *	Class accepts text database from:
 * 	http://software77.net/geo-ip
 *	The first time the text database is read it is compacted into a smaller
 *	format and stored on the drive.
 *	@see convertAndSaveDatabase()
 *
 *
 *	Compacted database file format, version 1:
 *	(all strings are null terminated)
 *	Header:
 *	@code
 *	TYPE			LENGTH		DESCRIPTION
 *  -----------------------------------------------------
 *	unsigned long	4			'I' 'P' '2' 'C' bytes
 *	unsigned short	2			Version (equal to 1)
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
 *
 *	Compacted database file format, version 2:
 *	(all strings are null terminated)
 *	Header:
 *	@code
 *	TYPE			LENGTH		DESCRIPTION
 *  -----------------------------------------------------
 *	unsigned long	4			'I' 'P' '2' 'C' bytes
 *	unsigned short	2			Version (equal to 2)
 *	@endcode
 *
 *	Block repeated until EOF:
 *	@code
 *	TYPE			LENGTH		DESCRIPTION
 *  -----------------------------------------------------
 *	string			N/A			Country full name
 *	string			N/A			Country abbreviation
 *	unsigned long	4			Number of IP Blocks (N_IP_BLOCKS)

 *  -- BLOCK: repeated N_IP_BLOCKS times.
 *	unsigned long	4			Beginning of an IP range
 *	unsigned long	4			End of an IP range
 *	-- END OF BLOCK
 *	@endcode
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

		IP2C(QString file);
		~IP2C();

		const QString& 	filename() { return file; }

		bool			isRead() const { return read; }

		/**
		 *	Returns a reference to the structure describing the country.
		 */
		const IP2CData&	lookupIP(unsigned int ipaddress) const;
		const IP2CData&	lookupIP(const QHostAddress &ipaddress) const { return lookupIP(ipaddress.toIPv4Address()); }

		/**
		 *	Returns country information based on given IP.
		 */
		CountryInfo		obtainCountryInfo(unsigned int ipaddress);
		CountryInfo		obtainCountryInfo(const QHostAddress& ipaddress) { return obtainCountryInfo(ipaddress.toIPv4Address()); }

	public slots:
		bool			readDatabase();
		
	signals:
		void			countryDataUpdated();

	protected:
		/**
		 *	Key value is the abbreviation of the country name.
		 */
		typedef QHash<QString, QList<IP2CData> > 					Countries;
		typedef QHash<QString, QList<IP2CData> >::iterator 			CountriesIt;
		typedef QHash<QString, QList<IP2CData> >::const_iterator 	CountriesConstIt;

		/**
		 *	Makes sure the database is sorted in ascending order.
		 */
		void			appendEntryToDatabase(const IP2CData& entry);

		/**
		 *	Converts downloaded text database to a compacted binary file.
		 *	The name of the new file is IP2C::file.
		 */
		bool			convertAndSaveDatabase(QByteArray& downloadedData);

		/**
		 *	Converts previously created by readTextDatabase() countries hash
		 *	table into an output data that can be saved into a file.
		 */
		void	convertCountriesIntoBinaryData(const Countries& countries, QByteArray& output);

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

		bool			readDatabaseVersion1(const QByteArray& dataArray);
		bool			readDatabaseVersion2(const QByteArray& dataArray);

		/**
		 *	Called by convertAndSaveDatabase().
		 *	@param textDatabase - contents of the file, this will be modified
		 *		by this function.
		 *	@param [out] countries - returned hash table of countries.
		 */
		void			readTextDatabase(QByteArray& textDatabase, Countries& countries);

	private:
		QList<IP2CData>			database;
		QString					file;
		const QPixmap			flagLan;
		const QPixmap			flagLocalhost;
		const QPixmap			flagUnknown;
		QHash<QString, QPixmap>	flags;
		const IP2CData			invalidData;
		bool					read;
};

#endif /* __IP2C_H__ */

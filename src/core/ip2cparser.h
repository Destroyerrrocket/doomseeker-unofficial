//------------------------------------------------------------------------------
// ip2cparser.h
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
#ifndef __IP2CPARSER_H__
#define __IP2CPARSER_H__

#include "ip2c.h"
#include <QMutex>
#include <QThread>

/**
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
class IP2CParser : public QObject
{
	Q_OBJECT

	public:
		IP2CParser(IP2C* pTargetDatabase);
		
		/**
		 *	@brief Retrieves the IP2C database this parser operates on.
		 */
		IP2C*					ip2c() const { return pTargetDatabase; }
		
		/**
		 *	@brief For multi-threading purposes. If this is true it is not 
		 *	recommended to delete this object nor the underlying IP2C database.
		 */
		bool					isParsing() const { return bIsParsing; }
		
		bool					readDatabase(const QString& filePath);	
		void					readDatabaseThreaded(const QString& filePath);	
		
	signals:
		/**
		 *	@brief A signal emitted when parser finishes its job.
		 *
		 *	It is not safe to delete IP2CParser object after readDatabase()
		 *	call and before this signal is emitted.
		 */
		void					parsingFinished(bool bSuccess);
		
	protected:
		/**
		 *	@brief Sets states for IP2C when being constructed and destructed.
		 *
		 *	//TODO: A nicer name?
		 */
		class ConstructorDestructorParserStateSetter
		{
			public:
				ConstructorDestructorParserStateSetter(IP2CParser* pParser);
				~ConstructorDestructorParserStateSetter();
			
			private:
				IP2CParser*	pParser;
		};
	
		class ParsingThread : public QThread
		{
			public:
				bool			bSuccessState;
				QString			filePath;
				IP2CParser*		pParser;
				
				
				ParsingThread(IP2CParser* pParser, const QString& filePath)
				{
					bSuccessState = false;
					this->filePath = filePath;
					this->pParser = pParser; 
				}
			
				void			run();
		};
	
		/**
		 *	Key value is the abbreviation of the country name.
		 */
		typedef QHash<QString, QList<IP2C::IP2CData> > 					Countries;
		typedef QHash<QString, QList<IP2C::IP2CData> >::iterator 		CountriesIt;
		typedef QHash<QString, QList<IP2C::IP2CData> >::const_iterator 	CountriesConstIt;	
	
		bool					bIsParsing;
		ParsingThread*			currentParsingThread;
		
		/**
		 *	@brief Database to which the IP2C parser will save the data it
		 *	retrieves from IP2C file.
		 *
		 *	Since IP2CParser is prepared to work in a separate thread it is
		 *	not advised to delete the IP2C object before parsing is complete.
		 */
		IP2C*					pTargetDatabase;
		
		QMutex					thisLock;
		
		
		/**
		 *	Converts downloaded text database to a compacted binary file.
		 *	The name of the new file is IP2C::file.
		 */
		bool					convertAndSaveDatabase(QByteArray& downloadedData, const QString& outFilePath);		
		
		/**
		 *	Converts previously created by readTextDatabase() countries hash
		 *	table into an output data that can be saved into a file.
		 */
		void					convertCountriesIntoBinaryData(const Countries& countries, QByteArray& output);		
		
		bool					doReadDatabase(const QString& filePath);

		bool					readDatabaseVersion1(const QByteArray& dataArray);
		bool					readDatabaseVersion2(const QByteArray& dataArray);

		/**
		 *	Called by convertAndSaveDatabase().
		 *	@param textDatabase - contents of the file, this will be modified
		 *		by this function.
		 *	@param [out] countries - returned hash table of countries.
		 */
		void					readTextDatabase(QByteArray& textDatabase, Countries& countries);
		
	protected slots:
		void					parsingThreadFinished();
};

#endif

//------------------------------------------------------------------------------
// ip2cupdater.h
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
#ifndef __IP2CUPDATER_H__
#define __IP2CUPDATER_H__

#include <QByteArray>
#include <QObject>
#include "wadseeker/www.h"

/**
 *	@brief IP2CUpdater is responsible for downloading a new version of database
 *	from the site.
 *	
 *	WWW class from Wadseeker library is used to communicate with WWW site.
 *	Since IP2C class remains the core of the whole system for more information
 *	refer there.
 */
class IP2CUpdater : public QObject
{
	Q_OBJECT
	
	public:
		/**
		 *	@brief Checks if IP2C file must be updated.
		 *
		 *	@param filePath - Path to the file. It is assumed that this is the
		 *		IP2Country database file.
		 *	@param minimumUpdateAge - Maximum age of the file for which this 
		 *		method will return false. Cannot be 0.
		 */
		static bool			needsUpdate(const QString& filePath, unsigned minimumUpdateAge);		

		IP2CUpdater();
		~IP2CUpdater();
	
		void				downloadDatabase(const QUrl& netLocation);
		const QByteArray&	downloadedData();

	signals:
		/**
		 *	@brief In case of failure the downloadedData array will be empty.
		 */
		void				databaseDownloadFinished(const QByteArray& downloadedData);		
		void				downloadProgress(int value, int max);
		
	protected:
	
		QByteArray			retrievedData;
		WWW*				www;
		
	protected slots:
		void				downloadProgressSlot(int value, int max);
		void				processHttp(QByteArray& data, const QString& filename);
};

#endif

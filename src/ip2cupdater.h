//------------------------------------------------------------------------------
// ip2cupdater.h
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
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
		IP2CUpdater();
		~IP2CUpdater();
	
		void				downloadDatabase(const QUrl& netLocation);
		const QByteArray&	downloadedData();
		bool				needsUpdate(const QString& filePath);		
		
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

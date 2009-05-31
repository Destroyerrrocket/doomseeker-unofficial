//------------------------------------------------------------------------------
// www.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __WWW_H_
#define __WWW_H_

#include "html.h"
#include "global.h"
#include "link.h"
#include "protocols/ftp.h"
#include "protocols/http.h"
#include <QObject>
#include <QSet>
#include <QStringList>
#include <QUrl>

/**
 *	@brief Search and protocol wrapper class.
 *
 *	WWW class seeks specified files by iterating through
 *	previously defined set of sites and parsing their HTML code
 *	if necessary.
 *
 *	This class handles URLs by selecting proper protocols for them.
 *	It hides how protocols work and provides simple interface:
 *	request a file, inform about download progress and
 *	send a proper signal when download is finished.
 *
 *	URLs with wildcards can be used, see: primaryFile
 */
class WWW : public QObject
{
	Q_OBJECT

	public:
		/**
		 *	Sets time after which to abort connecting
		 *	if no response is received.
		 *	@param seconds - time in seconds
		 */
		static void	setTimeConnectTimeout(int seconds) { Protocol::setTimeConnectTimeoutSeconds(seconds); }

		/**
		 *	Sets time after which to abort downloading
		 *	if data stops coming.
		 *	@param seconds - time in seconds
		 */
		static void	setTimeDownloadTimeout(int seconds) { Protocol::setTimeDownloadTimeoutSeconds(seconds); }

		/**
		 *	Allocates a new instance of Wadseeker and connects protocols
		 *	to signsls.
		 */
		WWW();

		/**
		 *	Issues an abort command on currently working protocol and
		 *	emits aborted() signal when it receives similar signal
		 *	from this protocol. If no protocol is currently working
		 *	it emits aborted() signal right away.
		 */
		void abort();

		/**
		 *	Tells the class to keep looking for file on next available
		 *	site. This method is public because WWW class stops when
		 *	it finds and downloads seeked file. If end-programmer decides
		 *	this is not the file he wants he can resume WWW class progress
		 *	by calling this method.
		 */
		void checkNextSite();

		/**
		 *	Begins the search process. This is the main entry method for this
		 *	class.
		 *	@param seekedFiles - list of filenames we want to get.
		 *	@param primaryFilename - filename used to replace wildcards.
		 *	@see primaryFile
		 */
		void searchFiles(const QStringList& seekedFiles, const QString& primaryFilename);

		/**
		 *	Sets a custom site. This site has priority over all other
		 *	sites and will be searched first. For example a link
		 *	provided by the server can be passed here.
		 *	@param url - a valid, absolute URL
		 */
		void setCustomSite(const QUrl& url) { customSite = url; }

		/**
		 *	Sets a list of primary sites. This is where Wadseeker begins
		 *	it's search and where it returns to if nothing of interest
		 *	was found on other pages.
		 *	@param lst - list of valid, absolute URLs
		 */
		void setPrimarySites(const QStringList& lst) { primarySites = lst; }

	signals:
		/**
		 *	Emitted after abort() method is used and when it's safe
		 *	to assume that WWW finished all its jobs.
		 */
		void	aborted();

		/**
		 *	Emits download progress. Programmer may use this to update
		 *	a progress bar, for example.
		 *	@param done - bytes downloaded
		 *	@param total - size of file
		 */
		void 	downloadProgress(int done, int total);

		/**
		 *	Emitted when WWW class finishes downloading searched file.
		 *	@param data - content of downloaded file
		 *	@param filename - name of downloaded file
		 */
		void 	fileDone(QByteArray& data, const QString& filename);

		/**
		 *	Emitted when WWW wants to communicate about its progress
		 *	with outside world.
		 *	@param msg - content of the message
		 *	@param type - See: Wadseeker::MessageType
		 */
		void 	message(const QString&, Wadseeker::MessageType type);

		/**
		 *	Emitted when there are no more sites available and
		 *	the file is still not found.
		 */
		void 	noMoreSites();

	protected slots:
		void	get(const QUrl&);
		void 	downloadProgressSlot(int done, int total);
		void 	protocolAborted();
		void 	protocolDone(bool success, QByteArray& data, int fileType, const QString& filename);
		void	protocolNameAndTypeOfReceivedFile(const QString&, int);
		void 	messageSlot(const QString&, Wadseeker::MessageType type);

	protected:
		static QString	ignoringMessage;

		bool			aborting;
		Protocol*		currentProtocol;
		Http			http;
		Ftp				ftp;

		QSet<QString>	checkedLinks;
		int				currentPrimarySite;
		QUrl			customSite;
		bool			customSiteUsed;
		QList<QUrl>		directLinks;
		QStringList		filesToFind;

		/**
		 *	This will replace all occurences of %WADNAME%
		 *	string in all URLs.
		 */
		QString			primaryFile;
		QStringList		primarySites;
		QUrl			processedUrl;
		QList<QUrl> 	siteLinks;

		QUrl		constructValidUrl(const QUrl&);
		bool		isWantedFileOrZip(const QString& filename);
		QUrl		nextSite();
};

#endif

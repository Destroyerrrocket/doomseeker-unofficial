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

#include <QObject>
#include <QSet>
#include <QStringList>
#include <QUrl>

#ifdef Q_OS_WIN32
#ifdef WADSEEKER_API_EXPORT
#define WADSEEKER_API	__declspec(dllexport)
#else
#define WADSEEKER_API	__declspec(dllimport)
#endif
#else
#define WADSEEKER_API
#endif

class Protocol;
class Http;
class Ftp;
class Idgames;

/**
 *	@brief Class for manual download of files.
 *
 *	This class hides how protocols work exactly and provides simple interface:
 *	get url, monitor the progress and emit signal when file is done
 *	downloading.
 */
class WADSEEKER_API WWW : public QObject
{
	Q_OBJECT

	public:
		static bool isAbsoluteUrl(const QUrl&);

		/**
		 *	Sets time after which to abort connecting
		 *	if no response is received.
		 *	@param seconds - time in seconds
		 */
		static void	setTimeConnectTimeout(int seconds);

		/**
		 *	Sets time after which to abort downloading
		 *	if data stops coming.
		 *	@param seconds - time in seconds
		 */
		static void	setTimeDownloadTimeout(int seconds);

		/**
		 *	Allocates a new instance of WWW and connects protocols
		 *	to signsls.
		 */
		WWW();
		virtual ~WWW();

		/**
		 *	Used to download a file. WWWSeeker class should override
		 *	this method to be protected, do nothing and always return false
		 *	@param url - a valid, absolute url to a site
		 *	@return false if url is invalid or protocol is not supported
		 */
		virtual bool 	getUrl(const QUrl& url);

		/**
		 *	Sets user agent that is used in HTTP queries.
		 *	@param agent - string to send in HTTP queries
		 */
		void setUserAgent(const QString& agent);

	public slots:
		/**
		 *	Issues an abort command on currently working protocol and
		 *	emits aborted() signal when it receives similar signal
		 *	from this protocol. If no protocol is currently working
		 *	it emits aborted() signal right away.
		 */
		void abort();

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
		 *	Emitted when WWW class was unsuccessful of downloading a file.
		 */
		void	fail();

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
		void 	message(const QString&, int type);

	protected slots:
		virtual void	get(const QUrl&);
		void 			downloadProgressSlot(int done, int total);
		virtual void	protocolAborted();
		virtual void	protocolDone(bool success, QByteArray& data, int fileType, const QString& filename);
		void 			messageSlot(const QString&, int type);

	protected:
		static QString	ignoringMessage;

		bool			aborting;
		Protocol*		currentProtocol;
		Http*			http;
		Ftp*			ftp;

		QUrl			processedUrl;

		/**
		 *	Executes the abort procedure setting aborting field accordingly
		 *	to the abortCompletely argument.
		 *	@param abortCompletely - abort() slot passes true here, while
		 *		WWWSeeker::skipSite() passes false.
		 */
		void			abortExec(bool abortCompletely);

		QUrl			constructValidUrl(const QUrl&);

		/**
		 *	Executed through setUserAgent. Descendant classes can assign
		 *	the same user-agent to other protocols through this.
		 *	@param agent - string to send in HTTP queries
		 */
		virtual void 	setUserAgentEx(const QString& agent) {}
};

/**
 *	@brief Search and protocol wrapper class.
 *
 *	WWWSeeker class seeks specified files by iterating through
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
class WADSEEKER_API WWWSeeker : public WWW
{
	Q_OBJECT

	public:
		WWWSeeker();
		virtual ~WWWSeeker();

		/**
		 *	@return Default URL of Idgames archive, hardcoded into the library.
		 */
		static const QString 	defaultIdgamesUrl();
		
		/**
		 *	Tells the class to keep looking for file on next available
		 *	site. This method is public because WWW class stops when
		 *	it finds and downloads seeked file. If end-programmer decides
		 *	this is not the file he wants he can resume WWW class progress
		 *	by calling this method.
		 */
		void checkNextSite();
		
		/**
		 *	@brief Erases all links that were found during the search.
		 *
		 *	This will leave links like customSite or primarySites untouched.
		 *	However all links that were found during the search will be removed.
		 *	The set containing links that were already checked is also cleared.
		 *
		 *	It is recommended to call this command before a new search is 
		 *	issued.
		 */
		void					clearLinksCache();		

		/**
		 *	Begins the search process. This is the main entry method for this
		 *	class.
		 *	@param seekedFiles - list of filenames we want to get.
		 *	@param primaryFilename - filename used to replace %WADNAME%
		 *		wildcards.
		 *	@param zipFilename - filename used to replace %ZIPNAME wildcards.
		 *	@see primaryFile
		 */
		void searchFiles(const QStringList& seekedFiles, const QString& primaryFilename, const QString& zipFilename);

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

		/**
		 *	Sets parameters for Idgames protocol.
		 *	@param use - @see useIdgames
		 *	@param highPriority - @see idgamesHasHighPriority
		 *	@param archiveURL - URL to the idgames search page.
		 */
		void setUseIdgames(bool use, bool highPriority = false, QString archiveURL = defaultIdgamesUrl());

	public slots:
		/**
		 *	Skips current site and proceeds to the next one in the queue.
		 */
		void				skipSite();

	protected slots:
		void	get(const QUrl&);
		void	protocolAborted();
		void	protocolDone(bool success, QByteArray& data, int fileType, const QString& filename);
		void	protocolNameAndTypeOfReceivedFile(const QString&, int);

	protected:
		QSet<QString>	checkedLinks;
		int				currentPrimarySite;
		QUrl			customSite;
		bool			customSiteUsed;
		QList<QUrl>		directLinks;
		QStringList		filesToFind;

		Idgames*		idgames;
		bool			idgamesUsed;

		/**
		 *	If true, idgames archive will be searched right after
		 *	the custom site.
		 *	If false, idgames archive will be searched after all other
		 *	sites fail.
		 */
		bool			idgamesHasHighPriority;

		/**
		 *	This will replace all occurences of %WADNAME%
		 *	string in all URLs.
		 */
		QString			primaryFile;
		QStringList		primarySites;
		QList<QUrl> 	siteLinks;

		/**
		 *	If true, idgames search will be performed.
		 *	If false, idgames archive will be skipped.
		 */
		bool			useIdgames;

		/**
		 *	This will replace all occurences of %ZIPNAME% string in
		 *	all URLs. Also this will be passed to Idgames::findFile()
		 *	as seeked file parameter.
		 */
		QString			zipFile;

		bool 		getUrl(const QUrl& url) { return false; }

		bool		isWantedFileOrZip(const QString& filename);
		QUrl		nextSite();

		/**
		 *	Calling this method will set idgamesUsed boolean to true
		 */
		void		searchIdgames();

		/**
		 *	@param agent - string to send in Idgames queries
		 */
		virtual void 	setUserAgentEx(const QString& agent);
};

#endif

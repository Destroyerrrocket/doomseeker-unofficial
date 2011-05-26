//------------------------------------------------------------------------------
// wwwseeker.h
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
#ifndef __WWWSEEKER_H__
#define __WWWSEEKER_H__

#include "www.h"

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
		 *	Begins the search process. This is the main entry method for this
		 *	class.
		 *	@param seekedFiles - list of filenames we want to get.
		 *	@param primaryFilename - filename used to replace %WADNAME%
		 *		wildcards.
		 *	@param zipFilename - filename used to replace %ZIPNAME% wildcards.
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
		void			skipSite();

	protected slots:
		void			get(const QUrl&);
		void			protocolAborted();
		void			protocolDone(bool success, QByteArray& data, int fileType, const QString& filename);
		void			protocolNameAndTypeOfReceivedFile(const QString&, int);

	protected:
		QSet<QString>	checkedLinks;
		int				currentPrimarySite;
		QUrl			customSite;
		bool			customSiteChecked;
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

		bool			shouldCheckIdgames() const;

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

		/**
		 *	@brief Erases all links that were found during the search.
		 *
		 *	This will leave links like customSite or primarySites untouched.
		 *	However all links that were found during the search will be removed.
		 *	The set containing links that were already checked is also cleared.
		 */
		void			clearLinksCache();

		bool			hasCustomSiteBeenProcessed() const;
		bool			hasMoreUrls() const;

		bool 			getUrl(const QUrl& url) { return false; }

		bool			isWantedFileOrZip(const QString& filename);
		QUrl			nextSite();

		/**
		 *	@brief Retrieves a URL that should be checked next.
		 *
		 *	The URL is removed from the queue, but its place holder strings
		 *	are not processed. This is done by the nextSite() method which
		 *	this method is called from.
		 *
		 *	@return If no URLs are left an empty URL is returned.
		 *	@see nextSite()
		 */
		QUrl			popNextUrl();

		/**
		 *	Calling this method will set idgamesUsed boolean to true
		 */
		void			searchIdgames();

		/**
		 *	@param agent - string to send in Idgames queries
		 */
		virtual void 	setUserAgentEx(const QString& agent);
};

#endif

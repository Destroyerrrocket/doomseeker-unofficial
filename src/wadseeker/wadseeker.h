//------------------------------------------------------------------------------
// wadseeker.h
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
#ifndef __WADSEEKER_H_
#define __WADSEEKER_H_

#include <QList>
#include <QString>
#include <QStringList>
#include <QUrl>

#include "wadseekerexportinfo.h"
#include "wadseekermessagetype.h"

#define WADSEEKER_CONNECT_TIMEOUT_SECONDS_DEFAULT 30
#define WADSEEKER_DOWNLOAD_TIMEOUT_SECONDS_DEFAULT 120

class SpeedCalculator;
class WadDownloadInfo;
class WadRetriever;
class WWWSeeker;

/**
 *	@mainpage Wadseeker API
 *
 *	@section intro Introduction
 *	Wadseeker is a library for searching for an automatically downloading a
 *	list of mods for Doom.  Wadseeker requires a small subset of the Qt Toolkit,
 *	namely the QtCore and QtNetwork modules.  In addition, to interact with
 *	Wadseeker, you will need to use Qt to connect to the signals.
 */

/**
 *	@brief Main controlling class.
 *
 *	The Wadseeker class provides an interface for searching for and downloading
 *	modifications for Doom engine games.  Wadseeker will search for mods in a
 *	list of locations provided by setPrimarySites.
 *
 *	Wadseeker will ignore a predefined set of file names which it suspects to
 *	be attempts to illegally download commercial game data, so passing in any
 *	name which is found in iwadNames will result in the file being ignored.
 *
 *	@section usage Usage
 *	Using Wadseeker is quite simple, there are three steps required for use:
 *		-#	Initialize an instance of Wadseeker
 *		-#	Configure some values such as the sites to search in and target
 *			directory.
 *		-#	Create a list of wads to search for and pass it into seekWads.
 *
 *	Observe the following example on how to do these things.
 *	@code
 *	Wadseeker ws;
 *	ws.setPrimarySitesToDefault(); // Use the default list of sites
 *	ws.setTargetDirectory("./");
 *	QStringList listOfWads;
 *	listOfWads << "doom2.wad"; // This will be ignored beacuse of iwadNames
 *	listOfWads << "somemod.wad";
 *	listOfWads << "somemod.pk3";
 *	ws.seekWads(listOfWads);
 *	@endcode
 */
class WADSEEKER_API Wadseeker : public QObject
{
	Q_OBJECT

	public:
		/**
		 *	@brief List of default sites to search for files.
		 *
		 *	@see setPrimarySitesToDefault
		 */
		static const QString	defaultSites[];

		/**
		 *	List of file names that will be ignored as they are part of a
		 *	commercial product. Empty string at the end of the array
		 *	is required and indicates the end of the array.
		 *
		 *	@see isForbiddenWad
		 */
		static const QString	forbiddenWads[];

		/**
		 *	@return Default URL of Idgames archive, hardcoded into the library.
		 */
		static const QString 	defaultIdgamesUrl();

		/**
		 *	Runs content of defaultSites array through
		 *	QUrl::toPercentEncoding() and returns a list of so
		 *	encoded strings.
		 */
		static QStringList 		defaultSitesListEncoded();

		/**
		 *	@brief Attempts to detect if the input is on
		 *	       a list of forbidden wads.
		 *
		 *	@param wad
		 *		Name of wad trying to be searched for.
		 *	@return True if Wadseeker will refuse to download the wad specified.
		 *	@see iwadNames
		 */
		static bool				isForbiddenWad(const QString& wad);

		/**
		 * Initializes a new instance of Wadseeker.
		 */
		Wadseeker();

		/**
		 * Deallocates an instance of Wadseeker.
		 */
		~Wadseeker();

		/**
		 * @brief Check if Wadseeker is still working.
		 *
		 * @return True if any search or download is still in progress.
		 */
		bool				isWorking() const;

		/**
		 *	Sets a custom site. This site has priority over all other
		 *	sites and will be searched first. For example a link
		 *	provided by the server can be passed here.
		 *
		 *	Custom site can be set before the seek is started.
		 *
		 *	@param url
		 *		A valid, absolute URL.
		 */
		void				setCustomSite(const QUrl& url);

		/**
		 *	@brief Toggles Idgames enabled state.
		 *
		 *	If true, Idgames search will be performed.
		 *	If false, Idgames archive will be skipped.
		 */
		void 				setIdgamesEnabled(bool bEnabled);

		/**
		 *	@brief Toggles Idgames priority state.
		 *
		 *	Ignored if Idgames is disabled.
		 *
		 *	If true, Idgames archive will be
		 *	searched right after the custom site.
		 *	If false, Idgames archive will be searched after all other
		 *	sites fail.
		 */
		void 				setIdgamesHighPriority(bool bHighPriority);

		/**
		 *	@brief Sets URL to Idgames search page.
		 */
		void 				setIdgamesUrl(QString archiveURL);

		/**
		 *	Sets a list of primary sites. This is where Wadseeker begins
		 *	it's search and where it returns to if nothing of interest
		 *	was found on other pages.
		 *
		 *	Primary sites must be set before the seek is started.
		 *
		 *	@param urlList
		 *		List of valid, absolute URLs
		 */
		void 				setPrimarySites(const QStringList& urlList);

		/**
		 *	Convenience method. Sets list of primary sites to sites
		 *	hardcoded into library itself.
		 *
		 *	@see defaultSites
		 */
		void				setPrimarySitesToDefault();

		/**
		 *	Target directory is a directory where all seeked files will
		 *  be saved. If this directory is not writable the seek will fail.
		 *
		 *	Must be set before the seek is started.
		 *
		 *	@param dir
		 *		Path to a writable directory.
		 */
		void				setTargetDirectory(const QString& dir);

		/**
		 *	Library's "entry" method. This is where Wadseeker begins
		 *	to iterate through WWW sites to find all files specified
		 *	on the list. seekStarted() signal is emitted.
		 *
		 *	@param wads
		 *		List of files that will be searched for.
		 *	@return True if seek was started. False otherwise.
		 */
		bool				startSeek(const QStringList& wads);

		/**
		 *	Target directory is a directory where all seeked files will
		 *  be saved.
		 *
		 *	@return Path to a directory last set by setTargetDirectory().
		 */
		QString				targetDirectory() const;

	public slots:
		/**
		 * Issues abort commands throughout entire library. aborted() signal
		 * is emitted when Wadseeker finishes aborting.
		 */
		void				abort();

	signals:
		/**
		 * Emitted when Wadseeker finishes iterating through all
		 * files passed to seekWads() method.
		 *
		 * @param bSuccess
		 *      True if all seeked WADs were installed. False if any single
		 *      one of the WADs was not found or if abort was issued.
		 */
		void allDone(bool bSuccess);

		/**
		 *	@brief Emitted when a particular file is finished.
		 *
		 *	@param filename
		 *		Unique filename for the affected file.
		 *		Emitted previously by seekStarted() signal as an entry on the
		 *		filenames list.
		 */
		void fileDone(const QString& filename);

		/**
		 *	@brief Emits download progress.
		 *
		 *	Programmer may use this for example to update
		 *	a progress bar.
		 *
		 *	@param filename
		 *		Unique filename for the affected file.
		 *		Emitted previously by seekStarted() signal as an entry on the
		 *		filenames list.
		 *	@param done
		 *		Bytes already downloaded.
		 *	@param total
		 *		Total size of the downloaded file.
		 */
		void fileDownloadProgress(const QString& filename, int done, int total);

		void fileMessage(const QString& filename, const QString& message, WadseekerLib::MessageType type);

		/**
		 *	Emitted when Wadseeker wants to communicate about its progress
		 *	with outside world.
		 *
		 *	@param msg - content of the message
		 *	@param type - See: Wadseeker::MessageType
		 */
		void message(const QString& msg, WadseekerLib::MessageType type);

		/**
		 *	@brief Emitted when Wadseeker begins the seek operation.
		 *
		 *	@param filenames
		 *		Contains unique names of all files that will be seeked.
		 *		Wadseeker will continue to refer to those filenames in other
		 *		signals.
		 */
		void seekStarted(const QStringList& filenames);

		/**
		 * @brief Emitted when a WWW site finishes download.
		 */
		void siteFinished(const QUrl& site);

		/**
		 * @brief Emitted when a WWW site is being downloaded.
		 */
		void siteProgress(const QUrl& site, qint64 bytes, qint64 total);

		/**
		 * @brief Emitted when a WWW site redirects to a different site.
		 */
		void siteRedirect(const QUrl& oldUrl, const QUrl& newUrl);

		/**
		 * @brief Emitted when a download of a WWW site starts.
		 */
		void siteStarted(const QUrl& site);

	private:
		class SeekParameters
		{
			public:
				bool bIdgamesEnabled;
				bool bIdgamesHighPriority;
				QUrl customSiteUrl;
				QString idgamesUrl;
				QString saveDirectoryPath;
				QStringList seekedWads;
				QStringList sitesUrls;

				SeekParameters();
		};

		class PrivData
		{
			public:
				bool bIsAborting;
				SeekParameters seekParameters;

				/**
				 * This object is created when startSeek() method is called. This will
				 * ensure that the parameters won't change during the seek operation.
				 * When seek is not in progress this is NULL.
				 */
				SeekParameters* seekParametersForCurrentSeek;

				WadRetriever* wadRetriever;
				WWWSeeker* wwwSeeker;
		};

		PrivData d;

		void cleanUpAfterFinish();

		bool isAllFinished() const;

		void setupSitesUrls();

	private slots:
		void fileLinkFound(const QString& filename, const QUrl& url);
		void wadRetrieverFinished();
		void wadRetrieverMessage(const QString& message, WadseekerLib::MessageType type);
		void wadRetrieverDownloadProgress(const WadDownloadInfo& wadDownloadInfo, qint64 current, qint64 total);
		void wadRetrieverWadInstalled(const WadDownloadInfo& wadDownloadInfo);
		void wwwSeekerAttachmentDownloaded(const QString& name, const QByteArray& data);
		void wwwSeekerFinished();
};

#endif


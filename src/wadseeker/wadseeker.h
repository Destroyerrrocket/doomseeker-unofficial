//------------------------------------------------------------------------------
// wadseeker.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __WADSEEKER_H_
#define __WADSEEKER_H_

#include <QList>
#include <QString>
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

class WWW;

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
		 *	Wadseeker library uses a message system that contains
		 *	a type of message together with a string representing it's
		 *	content. Types should be treated as follows:
		 *
		 *	- Notice - this is just an information of what Wadseeker
		 *			 is currently doing
		 *	- Error - something bad happened but Wadseeker is able to continue
		 *			by itself.
		 *	- CriticalError - Wadseeker stops working after such error.
		 *			Programmer should return control to the main application.
		 */
		enum MessageType
		{
			Notice 			= 0,
			Error	 		= 1,
			CriticalError	= 2
		};

		/**
		 * @return author of the library.
		 */
		static const QString	author();

		/**
		 *	List of default sites to search for files.
		 *	@see setPrimarySitesToDefault
		 */
		static const QString defaultSites[];
		/**
		 *	List of file names that will be ignored as they are part of a
		 *	commercial product.
		 *	@see isIwad
		 */
		static const QString iwadNames[];

		/**
		 *	Runs content of defaultSites array through
		 *	QUrl::toPercentEncoding() and returns a list of so
		 *	encoded strings.
		 */
		static QStringList 		defaultSitesListEncoded();


		/**
		 * @return description of the library.
		 */
		static const QString	description();

		/**
		 *	Attempts to detect if the input is an iwad.
		 *	@param wad Name of wad trying to be searched for.
		 *	@return True if the wad specified is an iwad.
		 *	@see iwadNames
		 */
		static bool				isIwad(const QString& wad);

		/**
		 * @return version string of the library.
		 */
		static const QString	version();

		/**
		 * @return Development dates.
		 */
		static const QString	yearSpan();

		/**
		 * Initializes a new instance of Wadseeker.
		 */
		Wadseeker();
		/**
		 * Deallocates an instance of Wadseeker.
		 */
		~Wadseeker();

		/**
		 * Issues abort commands throughout entire library. aborted() signal
		 * is emitted when Wadseeker finishes aborting.
		 */
		void				abort();

		/**
		 * 	@return	true if all files that were passed to seekWads() method
		 *		   	were found.
		 */
		bool				areAllFilesFound() const;

		/**
		 *	@return	list of files that weren't found
		 */
		const QStringList&	filesNotFound() const;

		/**
		 *	Library's "entry" method. This is where Wadseeker begins
		 *	to iterate through WWW sites to find all files specified
		 *	on the list.
		 *	@param wads - list of files that will be searched for
		 */
		void 				seekWads(const QStringList& wads);

		/**
		 *	Sets a custom site. This site has priority over all other
		 *	sites and will be searched first. For example a link
		 *	provided by the server can be passed here.
		 *	@param url - a valid, absolute URL
		 */
		void				setCustomSite(const QUrl& url);

		/**
		 *	Sets a list of primary sites. This is where Wadseeker begins
		 *	it's search and where it returns to if nothing of interest
		 *	was found on other pages.
		 *	@param lst - list of valid, absolute URLs
		 */
		void 				setPrimarySites(const QStringList& lst);

		/**
		 *	Convenience method. Sets list of primary sites to sites
		 *	hardcoded into library itself.
		 *	@see defaultSites
		 */
		void				setPrimarySitesToDefault();

		/**
		 *	Target directory is a directory where all seeked files will
		 *  be saved.
		 */
		void				setTargetDirectory(const QString& dir);

		/**
		 *	Target directory is a directory where all seeked files will
		 *  be saved.
		 */
		QString				targetDirectory() const;



	signals:
		/**
		 *	Emitted after abort() method is used and when it's safe
		 *	to assume that Wadseeker finished all its jobs.
		 */
		void aborted();

		/**
		 *	Emitted when Wadseeker finishes iterating through all
		 *	files passed to seekWads() method. This signal is emitted
		 *	regardless if all files were found or not. To check this
		 *	use areAllFilesFound() method.
		 */
		void allDone();

		/**
		 *	Emits download progress. Programmer may use this to update
		 *	a progress bar, for example.
		 *	@param done - bytes downloaded
		 *	@param total - size of file
		 */
		void downloadProgress(int done, int total);

		/**
		 *	Emitted when Wadseeker wants to communicate about its progress
		 *	with outside world.
		 *	@param msg - content of the message
		 *	@param type - See: Wadseeker::MessageType
		 */
		void message(const QString& msg, Wadseeker::MessageType type);

	protected:
		/**
		 * Returns the name of the actual files that will be searched for.
		 * For example, if somemod.wad was searched for, this would return
		 * somemod.wad and somemod.zip
		 * @param wad Absolute file being searched for. (ex: somemod.wad)
		 */
		static QStringList	wantedFilenames(const QString& wad);

	protected slots:
		/**
		 * This slot acts as a pipe to the message signal.
		 * @see message
		 */
		void			messageSlot(const QString& msg,
									Wadseeker::MessageType type);

	private:
		int				iNextWad;
		QString			currentWad;
		QStringList		notFound;
		QStringList		seekedWads;
		QString			targetDir;
		WWW				*www;

		void	nextWad();

	// The following slots are used to pickup progress from the WWW object.
	private slots:
		void 			downloadProgressSlot(int done, int total);
		void			fileDone(QByteArray& data, const QString& filename);
		void			wadFail();
};

#endif


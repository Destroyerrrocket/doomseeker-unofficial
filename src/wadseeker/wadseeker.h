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

#include "global.h"
#include "http.h"
#include <QDir>
#include <QObject>
#include <QUrl>

/**
 * How does this work:
 * 	1.	Entry point is seekWads() method, it copies the list and initializes the wad list iterator
 *		then it launches seekNextWad() method.
 *	2.	seekNextWad() checks if wad list iterator is at the end of the list, if it is it emits allDone()
 *		signal and exits, if not it launched the seekWad(wadname) method.
 *	3.	seekWad() resets state of current Wadseeker class object and launches nextSite() method
 *	4.	nextSite() picks site in order: customLink, directLinks, siteLinks, globalSiteLinks.
 * 		If there are no more sites to pick it emits wadDone signal with flag set to 'wad not found'.
 *		If there are still some sites left it sends a request and awaits finishedReceiving() signal.
 *	5.
 *		a)
 *			Http::finishedReceiving() signal is received. If this is a file it is downloaded
 *			and unpacked if necessary. If this is a site it is parsed
 *			to find any links. Links leading to file with the name of seeked file
 *			are added to directLinks list. Links leading to other sites are
 *			added to siteLinks list.
 *		b)
 *			Http::error signal is received. Proceed to nextSite() (4).
 */
class PLUGIN_EXPORT Wadseeker : public QObject
{
	Q_OBJECT

	public:
		Wadseeker();
		~Wadseeker();

		void seekWads(const QStringList& wads);
		void setCustomSite(const QUrl&);
		void setTargetDirectory(const QString& dir) { targetDirectory = dir; }

	signals:
		void allDone();
		void error(const QString&);
		void wadDone(bool bFound, const QString& wadname);

	protected slots:
		void httpError(const QString&);
		void finishedReceiving(QString);
		void seekWad(const QString& wad);
		void size(unsigned int);
		void sizeUpdate(unsigned howMuch, unsigned howMuchSum, unsigned percent);

	protected:
		static QUrl						globalSiteLinks[];

		QSet<QString>					checkedLinks;
		int								currentGlobalSite;
		QStringList::const_iterator 	currentWad;
		QUrl							customSite;
		bool							customSiteUsed;
		QList<QUrl>	 					directLinks;
		Http 							http;
		QString							seekedWad;
		QList<QUrl> 					siteLinks;
		QString							targetDirectory;
		QUrl							url;
		QStringList						wadnames;

		/**
		 * Retrievies links from HTML file.
		 */
		void							getLinks();
		bool							hasFileReferenceSomewhere(const QStringList& wantedFileNames, const Link& link);
		bool							isDirectLinkToFile(const QStringList& wantedFileNames, const QUrl& link);
		void							nextSite();
		QString							nextWad();

		/**
		 * Returns true if wad file was installed properly.
		 */
		bool							parseFile();
		void 							seekNextWad();

};

#endif


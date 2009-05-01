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
#include <QObject>
#include <QUrl>

/**
 * How does this work:
 * 	1.	Entry point is seekWads() method, it copies the list and initializes the iterator
 *		then it launched seekNextWad() method.
 *	2.	seekNextWad() checks if iterator is at the end of the list, if it is it emits allDone()
 *		signal and exits, if not it launched the seekWad(wadname) method.
 *	3.	seekWad() resets state of current Wadseeker class object and launches nextSite() method
 *	4.	nextSite() picks site in order: directLinks, siteLinks, globalSiteLinks.
 *	5.	Http::finishedReceiving() signal is received. If this is a file it is downloaded
 *		and unpacked if necessary. If this is a site it is parsed
 *		to find any links. Links leading to file with the name of seeked file
 *		are added to directLinks list. Links leading to other sites are
 *		added to siteLinks list. Checked links are added to checkedLinks set.
 */
class PLUGIN_EXPORT Wadseeker : public QObject
{
	Q_OBJECT

	public:
		Wadseeker();
		~Wadseeker();

		void seekWads(const QStringList& wads);

	signals:
		void allDone();
		void wadDone(bool bFound, const QString& wadname);

	protected slots:
		void finishedReceiving(QString);
		void seekWad(const QString& wad);

	protected:
		static QUrl						globalSiteLinks[];

		QSet<QString>					checkedLinks;
		int								currentGlobalSite;
		QStringList::const_iterator 	currentWad;
		QList<Link>	 					directLinks;
		Http 							http;
		QString							seekedWad;
		QList<Link> 					siteLinks;
		QUrl							url;
		QStringList						wadnames;

		bool							isDirectLinkToFile(const QStringList& wantedFileNames, const QUrl& link);
		void							nextSite();
		QString							nextWad();
		void 							seekNextWad();

};

#endif


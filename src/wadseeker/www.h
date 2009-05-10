//------------------------------------------------------------------------------
// www.h
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
#ifndef __WWW_H_
#define __WWW_H_

#include "protocols/ftp.h"
#include "protocols/http.h"
#include <QObject>
#include <QSet>
#include <QString>
#include <QUrl>


class WWW : public QObject
{
	Q_OBJECT

	public:
		WWW();

		void 			abort();
		void			get(const QString& strSeekedWad);
		const QString&	lastFile() const { return lastFilename; }
		const QUrl&		lastUrl() const { return url; }

		void			nextSite();

		void 			reset();

		void 			setCustomSite(const QUrl& u) { customSite = u; }
		void 			setGlobalSiteLinks(const QStringList& l) { globalSiteLinks = l; }

	signals:
		void dataReceived(unsigned howMuch, unsigned howMuchSum, unsigned percent);
		void error(const QString&);
		void finishedReceiving(const QByteArray& data);
		void noMoreSites();
		void notice(const QString&);
		void size(unsigned int);

	protected slots:
		void dataReceivedSlot(unsigned howMuch, unsigned howMuchSum, unsigned percent);
		void httpError(const QString&);
		void httpFinishedReceiving(const QString& error);
		void httpNotice(const QString&);
		void nameOfCurrentlyDownloadedResource(const QString&);
		void sizeSlot(unsigned int);

	protected:
		/**
		 * Retrievies links from HTML file.
		 */
		void			getLinks();

		QSet<QString>	checkedLinks;
		int				currentGlobalSite;
		QUrl			customSite;
		bool			customSiteUsed;
		QStringList		globalSiteLinks;
		QList<QUrl>		directLinks;
		QString			seekedWad;
		QList<QUrl> 	siteLinks;

		bool	bAbort;
		QString lastFilename;
		Http	http;
		Ftp		ftp;
		QUrl	url;
};

#endif

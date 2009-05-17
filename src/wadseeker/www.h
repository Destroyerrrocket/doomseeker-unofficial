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

#include "html.h"
#include "global.h"
#include "link.h"
#include "protocols/http.h"
#include <QObject>
#include <QSet>
#include <QStringList>
#include <QUrl>

class WADSEEKER_API WWW : public QObject
{
	Q_OBJECT

	public:
		WWW();

		void abort();
		void checkNextSite();
		void searchFiles(const QStringList& seekedFiles, const QString& primaryFilename);
		void setCustomSite(const QUrl& url) { customSite = url; }
		void setPrimarySites(const QStringList& lst) { primarySites = lst; }

	signals:
		void 	downloadProgress(int done, int total);
		void 	fileDone(QByteArray& data, const QString& filename);
		void 	message(const QString&, Wadseeker::MessageType type);
		void 	noMoreSites();

	protected slots:
		void	get(const QUrl&);
		void 	downloadProgressSlot(int done, int total);
		void 	protocolAborted();
		void 	protocolDone(bool success, QByteArray& data, int fileType, const QString& filename);
		void 	messageSlot(const QString&, Wadseeker::MessageType type);

	protected:
		static bool			hasFileReferenceSomewhere(const QStringList& wantedFileNames, const Link& link);
		static bool			isDirectLinkToFile(const QStringList& wantedFileNames, const QUrl& link);

		bool			aborting;
		Http			http;

		QSet<QString>	checkedLinks;
		int				currentPrimarySite;
		QUrl			customSite;
		bool			customSiteUsed;
		QList<QUrl>		directLinks;
		QStringList		filesToFind;
		QString			primaryFile;
		QStringList		primarySites;
		QUrl			processedUrl;
		QList<QUrl> 	siteLinks;

		/**
		 * Capitalizes all HTML keywords
		 */
		void		capitalizeHTMLTags(QByteArray&);

		QUrl		constructValidUrl(const QUrl&);

		/**
		 * Finds a HTML tag starting from index in the byte array.
		 * @param byte		- array that will be searched
		 * @param beginAt 	- index from which searching starts
		 * @param end 		- end index of a tag
		 * @return 			- begin index of a tag
		 */
		int			findTag(QByteArray& byte, int beginAt, int* end);

		/**
		 * Extracts links from HTML file.
		 * @param - content of the HTML file, all characters in HTML keywords must be capitalized
		 */
		QList<Link> linksFromHTML(const QByteArray& data);

		/**
		 * Extracts links from HTML file but only those that match "pattern".
		 * @param data - content of the HTML file, all characters in HTML keywords must be capitalized
		 * @param wantedFiles - names of the files we want to get the links to.
		 */
		void		linksFromHTMLByPattern(const QByteArray& data, const QStringList& wantedFiles);

		QUrl		nextSite();
};

#endif

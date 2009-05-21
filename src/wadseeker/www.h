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

class WWW : public QObject
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
		QString			primaryFile;
		QStringList		primarySites;
		QUrl			processedUrl;
		QList<QUrl> 	siteLinks;

		QUrl		constructValidUrl(const QUrl&);

		QUrl		nextSite();
};

#endif

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
#include "wadseekerglobals.h"
#include "www.h"
#include "zip/unzip.h"
#include <QList>
#include <QString>
#include <QStringList>
#include <QUrl>

class PLUGIN_EXPORT Wadseeker : public QObject
{
	Q_OBJECT

	public:
		static QString defaultSites[];

		static QStringList 	defaultSitesListEncoded();

		Wadseeker();

		void				abort();
		bool				areAllFilesFound() { return notFound.isEmpty(); }
		const QStringList&	filesNotFound() { return notFound; }
		void 				seekWads(const QStringList& wads);
		void				setCustomSite(const QUrl& url) { www.setCustomSite(url); }
		void 				setPrimarySites(const QStringList& lst) { www.setPrimarySites(lst); }
		void				setPrimarySitesToDefault();
		void				setTargetDirectory(const QString& dir);
		QString				targetDirectory() { return targetDir; }

	signals:
		void aborted();
		void allDone();
		void downloadProgress(int done, int total);
		void message(const QString& msg, WadseekerMessageType type);

	protected:
		int				iNextWad;
		QString			currentWad;
		QStringList		notFound;
		QStringList		seekedWads;
		QString			targetDir;
		WWW				www;

		void			nextWad();
		QStringList		wantedFilenames(const QString& wad);

	protected slots:
		void 			downloadProgressSlot(int done, int total);
		void			fileDone(QByteArray& data, const QString& filename);
		void			messageSlot(const QString& msg, WadseekerMessageType type);
		void			wadFail();

	private:
		static QString iwadNames[];

		bool	isIwad(const QString&);
};

#endif


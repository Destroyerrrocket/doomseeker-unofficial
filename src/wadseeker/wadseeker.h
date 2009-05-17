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

class WADSEEKER_API WWW;

/**
 * The Wadseeker class provides an interface for searching for and downloading 
 * modifications for Doom engine games.  Wadseeker will search for mods in a 
 * list of locations provided by setPrimarySites.
 */
class WADSEEKER_API Wadseeker : public QObject
{
	Q_OBJECT

	public:
		enum MessageType
		{
			Notice 			= 0,
			Error	 		= 1,
			CriticalError	= 2
		};

		static const QString defaultSites[];

		static QStringList 	defaultSitesListEncoded();

		Wadseeker();
		~Wadseeker();

		void				abort();
		bool				areAllFilesFound() const;
		const QStringList&	filesNotFound() const;
		void 				seekWads(const QStringList& wads);
		void				setCustomSite(const QUrl& url);
		void 				setPrimarySites(const QStringList& lst);
		void				setPrimarySitesToDefault();
		void				setTargetDirectory(const QString& dir);
		QString				targetDirectory() const;

	signals:
		void aborted();
		void allDone();
		void downloadProgress(int done, int total);
		void message(const QString& msg, Wadseeker::MessageType type);

	protected:
		int				iNextWad;
		QString			currentWad;
		QStringList		notFound;
		QStringList		seekedWads;
		QString			targetDir;
		WWW				*www;

		void			nextWad();
		QStringList		wantedFilenames(const QString& wad) const;

	protected slots:
		void 			downloadProgressSlot(int done, int total);
		void			fileDone(QByteArray& data, const QString& filename);
		void			messageSlot(const QString& msg, 
									Wadseeker::MessageType type);
		void			wadFail();

	private:
		static const QString iwadNames[];

		bool	isIwad(const QString&) const;
};

#endif


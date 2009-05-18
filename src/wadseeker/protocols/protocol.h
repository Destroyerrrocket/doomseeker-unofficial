//------------------------------------------------------------------------------
// protocol.h
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
#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include "../wadseeker.h"
#include <QObject>
#include <QTimer>
#include <QUrl>

#define WWW_CONNECT_TIMEOUT_MS 5000
#define WWW_DOWNLOAD_TIMEOUT_MS 1000 * 60

class WADSEEKER_API Protocol : public QObject
{
	Q_OBJECT

	public:
		enum FileType
		{
			Html 	= 0,
			Other 	= 1
		};

		Protocol();
		virtual ~Protocol() {}

		void 	abort();
		void	get(const QUrl&);

	signals:
		void	aborted();
		void 	currentFileSize(int);
		void	dataReadProgress(int done, int total);
		void	done(bool success, QByteArray& data, int fileType, const QString& filename);
		void 	message(const QString& msg, Wadseeker::MessageType type);

	protected slots:
		void			dataReadProgressSlot(int done, int total);
		void			doneSlot(bool error);
		void 			timeout();

	protected:
		bool 		aborting;
		QString		processedFileName;
		QTimer		timeoutTimer;

		virtual void 	abortEx() =0;
		virtual void	doneEx(bool error)=0;
		virtual void 	getEx(const QUrl&) =0;
};

#endif

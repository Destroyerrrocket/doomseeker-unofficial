//------------------------------------------------------------------------------
// protocol.h
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
#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include "../wadseeker.h"
#include <QObject>
#include <QTimer>
#include <QUrl>

class Protocol : public QObject
{
	Q_OBJECT

	public:
		enum FileType
		{
			Html 	= 0,
			Other 	= 1
		};

		/**
		 *	Sets time after which to abort connecting
		 *	if no response is received.
		 *	@param seconds - time in seconds
		 */
		static void	setTimeConnectTimeoutSeconds(int seconds) { timeConnectTimeoutSeconds = seconds; }

		/**
		 *	Sets time after which to abort downloading
		 *	if data stops coming.
		 *	@param seconds - time in seconds
		 */
		static void	setTimeDownloadTimeoutSeconds(int seconds) {timeDownloadTimeoutSeconds = seconds; }

		Protocol();
		virtual ~Protocol() {}

		void 	abort();
		void	get(const QUrl&);

	signals:
		void	aborted();
		void 	currentFileSize(int);
		void	dataReadProgress(int done, int total);
		void	done(bool success, QByteArray& data, int fileType, const QString& filename);
		void	nameAndTypeOfReceivedFile(const QString& name, int fileType);
		void 	message(const QString& msg, Wadseeker::MessageType type);

	protected slots:
		void			dataReadProgressSlot(int done, int total);
		void			doneSlot(bool error);
		void 			timeout();

	protected:
		static int	timeConnectTimeoutSeconds;
		static int	timeDownloadTimeoutSeconds;

		bool 		aborting;
		bool		noData;
		QString		processedFileName;
		QTimer		timeoutTimer;

		virtual void 	abortEx() =0;
		virtual void	doneEx(bool error)=0;
		virtual void 	getEx(const QUrl&) =0;
};

#endif

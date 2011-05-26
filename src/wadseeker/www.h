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

#include <QObject>
#include <QSet>
#include <QStringList>
#include <QUrl>

#include "wadseekerexportinfo.h"

class Protocol;
class Http;
class Ftp;
class Idgames;

/**
 *	@brief Class for manual download of files.
 *
 *	This class hides how protocols work exactly and provides simple interface:
 *	get url, monitor the progress and emit signal when file is done
 *	downloading.
 */
class WADSEEKER_API WWW : public QObject
{
	Q_OBJECT

	public:
		static bool isAbsoluteUrl(const QUrl&);

		/**
		 *	Sets time after which to abort connecting
		 *	if no response is received.
		 *	@param seconds - time in seconds
		 */
		static void	setTimeConnectTimeout(int seconds);

		/**
		 *	Sets time after which to abort downloading
		 *	if data stops coming.
		 *	@param seconds - time in seconds
		 */
		static void	setTimeDownloadTimeout(int seconds);

		/**
		 *	Allocates a new instance of WWW and connects protocols
		 *	to signsls.
		 */
		WWW();
		virtual ~WWW();

		/**
		 *	Used to download a file. WWWSeeker class should override
		 *	this method to be protected, do nothing and always return false
		 *	@param url - a valid, absolute url to a site
		 *	@return false if url is invalid or protocol is not supported
		 */
		virtual bool 	getUrl(const QUrl& url);

		/**
		 *	Sets user agent that is used in HTTP queries.
		 *	@param agent - string to send in HTTP queries
		 */
		void setUserAgent(const QString& agent);

	public slots:
		/**
		 *	Issues an abort command on currently working protocol and
		 *	emits aborted() signal when it receives similar signal
		 *	from this protocol. If no protocol is currently working
		 *	it emits aborted() signal right away.
		 */
		void abort();

	signals:
		/**
		 *	Emitted after abort() method is used and when it's safe
		 *	to assume that WWW finished all its jobs.
		 */
		void	aborted();

		/**
		 *	Emits download progress. Programmer may use this to update
		 *	a progress bar, for example.
		 *	@param done - bytes downloaded
		 *	@param total - size of file
		 */
		void 	downloadProgress(int done, int total);

		/**
		 *	Emitted when WWW class was unsuccessful of downloading a file.
		 */
		void	fail();

		/**
		 *	Emitted when WWW class finishes downloading searched file.
		 *	@param data - content of downloaded file
		 *	@param filename - name of downloaded file
		 */
		void 	fileDone(QByteArray& data, const QString& filename);

		/**
		 *	Emitted when WWW wants to communicate about its progress
		 *	with outside world.
		 *	@param msg - content of the message
		 *	@param type - See: Wadseeker::MessageType
		 */
		void 	message(const QString&, int type);

	protected slots:
		virtual void			get(const QUrl&);
		void 					downloadProgressSlot(int done, int total);
		virtual void			protocolAborted();
		virtual void			protocolDone(bool success, QByteArray& data, int fileType, const QString& filename);
		void 					messageSlot(const QString&, int type);

	protected:
		static const QString	MESSAGE_IGNORE;

		bool					aborting;
		Protocol*				currentProtocol;
		Http*					http;
		Ftp*					ftp;

		QUrl					processedUrl;

		/**
		 *	Executes the abort procedure setting aborting field accordingly
		 *	to the abortCompletely argument.
		 *	@param abortCompletely - abort() slot passes true here, while
		 *		WWWSeeker::skipSite() passes false.
		 */
		void					abortExec(bool abortCompletely);

		QUrl					constructValidUrl(const QUrl&);

		/**
		 *	Executed through setUserAgent. Descendant classes can assign
		 *	the same user-agent to other protocols through this.
		 *	@param agent - string to send in HTTP queries
		 */
		virtual void 			setUserAgentEx(const QString& agent) {}
};

#endif

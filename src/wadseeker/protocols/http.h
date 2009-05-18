//------------------------------------------------------------------------------
// http.h
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
#ifndef __HTTP_H_
#define __HTTP_H_

#include "protocol.h"
#include <QHttp>

class WADSEEKER_API Http : public Protocol
{
	Q_OBJECT

	public:
		enum	HTTPResponseCodes
		{
			OK 			= 200,
			Redirect 	= 302,
		};

		static bool	isHTTPLink(const QUrl&);

		Http();

	signals:
		void	redirect(const QUrl& where);

	protected slots:
		void	headerReceived(const QHttpResponseHeader&);
		void	stateChanged(int);

	protected:
		QHttp*		qHttp;
		FileType	fileType;
		bool		noData;
		bool		redirected;
		QString		redirectUrl;

		void 	abortEx();

		/**
		 * Looks for attachment information in http header.
		 * @return empty string if nothing found or string of values (like filename="something")
		 */
		QString		attachmentInformation(const QHttpHeader&, QString& filename);

		void	disconnectQHttp();

		void 	getEx(const QUrl&);
		void	doneEx(bool error);
		bool 	isHTMLFile(const QHttpHeader& http);
};

#endif

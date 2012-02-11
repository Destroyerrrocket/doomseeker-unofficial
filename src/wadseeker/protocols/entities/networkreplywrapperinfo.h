//------------------------------------------------------------------------------
// networkreplywrapperinfo.h
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
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __NETWORKREPLYWRAPPERINFO_H__
#define __NETWORKREPLYWRAPPERINFO_H__

#include <QNetworkReply>

class NetworkReplySignalWrapper;
class NetworkReplyTimeouter;

class NetworkReplyWrapperInfo
{
	public:
		/**
		 * @brief Default timeout for connection establishment.
		 *
		 * This is the timeout between the call to 
		 * startConnectionTimeoutTimer() and first reception of
		 * downloadProgress() or uploadProgress() signals.
		 */
		static const unsigned DEFAULT_CONNECTION_TIMEOUT_MSECS = 15 * 1000;
		static const unsigned SUGGESTED_PROGRESS_TIMEOUT_MSECS = 60 * 1000;
		
		NetworkReplySignalWrapper* pSignalWrapper;
		NetworkReplyTimeouter* pTimeouter;
		QNetworkReply* pReply;

		NetworkReplyWrapperInfo(QNetworkReply* pReply);
		~NetworkReplyWrapperInfo();

		void deleteMembersLater();
		
		/**
		 * @brief Sets timeout that occurs when progress stops.
		 *
		 * This is the timeout between the receptions of either
		 * downloadProgress() or uploadProgress() signals.
		 *
		 * @param timeoutMsecs
		 *      Time in milliseconds after which the timeout will be
		 *      registered and the underlying QNetworkReply will be
		 *      aborted. Also generates a QNetworkReply::TimeoutError error.
		 *      Set to 0 to disable timeout.
		 */
		void setProgressTimeout(unsigned timeoutMsecs);
		
		/**
		 * @brief Starts the timeout for connection establishment.
		 *
		 * This is the timeout between the call to 
		 * startConnectionTimeoutTimer() and first reception of
		 * downloadProgress() or uploadProgress() signals.
		 *
		 * @param timeoutMsecs
		 *      Time in milliseconds after which the timeout will be
		 *      registered and the underlying QNetworkReply will be
		 *      aborted. Also generates a QNetworkReply::TimeoutError error.
		 */
		void startConnectionTimeoutTimer(unsigned timeoutMsecs = DEFAULT_CONNECTION_TIMEOUT_MSECS);

		/**
		 * @brief NetworkReplyWrapperInfo objects are equal if their pReply
		 * is the same.
		 */
		bool operator==(const NetworkReplyWrapperInfo& other) const;
		bool operator!=(const NetworkReplyWrapperInfo& other) const
		{
			return ! (*this == other);
		}

		bool operator==(const QNetworkReply* pReply) const;
		bool operator!=(const QNetworkReply* pReply) const
		{
			return ! (*this == pReply);
		}
};

#endif

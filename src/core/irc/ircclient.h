//------------------------------------------------------------------------------
// ircclient.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __IRCCLIENT_H__
#define __IRCCLIENT_H__

#include "socketsignalsadapter.h"
#include <QHostInfo>
#include <QTcpSocket>
#include <QTimer>

class IRCClient : public QObject
{
	Q_OBJECT

	public:
		static const int MAX_MESSAGE_LENGTH = 510;
		static const int SAFE_MESSAGE_LENGTH = 400;

		IRCClient();
		~IRCClient();

		void connect(const QString& address, unsigned short port);
		void connectSocketSignals(SocketSignalsAdapter* pAdapter);
		void disconnect();

		bool isConnected() const;

		bool sendMessage(const QString& message);
		void setFakeRecvLag(int lagMs);

	signals:
		void hostLookupError(QHostInfo::HostInfoError errorValue);

		/**
		 *	@brief These are the messages that IRCClient class sends to
		 *	inform the upper layers of progress.
		 *
		 *	These messages should be used for informational purposes only.
		 */
		void infoMessage(const QString& message);
		void ircServerResponse(const QString& message);

	private:
		bool bIsInHostLookupMode;
		QString hostName;
		unsigned short port;
		QTimer recvTimer;
		QTcpSocket socket;

		/**
		 *	@brief Will prefer IPv4 addresses.
		 *
		 *	@return Pointer to a element on the list.
		 */
		const QHostAddress* pickAddress(const QList<QHostAddress>& addressesList);

	private slots:
		void hostLookupFinished(const QHostInfo& hostInfo);
		void receiveSocketData();
		void receiveSocketDataDelayed();

};

#endif

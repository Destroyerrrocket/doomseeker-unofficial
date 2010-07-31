//------------------------------------------------------------------------------
// ircclient.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __IRCCLIENT_H__
#define __IRCCLIENT_H__

#include "socketsignalsadapter.h"
#include <QHostAddress>
#include <QTcpSocket>

class IRCClient : public QObject
{
	Q_OBJECT

	public:
		static const int		MAX_MESSAGE_LENGTH = 510;
	
		IRCClient();
		~IRCClient();

		void					connect(const QHostAddress&	address, unsigned short port);
		void					connectSocketSignals(SocketSignalsAdapter* pAdapter);
		void					disconnect();

		bool					isConnected() const;

		bool					sendMessage(const QString& message);
		
	signals:
		void					ircServerResponse(const QString& message);
		
	protected:
		QTcpSocket				socket;

	protected slots:
		void					receiveSocketData();
};

#endif

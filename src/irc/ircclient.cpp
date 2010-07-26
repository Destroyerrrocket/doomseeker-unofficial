//------------------------------------------------------------------------------
// ircclient.cpp
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
#include "ircclient.h"
#include "log.h"

IrcClient::IrcClient()
{
}

IrcClient::~IrcClient()
{
	disconnect();
}

bool IrcClient::connect(const QHostAddress&	address, unsigned short port)
{
	socket.connectToHost(address, port);
}

void IrcClient::disconnect()
{
	socket.abort();
}

bool IrcClient::isConnected() const
{
	return socket.state() == QTcpSocket::ConnectedState;
}

void IrcClient::receiveSocketData()
{
	gLog << "IRC received: ";
	for (int i = 0; socket.bytesAvailable() > 0; ++i)
	{
		gLog << QString("%1: %2").arg(i).arg( QString(socket.readLine()) );
	}
}

bool IrcClient::sendMessage(const QString& message)
{
	QByteArray messageContent = message.toAscii();
	socket.write(messageContent);
}

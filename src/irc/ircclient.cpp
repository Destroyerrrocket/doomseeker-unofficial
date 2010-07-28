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

IRCClient::IRCClient()
{
	QObject::connect(&socket, SIGNAL( readyRead() ), this, SLOT( receiveSocketData() ) );
}

IRCClient::~IRCClient()
{
	disconnect();
}

void IRCClient::connect(const QHostAddress&	address, unsigned short port)
{
	gLog << QString("Connecting: %1:%2\n").arg(address.toString()).arg(port);
	socket.connectToHost(address, port);
}

void IRCClient::connectSocketSignals(SocketSignalsAdapter* pAdapter)
{
	pAdapter->pSocket = &socket;
	pAdapter->connect(&socket, SIGNAL( connected() ), SLOT( connected() ));
	pAdapter->connect(&socket, SIGNAL( disconnected() ), SLOT( disconnected() ));
	pAdapter->connect(&socket, SIGNAL( error(QAbstractSocket::SocketError) ), SLOT( errorReceived(QAbstractSocket::SocketError) ));
}

void IRCClient::disconnect()
{
	socket.disconnectFromHost();
}

bool IRCClient::isConnected() const
{
	return socket.state() == QTcpSocket::ConnectedState;
}

void IRCClient::receiveSocketData()
{
	gLog << "IRC received: ";
	for (int i = 0; socket.bytesAvailable() > 0; ++i)
	{
		gLog << QString("%1: %2").arg(i).arg( QString(socket.readLine()) );
	}
}

bool IRCClient::sendMessage(const QString& message)
{
	if (!isConnected())
	{
		return false;
	}

	QByteArray messageContent = message.toAscii();
	messageContent.append("\r\n");
	
	gLog << QString("Sending message: %1").arg(QString(messageContent));
	
	qint64 numBytesWritten = socket.write(messageContent);
	
	return numBytesWritten == messageContent.size();
}

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
	this->lookUpId = -1;
	this->bIsInHostLookupMode = false;

	QObject::connect(&socket, SIGNAL( readyRead() ), this, SLOT( receiveSocketData() ) );
}

IRCClient::~IRCClient()
{
	if (this->bIsInHostLookupMode)
	{
		QHostInfo::abortHostLookup(this->lookUpId);
		this->bIsInHostLookupMode = false;
	}

	disconnect();
}

void IRCClient::connect(const QString& address, unsigned short port)
{
	emit infoMessage(tr("IRC: attempting host lookup: %1").arg(address));

	this->bIsInHostLookupMode = true;
	this->port = port;
	this->hostName = address;

	this->lookUpId = QHostInfo::lookupHost(address, this, SLOT( hostLookupFinished(const QHostInfo&) ) );
}

void IRCClient::connectSocketSignals(SocketSignalsAdapter* pAdapter)
{
	pAdapter->pSocket = &socket;
	pAdapter->connect(&socket, SIGNAL( connected() ), SLOT( connected() ));
	pAdapter->connect(&socket, SIGNAL( disconnected() ), SLOT( disconnected() ));
	pAdapter->connect(&socket, SIGNAL( error(QAbstractSocket::SocketError) ), SLOT( errorReceived(QAbstractSocket::SocketError) ));
	pAdapter->connect(this, SIGNAL( hostLookupError(QHostInfo::HostInfoError) ), SLOT( hostLookupError(QHostInfo::HostInfoError) ));
	pAdapter->connect(this, SIGNAL( infoMessage(const QString&) ), SLOT( infoMessage(const QString&) ));
}

void IRCClient::disconnect()
{
	if (isConnected())
	{
		socket.close();
	}
}

void IRCClient::hostLookupFinished(const QHostInfo& hostInfo)
{
	this->bIsInHostLookupMode = false;

	if (hostInfo.error() != QHostInfo::NoError)
	{
		emit hostLookupError(hostInfo.error());
	}
	else
	{
		const QHostAddress* pIp = this->pickAddress(hostInfo.addresses());

		if (pIp == NULL)
		{
			emit hostLookupError(QHostInfo::HostNotFound);
		}
		else
		{
			emit infoMessage(tr("IRC: Connecting: %1:%2 [IP: %3]").arg(this->hostName).arg(this->port).arg(pIp->toString()));
			socket.connectToHost(*pIp, port);
		}
	}
}

bool IRCClient::isConnected() const
{
	return socket.state() == QTcpSocket::ConnectedState && !this->bIsInHostLookupMode;
}

const QHostAddress* IRCClient::pickAddress(const QList<QHostAddress>& addressesList)
{
	const QHostAddress* pIPv6 = NULL;;

	foreach (const QHostAddress& addr, addressesList)
	{
		if (addr.protocol() != QAbstractSocket::IPv4Protocol)
		{
			if (addr.protocol() == QAbstractSocket::IPv6Protocol
			&& pIPv6 == NULL)
			{
				pIPv6 = &addr;
			}
		}
		else
		{
			return &addr;
		}
	}

	// If we didn't return yet we must return the IPv6 addreses.
	// If it is not null that is...
	return pIPv6;
}

void IRCClient::receiveSocketData()
{
	while (socket.canReadLine())
	{
		QString responseLine = socket.readLine();
		emit ircServerResponse(responseLine);
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

	qint64 numBytesWritten = socket.write(messageContent);
	socket.flush();

	return numBytesWritten == messageContent.size();
}

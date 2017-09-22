//------------------------------------------------------------------------------
// ircclient.cpp
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
#include "ircclient.h"
#include "log.h"
#include "lookuphost.h"

IRCClient::IRCClient()
{
	this->bIsInHostLookupMode = false;

	recvTimer.setSingleShot(true);
	setFakeRecvLag(0);

	QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(receiveSocketDataDelayed()));
	QObject::connect(&recvTimer, SIGNAL(timeout()), this, SLOT(receiveSocketData()));
}

IRCClient::~IRCClient()
{
	disconnect();
}

void IRCClient::connect(const QString& address, unsigned short port)
{
	emit infoMessage(tr("IRC: attempting host lookup: %1").arg(address));

	this->bIsInHostLookupMode = true;
	this->port = port;
	this->hostName = address;

	LookupHost::lookupHost(address, this, SLOT( hostLookupFinished(const QHostInfo&) ) );
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
		QByteArray socketData = socket.readLine();
		QString responseLine = QString::fromUtf8(socketData.constData(), socketData.size());
		emit ircServerResponse(responseLine);
	}
}

void IRCClient::receiveSocketDataDelayed()
{
	if (!recvTimer.isActive())
	{
		recvTimer.start();
	}
}

bool IRCClient::sendMessage(const QString& message)
{
	if (!isConnected())
	{
		return false;
	}

	QByteArray messageContent = message.toUtf8();
	messageContent.append("\r\n");

	qint64 numBytesWritten = socket.write(messageContent);
	socket.flush();

	return numBytesWritten == messageContent.size();
}

void IRCClient::setFakeRecvLag(int lagMs)
{
	recvTimer.setInterval(lagMs);
}

//------------------------------------------------------------------------------
// masterresponder.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "masterresponder.h"

#include "responder/respondercfg.h"
#include <QTimer>
#include <QUdpSocket>

class AwaitingClient
{
	public:
		QHostAddress address;
		quint16 port;

		AwaitingClient(const QHostAddress& address, quint16 port)
		{
			this->address = address;
			this->port = port;
		}
};

class MasterResponder::PrivData
{
	public:
		QList<AwaitingClient> awaitingClients;
		QUdpSocket* socket;
};
///////////////////////////////////////////////////////////////////////////////
MasterResponder::MasterResponder(QObject* parent)
: QObject(parent)
{
	d = new PrivData();
	d->socket = new QUdpSocket();
	this->connect(d->socket, SIGNAL(readyRead()),
		SLOT(readPendingDatagram()));
}

MasterResponder::~MasterResponder()
{
	delete d->socket;
	delete d;
}

bool MasterResponder::bind(unsigned short port)
{
	return d->socket->bind(port);
}

QList<QByteArray> MasterResponder::buildResponsePackets()
{
	QList<QByteArray> result;
	QString packet = "";
	for (int i = 0; i < ResponderCfg::numServers(); ++i)
	{
		QString currentPort = QString::number(ResponderCfg::serverPortBase() + i);
		QString tmp = packet + ";" + currentPort;
		if (tmp.toAscii().length() > ResponderCfg::maxPacketSize())
		{
			result.append(packet.toAscii());
			packet = currentPort;
		}
		else
		{
			packet = tmp;
		}
	}
	result.append(packet.toAscii());
	return result;
}

void MasterResponder::readPendingDatagram()
{
	// This mocker awaits for "FAKE" magic number to arrive,
	// then sleeps for a moment to simulate network
	// behavior, and then responds.
	QHostAddress address;
	quint16 port = 0;
	const int MAGIC_SIZE = 4;
	char* buffer = new char[MAGIC_SIZE];
	d->socket->readDatagram(buffer, MAGIC_SIZE, &address, &port);
	QByteArray packet = QByteArray(buffer, MAGIC_SIZE);
	delete buffer;

	qDebug() << "FakePlugin, received master datagram. Magic =" << packet
		<< ", Client: '" << address << ":" << port << "'";
	if (packet == "FAKE")
	{
		d->awaitingClients.append(AwaitingClient(address, port));
		int msec = 10 + qrand() % 300;
		qDebug() << "FakePlugin, sending master response in:" << msec << "ms.";
		QTimer::singleShot(msec, this, SLOT(respond()));
	}
}

void MasterResponder::respond()
{
	AwaitingClient client = d->awaitingClients.takeFirst();
	QList<QByteArray> packets = buildResponsePackets();
	qDebug() << "FakePlugin, sending master response now. Packets count ="
		<< packets.size() << ", Target client: '" << client.address << ":"
		<< client.port << "'";
	foreach (const QByteArray& packet, packets)
	{
		d->socket->writeDatagram(packet, client.address, client.port);
	}
}

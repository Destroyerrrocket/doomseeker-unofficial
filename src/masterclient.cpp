//------------------------------------------------------------------------------
// masterclient.cpp
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "masterclient.h"
#include "main.h"
#include "serverapi/playerslist.h"

#include <QErrorMessage>
#include <QMessageBox>
#include <QUdpSocket>

MasterClient::MasterClient(QHostAddress address, unsigned short port) : QObject(), address(address), port(port)
{
}

MasterClient::~MasterClient()
{
	emptyServerList();
}

void MasterClient::emptyServerList()
{
	for(int i = 0;i < servers.size();i++)
	{
		servers[i]->disconnect();
		servers[i]->setToDelete(true);
	}
	servers.clear();
}

bool MasterClient::hasServer(const Server* server)
{
	for (int i = 0; i < servers.count(); ++i)
	{
		if (server == servers[i])
			return true;
	}

	return false;
}

void MasterClient::notifyBanned(const QString& engineName)
{
	emit message(engineName, tr("You have been banned from the master server."), true);
}

void MasterClient::notifyDelay(const QString& engineName)
{
	emit message(engineName, tr("Could not fetch a new server list from the master because not enough time has past."), true);
}

void MasterClient::notifyUpdate(const QString& engineName)
{
	emit message(engineName, tr("Could not fetch a new server list.  The protocol you are using is too old.  An update may be available."), true);
}

int MasterClient::numPlayers() const
{
	int players = 0;
	foreach(Server* server, servers)
	{
		players += server->playersList()->numClients();
	}

	return players;
}

void MasterClient::refresh()
{
	emptyServerList();

	// Connect to the server
	QUdpSocket socket;
	socket.bind();


	// Make request
	QByteArray request;
	if(!sendRequest(request))
	{
		return;
	}
	socket.writeDatagram(request, address, port);

	bool expectingMorePackets = false;
	do
	{
		if(!socket.hasPendingDatagrams())
		{
			if(!socket.waitForReadyRead(10000))
			{
				return;
			}
		}

		// get data
		qint64 datagramSize = socket.pendingDatagramSize();
		if (datagramSize > 0)
		{
			char* datagram = new char[datagramSize];
			socket.readDatagram(datagram, datagramSize);
			QByteArray data(datagram, datagramSize);
			delete[] datagram;

			if(!readRequest(data, expectingMorePackets))
				return;
		}
		else
		{
			expectingMorePackets = false;
		}
	}
	while(expectingMorePackets);

	emit listUpdated();
}

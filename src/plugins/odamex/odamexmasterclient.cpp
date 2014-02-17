//------------------------------------------------------------------------------
// odamexmasterclient.cpp
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
#include <QBuffer>
#include <QDataStream>

#include "datastreamoperatorwrapper.h"
#include "global.h"
#include "odamexmasterclient.h"
#include "odamexengineplugin.h"
#include "odamexserver.h"

#define MASTER_CHALLENGE		0x000BDBA3

OdamexMasterClient::OdamexMasterClient() : MasterClient()
{
}

bool OdamexMasterClient::getServerListRequest(QByteArray &data)
{
	char challenge[4];
	WRITEINT32(challenge, MASTER_CHALLENGE);

	const QByteArray chall(challenge, 4);
	data.append(chall);

	return true;
}

const EnginePlugin* OdamexMasterClient::plugin() const
{
	return OdamexEnginePlugin::staticInstance();
}

bool OdamexMasterClient::readMasterResponse(QByteArray &data)
{
	// Decompress the response.
	QBuffer ioBuffer(&data);
	ioBuffer.open(QIODevice::ReadOnly);
	QDataStream inStream(&ioBuffer);
	inStream.setByteOrder(QDataStream::LittleEndian);
	DataStreamOperatorWrapper in(&inStream);

	// Check the response code
	int response = in.readQUInt32();


	if (response != MASTER_CHALLENGE)
	{
		return false;
	}

	// Make sure we have an empty list.
	emptyServerList();

	int numServers = in.readQUInt16();
	for(;numServers > 0;numServers--)
	{
		// This might be able to be simplified a little bit...
		quint8 ip1 = in.readQUInt8();
		quint8 ip2 = in.readQUInt8();
		quint8 ip3 = in.readQUInt8();
		quint8 ip4 = in.readQUInt8();

		QString ip = QString("%1.%2.%3.%4").
			arg(ip1, 1, 10, QChar('0')).
			arg(ip2, 1, 10, QChar('0')).
			arg(ip3, 1, 10, QChar('0')).
			arg(ip4, 1, 10, QChar('0'));

		quint16 port = in.readQUInt16();
		OdamexServer *server = new OdamexServer(QHostAddress(ip), port);
		servers() << ServerPtr(server);
	}

	emit listUpdated();
	return true;
}

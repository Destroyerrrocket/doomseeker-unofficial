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

#include "global.h"
#include "odamex/odamexmasterclient.h"
#include "odamex/odamexmain.h"
#include "odamex/odamexserver.h"

#define MASTER_CHALLENGE		0x000BDBA3

OdamexMasterClient::OdamexMasterClient(QHostAddress address, unsigned short port) : MasterClient(address, port)
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

const PluginInfo* OdamexMasterClient::plugin() const
{
	return OdamexMain::get();
}

bool OdamexMasterClient::readMasterResponse(QByteArray &data)
{
	// Decompress the response.
	const char* in = data.data();

	// Check the response code
	int response = READINT32(&in[0]);


	if (response != MASTER_CHALLENGE)
	{
		return false;
	}

	// Make sure we have an empty list.
	emptyServerList();

	int numServers = READINT16(&in[4]);
	int pos = 6;
	for(;numServers > 0;numServers--)
	{
		// This might be able to be simplified a little bit...
		QString ip = QString("%1.%2.%3.%4").
			arg(static_cast<quint8> (in[pos]), 1, 10, QChar('0')).arg(static_cast<quint8> (in[pos+1]), 1, 10, QChar('0')).arg(static_cast<quint8> (in[pos+2]), 1, 10, QChar('0')).arg(static_cast<quint8> (in[pos+3]), 1, 10, QChar('0'));
		OdamexServer *server = new OdamexServer(QHostAddress(ip), READINT16(&in[pos+4]));
		servers.push_back(server);
		pos += 6;
	}
	
	emit listUpdated();
	return true;
}

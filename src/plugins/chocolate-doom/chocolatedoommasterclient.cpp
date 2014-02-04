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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "global.h"
#include "chocolatedoommasterclient.h"
#include "chocolatedoomengineplugin.h"
#include "chocolatedoomserver.h"

#define NET_MASTER_PACKET_TYPE_QUERY 2
#define NET_MASTER_PACKET_TYPE_QUERY_RESPONSE 3

ChocolateDoomMasterClient::ChocolateDoomMasterClient() : MasterClient()
{
}

bool ChocolateDoomMasterClient::getServerListRequest(QByteArray &data)
{
	char challenge[2] = {0, NET_MASTER_PACKET_TYPE_QUERY};

	const QByteArray chall(challenge, 2);
	data.append(chall);

	return true;
}

const EnginePlugin* ChocolateDoomMasterClient::plugin() const
{
	return ChocolateDoomEnginePlugin::staticInstance();
}

bool ChocolateDoomMasterClient::readMasterResponse(QByteArray &data)
{
	// Decompress the response.
	const char* in = data.data();

	// Check the response code
	if (in[1] != NET_MASTER_PACKET_TYPE_QUERY_RESPONSE)
	{
		return false;
	}

	// Make sure we have an empty list.
	emptyServerList();

	int pos = 2;
	while(pos < data.size())
	{
		QString address(&in[pos]);
		pos += address.length()+1;
		QStringList ip = address.split(":");
		ChocolateDoomServer *server = new ChocolateDoomServer(QHostAddress(ip[0]), ip[1].toUShort());
		servers() << server;
	}
	
	emit listUpdated();
	return true;
}

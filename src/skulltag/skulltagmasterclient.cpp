//------------------------------------------------------------------------------
// skulltagmasterclient.cpp
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

#include <QUdpSocket>

#include "global.h"
#include "huffman/huffman.h"
#include "skulltag/skulltagmasterclient.h"
#include "skulltag/skulltagserver.h"

#define MASTER_CHALLENGE		0xC7,0x00,0x00,0x00
#define MASTER_RESPONSE_GOOD	0
#define MASTER_RESPONSE_BANNED	3
#define MASTER_RESPONSE_BAD		4
#define MASTER_RESPONSE_SERVER	1
#define MASTER_RESPONSE_END		2

SkulltagMasterClient::SkulltagMasterClient(QHostAddress address, unsigned short port) : MasterClient(address, port)
{
}

void SkulltagMasterClient::refresh()
{
	// Connect to the server
	QUdpSocket socket;
	socket.connectToHost(address, port);
	if(!socket.waitForConnected(1000))
	{
		printf("%s\n", socket.errorString().toAscii().data());
		return;
	}

	// Send launcher challenge.
	const char challenge[4] = {MASTER_CHALLENGE};
	char challengeOut[12];
	int out = 12;
	g_Huffman.encode(challenge, challengeOut, 4, &out);
	socket.write(challengeOut, out);
	if(!socket.waitForReadyRead(10000))
		return;

	// Decompress the response.
	QByteArray data = socket.readAll();
	const char* in = data.data();
	char packetOut[2000];
	out = 2000;
	g_Huffman.decode(in, packetOut, data.size(), &out);

	// Check the response code
	int response = READINT32(&packetOut[0]);
	if(response == MASTER_RESPONSE_BANNED)
	{
		notifyBanned();
		return;
	}
	else if(response == MASTER_RESPONSE_BAD)
	{
		notifyDelay();
		return;
	}
	else if(response != MASTER_RESPONSE_GOOD)
		return;

	// Make sure we have an empty list.
	emptyServerList();

	quint8 firstByte = READINT8(&packetOut[4]);
	int pos = 5;
	while(firstByte == MASTER_RESPONSE_SERVER)
	{
		// This might be able to be simplified a little bit...
		QString ip = QString("%1.%2.%3.%4").
			arg(static_cast<quint8> (packetOut[pos]), 1, 10, QChar('0')).arg(static_cast<quint8> (packetOut[pos+1]), 1, 10, QChar('0')).arg(static_cast<quint8> (packetOut[pos+2]), 1, 10, QChar('0')).arg(static_cast<quint8> (packetOut[pos+3]), 1, 10, QChar('0'));
		SkulltagServer *server = new SkulltagServer(QHostAddress(ip), READINT16(&packetOut[pos+4]));
		servers.push_back(server);
		pos += 6;
		firstByte = READINT8(&packetOut[pos++]);
	}

	socket.close();

	emit listUpdated();
}

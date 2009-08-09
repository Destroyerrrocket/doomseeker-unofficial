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

#define MASTER_CHALLENGE				5660028
#define MASTER_PROTOCOL_VERSION			1
#define MASTER_RESPONSE_GOOD			0
#define MASTER_RESPONSE_BANNED			3
#define MASTER_RESPONSE_BAD				4
#define MASTER_RESPONSE_WRONGVERSION	5
#define MASTER_RESPONSE_SERVER			1
#define MASTER_RESPONSE_END				2
#define MASTER_RESPONSE_BEGINPART		6
#define MASTER_RESPONSE_ENDPART			7
#define MASTER_RESPONSE_SERVERBLOCK		8

SkulltagMasterClient::SkulltagMasterClient(QHostAddress address, unsigned short port) : MasterClient(address, port)
{
}

bool SkulltagMasterClient::sendRequest(QByteArray &data)
{
	// Send launcher challenge.
	const char challenge[6] = {WRITEINT32_DIRECT(MASTER_CHALLENGE), WRITEINT16_DIRECT(MASTER_PROTOCOL_VERSION)};
	char challengeOut[12];
	int out = 12;
	g_Huffman.encode(challenge, challengeOut, 6, &out);
	const QByteArray chall(challengeOut, out);
	data.append(chall);
	return true;
}

bool SkulltagMasterClient::readRequest(QByteArray &data, bool &expectingMorePackets)
{
	const char* in = data.data();
	char packetOut[2000];
	int out = 2000;
	g_Huffman.decode(in, packetOut, data.size(), &out);

	// Check the response code
	int response = READINT32(&packetOut[0]);
	if(response == MASTER_RESPONSE_BANNED)
	{
		notifyBanned();
		return false;
	}
	else if(response == MASTER_RESPONSE_BAD)
	{
		notifyDelay();
		return false;
	}
	else if(response == MASTER_RESPONSE_WRONGVERSION)
	{
		notifyUpdate();
		return false;
	}
	else if(response != MASTER_RESPONSE_BEGINPART)
		return false;

	// if we are not waiting for packets then this is probably the first time
	// the function was executed.  So store the number of packets.
	if(!expectingMorePackets)
	{
		// Make sure we have an empty list.
		emptyServerList();
		readLastPacket = false;
		numPackets = 0;
		numPacketsRead = 0;
	}
	int packetNum = READINT8(&packetOut[4]);
	if(packetNum+1 > numPackets) // Packet numbers start at 0
		numPackets = packetNum+1;

	quint8 firstByte = READINT8(&packetOut[5]);
	int pos = 6;
	while(firstByte != MASTER_RESPONSE_ENDPART && firstByte != MASTER_RESPONSE_END)
	{
		QString ip = QString("%1.%2.%3.%4").
				arg(static_cast<quint8> (packetOut[pos]), 1, 10, QChar('0')).arg(static_cast<quint8> (packetOut[pos+1]), 1, 10, QChar('0')).arg(static_cast<quint8> (packetOut[pos+2]), 1, 10, QChar('0')).arg(static_cast<quint8> (packetOut[pos+3]), 1, 10, QChar('0'));
		if(firstByte == MASTER_RESPONSE_SERVER)
		{
			SkulltagServer *server = new SkulltagServer(QHostAddress(ip), READINT16(&packetOut[pos+4]));
			servers.push_back(server);
			pos += 6;
		}
		else // MASTER_RESPONSE_SERVERBLOCK
		{
			unsigned int numServersInBlock = READINT8(&packetOut[pos+4]);
			pos += 5;
			for(unsigned int i = 0;i < numServersInBlock;i++)
			{
				SkulltagServer *server = new SkulltagServer(QHostAddress(ip), READINT16(&packetOut[pos]));
				servers.push_back(server);
				pos += 2;
			}
		}
		firstByte = READINT8(&packetOut[pos++]);
	}

	if(firstByte == MASTER_RESPONSE_END)
		readLastPacket = true;
	if(readLastPacket)
		expectingMorePackets = (++numPacketsRead < numPackets);
	else
	{
		expectingMorePackets = true;
		numPacketsRead++;
	}

	return true;
}

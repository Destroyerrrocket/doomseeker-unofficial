//------------------------------------------------------------------------------
// zdaemonmasterclient.cpp
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
#include <QBuffer>
#include <QDataStream>
#include <QHostInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QMutex>
#include <QWaitCondition>

#include "datastreamoperatorwrapper.h"
#include "global.h"
#include "version.h"
#include "zdaemonengineplugin.h"
#include "zdaemonmasterclient.h"
#include "zdaemonserver.h"

#define RESPONSE_KEY	0x594
#define RESPONSE_LIST	0xBDB36

ZDaemonMasterClient::ZDaemonMasterClient() : MasterClient(), netAccessManager(NULL)
{
	// The ZDaemon master is configured such that we can't access it from just
	// the ip.  I could probably change the headers to fix that, but it would
	// require the same amount of work anyways.
	/*QHostInfo info = QHostInfo::fromName("master.zdaemon.org");
	if(info.addresses().size() == 0)
		defaultIP = 0;
	else
		defaultIP = info.addresses().first().toIPv4Address();*/

	connect(this, SIGNAL(request()), this, SLOT(createQueryRequest()));
}

void ZDaemonMasterClient::createQueryRequest()
{
	if(netAccessManager != NULL)
		delete netAccessManager;
	netAccessManager = new QNetworkAccessManager();
//	connect(netAccessManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(listFetched(QNetworkReply *)));

//	QString server = address.toString();
	/*if(address.toIPv4Address() == defaultIP)
		server = "master.zdaemon.org";*/

	/*QString userAgent = Version::name() + "/" + Version::version() + " (ZDaemon Plugin)";

	QNetworkRequest netRequest(QUrl("http://" + server + ":" + QString("%1").arg(port) + "/servers/zdaemon.txt")); // Used to be masterlist.php
	netRequest.setRawHeader("User-Agent", userAgent.toAscii());

	QNetworkReply *reply = netAccessManager->get(netRequest);*/
}

bool ZDaemonMasterClient::getServerListRequest(QByteArray &data)
{
	static const char keyRequest[8] = { 0x93,0x05,0x00,0x00, 0x63,0x3d,0x92,0xeb };

	data.append(QByteArray(keyRequest, 8));
	return true;
}

void ZDaemonMasterClient::listFetched(QNetworkReply *reply)
{
	char serverData[24];
	while(reply->readLine(serverData, 24) != -1)
	{
		// Should be <ip>\t<exp>
		QStringList serverInfo = QString(serverData).split(QChar('\t'));
		if(serverInfo.size() != 2)
			continue;
		QStringList address = serverInfo[0].split(QChar(':'));
		if(serverInfo.size() != 2)
			continue;

		ZDaemonServer *server = new ZDaemonServer(QHostAddress(address[0]), address[1].toUShort());
		servers.append(server);
	}
	reply->deleteLater();

	listWaitCondition.wakeAll();
	emit listUpdated();
}

const EnginePlugin* ZDaemonMasterClient::plugin() const
{
	return ZDaemonEnginePlugin::staticInstance();
}

/*void ZDaemonMasterClient::refresh()
{
	bTimeouted = false;
	emptyServerList();

	// We must do this to avoid thread issues with QNetworkAccessManager
	emit request();
}*/

bool ZDaemonMasterClient::readMasterResponse(QByteArray &data)
{
	QBuffer ioBuffer(&data);
	ioBuffer.open(QIODevice::ReadOnly);
	QDataStream inStream(&ioBuffer);
	inStream.setByteOrder(QDataStream::LittleEndian);
	DataStreamOperatorWrapper in(&inStream);

	quint32 responseType = in.readQUInt32();
	if(responseType == RESPONSE_KEY)
	{
		in.skipRawData(4);
		static char requestHeader[12] = { 0x36, 0xdb, 0x0b, 0x00, 0x6c, 0x00, 0x00, 0x00, 0x33,0x66,0xaa,0x6c };
		key = in.readRawUntilByte('\0');
		QByteArray request(requestHeader, 8);
		request.push_back(QByteArray(key.toAscii().constData(), key.toAscii().length()));
		request.push_back('\0');
		request.push_back(QByteArray(requestHeader+8, 4));
		pGlobalUdpSocket->writeDatagram(request, address, port);

		packetBuffer.clear();
		packetMask = 0; // Keep track of what packet #s we recieved.
		return false;
	}
	else if(responseType == RESPONSE_LIST)
	{
		in.skipRawData(4);
		quint8 numPackets = in.readQUInt8(); // Not to sure about this actually.
		quint32 expectedSize = in.readQUInt32();
		in.skipRawData(12);
		quint8 packetNumber = in.readQUInt8();
		packetMask |= 1<<packetNumber;
		quint32 packetPosition = in.readQUInt32();
		if(packetBuffer.size() == 0)
		{
			char compressionHeader[4]; 
			
			// To big endian for Qt
			for (int i = 3; i >= 0; --i)
			{
				compressionHeader[i] = in.readQInt8();
			}
			
			QByteArray compressedData(compressionHeader, 4);
			packetBuffer.push_back(compressionHeader);
		}
		QByteArray compressedRawData = in.readRawAll();
		packetBuffer.insert(packetPosition+4, compressedRawData);
		
		if(packetBuffer.size() != expectedSize+4 || packetMask != ((1<<numPackets)-1))
			return false;

		// Perform zlib uncompression.
		QByteArray uncompressedData = qUncompress(packetBuffer);
		QBuffer ioBufferUncompressed(&uncompressedData);
		ioBufferUncompressed.open(QIODevice::ReadOnly);
		QDataStream inStreamUncompressed(&ioBufferUncompressed);
		inStreamUncompressed.setByteOrder(QDataStream::LittleEndian);
		DataStreamOperatorWrapper inUncompressed(&inStreamUncompressed);

		quint8 next = 0;
		quint8 lastIP = 0;
		bool secondRound = false; // I have no idea why this needs to be done...
		do
		{
			quint8 nextIP = inUncompressed.readQUInt8();
			if(nextIP < lastIP)
				secondRound = true;
			lastIP = nextIP;
			
			quint8 ip2 = inUncompressed.readQUInt8();
			quint8 ip3 = inUncompressed.readQUInt8();
			quint8 ip4 = inUncompressed.readQUInt8();
			
			QString ip = QString("%1.%2.%3.%4").
				arg(nextIP, 1, 10, QChar('0')).
				arg(ip2, 1, 10, QChar('0')).
				arg(ip3, 1, 10, QChar('0')).
				arg(ip4, 1, 10, QChar('0'));
			do
			{
				if(secondRound)
					inUncompressed.skipRawData(2);

				quint16 port = inUncompressed.readQUInt16();
				ZDaemonServer *server = new ZDaemonServer(QHostAddress(ip), port);
				servers.push_back(server);
				//qDebug() << QString("%1:%2").arg(ip).arg(READINT16(&serverList[pos]));

				next = inUncompressed.readQUInt8();
			} // 3 and 4 mean exp server.
			while((next&1) != 1 && inUncompressed.hasRemaining());
		}
		while(inUncompressed.hasRemaining());
	}
	else
	{
		qDebug() << "Notify banned";
		notifyBanned(); // Uhh not sure?
		return false;
	}

	emit listUpdated();
	return true;
}

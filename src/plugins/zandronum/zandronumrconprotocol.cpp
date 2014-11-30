//------------------------------------------------------------------------------
// zandronumrconprotocol.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "zandronumrconprotocol.h"

#include <QBuffer>
#include <QCryptographicHash>
#include <QDateTime>
#include <QMessageBox>
#include <datastreamoperatorwrapper.h>
#include "huffman/huffman.h"
#include "huffmanqt.h"
#include "zandronumserver.h"

#define RCON_PROTOCOL_VERSION	3

ZandronumRConProtocol::ZandronumRConProtocol(ServerPtr server)
: RConProtocol(server)
{
	set_disconnectFromServer(&ZandronumRConProtocol::disconnectFromServer);
	set_sendCommand(&ZandronumRConProtocol::sendCommand);
	set_sendPassword(&ZandronumRConProtocol::sendPassword);
	set_packetReady(&ZandronumRConProtocol::packetReady);

	// Note: the original rcon utility did TIMEOUT/4.
	// Try to get at least 4 packets in before timing out,
	pingTimer.setInterval(2500);
	connect(&pingTimer, SIGNAL( timeout() ), this, SLOT( sendPong() ));
}

RConProtocol *ZandronumRConProtocol::connectToServer(ServerPtr server)
{
	ZandronumRConProtocol *protocol = new ZandronumRConProtocol(server);

	const char beginConnection[2] = { CLRC_BEGINCONNECTION, RCON_PROTOCOL_VERSION };
	QByteArray encodedConnection = HuffmanQt::encode(beginConnection, 2);

	// Try to connect, up to 3 times
	protocol->setConnected(false);
	for(unsigned int attempts = 0;attempts < 3;attempts++)
	{
		protocol->socket().writeDatagram(encodedConnection, protocol->address(), protocol->port());
		if(protocol->socket().waitForReadyRead(3000))
		{
			int size = protocol->socket().pendingDatagramSize();
			if (size <= 0)
			{
				// [Zalewa]
				// This situation always occurs when trying to connect to
				// a non-existent localhost server on Windows. I don't know
				// if it happens in any other situation.
				continue;
			}
			QByteArray encodedPacket;
			encodedPacket.resize(protocol->socket().pendingDatagramSize());
			protocol->socket().readDatagram(encodedPacket.data(), encodedPacket.size());
			QByteArray packet = HuffmanQt::decode(encodedPacket);
			switch(packet[0])
			{
				case SVRC_BANNED:
					QMessageBox::critical(NULL, tr("Banned"), tr("You have been banned from this server."));
					break;
				default:
				case SVRC_OLDPROTOCOL:
					QMessageBox::critical(NULL, tr("Incompatible Protocol"), tr("The protocol appears to be outdated."));
					break;
				case SVRC_SALT:
					protocol->setConnected(true);
					protocol->salt = QString(packet.mid(1));
					protocol->pingTimer.start();
					return protocol;
			}
			break;
		}
	}
	delete protocol;
	return NULL;
}

void ZandronumRConProtocol::disconnectFromServer()
{
	const char disconnectPacket[1] = { CLRC_DISCONNECT };
	QByteArray encodedDisconnect = HuffmanQt::encode(disconnectPacket, 1);
	socket().writeDatagram(encodedDisconnect, address(), port());
	setConnected(false);
	pingTimer.stop();
	emit disconnected();
}

void ZandronumRConProtocol::sendCommand(const QString &cmd)
{
	char packet[4096];
	packet[0] = CLRC_COMMAND;
	packet[cmd.length()+1] = 0;
	memcpy(packet+1, cmd.toAscii().constData(), cmd.length());
	QByteArray encodedPacket = HuffmanQt::encode(packet, 4096);
	socket().writeDatagram(encodedPacket, address(), port());
}

void ZandronumRConProtocol::sendPassword(const QString &password)
{
	// Calculate the MD5 of the salt + password
	QString hashPassword = salt + password;
	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData(hashPassword.toAscii());
	QByteArray md5 = hash.result().toHex();

	// Create the packet
	char passwordPacket[34];
	passwordPacket[0] = CLRC_PASSWORD;
	memcpy(passwordPacket+1, md5.data(), md5.size());
	passwordPacket[33] = 0;
	QByteArray encodedPassword = HuffmanQt::encode(passwordPacket, 34);

	for(unsigned int i = 0;i < 3;i++)
	{
		socket().writeDatagram(encodedPassword, address(), port());

		if(socket().waitForReadyRead(3000))
		{
			packetReady();
			connect(&socket(), SIGNAL( readyRead() ), this, SLOT( packetReady() ));
			break;
		}
	}
}

void ZandronumRConProtocol::sendPong()
{
	// create a "PONG" packet
	const char pong[1] = { CLRC_PONG };
	QByteArray encodedPong = HuffmanQt::encode(pong, 1);
	socket().writeDatagram(encodedPong, address(), port());
}

void ZandronumRConProtocol::packetReady()
{
	if(!isConnected())
		return;

	while(socket().hasPendingDatagrams())
	{
		int size = socket().pendingDatagramSize();
		char* data = new char[size];
		socket().readDatagram(data, size);
		QByteArray packet = HuffmanQt::decode(data, size);
		delete[] data;

		QBuffer stream(&packet);
		stream.open(QIODevice::ReadOnly);
		processPacket(&stream);
	}
}

void ZandronumRConProtocol::processPacket(QIODevice* ioDevice, bool initial, int maxUpdates)
{
	static const QRegExp colorCode("\\\\c(\\[[a-zA-Z0-9]*\\]|[a-v+\\-!*])");

	// QIODevice is assumed to be already opened at this point.
	QDataStream dataStream(ioDevice);
	dataStream.setByteOrder(QDataStream::LittleEndian);
	DataStreamOperatorWrapper in(&dataStream);

	while(in.hasRemaining() && maxUpdates-- != 0)
	{
		// Determine how we get to the update.
		int update = 0;
		if(initial)
			update = SVRC_UPDATE;
		else
			update = in.readQUInt8();

		switch(update)
		{
			default:
				qDebug() << "Unknown update (" << update << ")";
				return;
			case SVRC_INVALIDPASSWORD:
				emit invalidPassword();
				break;
			case SVRC_LOGGEDIN:
			{
				connect(&socket(), SIGNAL( readyRead() ), this, SLOT( packetReady() ));
				serverProtocolVersion = in.readQUInt8();
				hostName = in.readRawUntilByte('\0');
				emit serverNameChanged(hostName);

				int numUpdates = in.readQUInt8();

				processPacket(ioDevice, true, numUpdates);

				int numStrings = in.readQUInt8();
				while(numStrings-- > 0)
				{
					QString message = in.readRawUntilByte('\0');
					message.replace(colorCode, "\034\\1");
					emit messageReceived(message.trimmed());
				}
				break;
			}
			case SVRC_MESSAGE:
			{
				QString message = QDateTime::currentDateTime().toString("[hh:mm:ss ap] ") + in.readRawUntilByte('\0');
				message.replace(colorCode, "\034\\1");
				emit messageReceived(message);
				break;
			}
			case SVRC_UPDATE:
				int updateType = in.readQUInt8();
				switch(updateType)
				{
					default:
						qDebug() << "Uknown streamlined update (" << update << ")";
						return;
					case SVRCU_MAP:
					{
						QString map = in.readRawUntilByte('\0');
						break;
					}
					case SVRCU_ADMINCOUNT:
					{
						// Unused:
						// int admins = in.readQUint8();
						// !!! MAKE SURE to remove the line below if line above
						// is uncommented!
						in.skipRawData(1);
						break;
					}
					case SVRCU_PLAYERDATA:
					{
						int players = in.readQUInt8();
						this->playersMutable().clear();
						while(players-- > 0)
						{
							QString player = in.readRawUntilByte('\0');
							this->playersMutable().append(Player(player, 0, 0));
						}
						emit playerListUpdated();
						break;
					}
				}
				break;
		}
	}
}

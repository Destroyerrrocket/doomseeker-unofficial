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

#include "log.h"
#include "masterserver/masterclient.h"
#include "plugins/engineplugin.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "serverapi/playerslist.h"
#include "main.h"

#include <QDataStream>
#include <QErrorMessage>
#include <QHostInfo>
#include <QMessageBox>
#include <QUdpSocket>

QUdpSocket* MasterClient::pGlobalUdpSocket = NULL;

MasterClient::MasterClient() : QObject(), cache(NULL)
{
}

MasterClient::~MasterClient()
{
	emptyServerList();
	resetPacketCaching();
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

QString MasterClient::engineName() const
{
	if (plugin() == NULL)
	{
		return "";
	}

	return plugin()->data()->name;
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

void MasterClient::notifyBanned()
{
	emit message(engineName(), tr("You have been banned from the master server."), true);

	Message msg = Message(Message::Types::BANNED_FROM_MASTERSERVER);

	emit messageImportant(msg);
}

void MasterClient::notifyDelay()
{
	emit message(engineName(), tr("Could not fetch a new server list from the master because not enough time has past."), true);
}

void MasterClient::notifyError()
{
	emit message(engineName(), tr("Bad response from master server."), true);
}

void MasterClient::notifyUpdate()
{
	emit message(engineName(), tr("Could not fetch a new server list.  The protocol you are using is too old.  An update may be available."), true);
}

int MasterClient::numPlayers() const
{
	int players = 0;
	foreach(Server* server, servers)
	{
		if (server != NULL)
		{
			players += server->players()->numClients();
		}
	}

	return players;
}

bool MasterClient::preparePacketCache(bool write)
{
	if(cache == NULL || cache->isWritable() != write)
	{
		if(plugin() == NULL)
			return false;

		if(cache == NULL)
		{
			QString cacheFile(Main::dataPaths->programsDataDirectoryPath() + "/" + QString(plugin()->data()->name).replace(' ', ""));
			cache = new QFile(cacheFile);
		}
		else
			cache->close();

		if(!cache->open(write ? QIODevice::WriteOnly|QIODevice::Truncate : QIODevice::ReadOnly))
		{
			resetPacketCaching();
			return false;
		}
	}
	return cache != NULL;
}

void MasterClient::pushPacketToCache(QByteArray &data)
{
	if(!preparePacketCache(true))
		return;

	QDataStream strm(cache);
	strm << static_cast<quint16>(data.size());
	strm << data;
}

bool MasterClient::readMasterResponse(QHostAddress& address, unsigned short port, QByteArray &data)
{
	if (isAddressDataCorrect(address, port))
	{
		pushPacketToCache(data);
		if(readMasterResponse(data))
			return true;
		return false;
	}

	return false;
}

void MasterClient::readPacketCache()
{
	if(!preparePacketCache(false))
	{
		// Cache didn't open? Guess we just emit the signal.
		emit listUpdated();
		return;
	}

	gLog << tr("Reloading master server results from cache for %1!").arg(plugin()->data()->name);
	QDataStream strm(cache);
	while(!strm.atEnd())
	{
		quint16 size;
		strm >> size;

		QByteArray data(size, '\0');
		strm >> data;

		if(!readMasterResponse(data))
		{
			// Cache was not read properly. We need to emit the signal
			// to notify the program that this master client finished
			// updating.
			emit listUpdated();
			break;
		}
	}
}

void MasterClient::resetPacketCaching()
{
	if(cache != NULL)
	{
		delete cache;
		cache = NULL;
	}
}

void MasterClient::refresh()
{
	bTimeouted = false;
	emptyServerList();
	resetPacketCaching();

	if(address.isNull())
		return;

	// Make request
	QByteArray request;
	if(!getServerListRequest(request))
	{
		return;
	}
	pGlobalUdpSocket->writeDatagram(request, address, port);
}

void MasterClient::timeoutRefresh()
{
	// Avoid timeouting more than once. This would cause errors.
	if (!bTimeouted)
	{
		bTimeouted = true;

		emit message(tr("Master server timeout"), tr("Connection timeout (%1:%2).").arg(address.toString()).arg(port), true);
		readPacketCache();

		timeoutRefreshEx();
	}
}

void MasterClient::updateAddress()
{
	QString host;
	unsigned short port;
	plugin()->masterHost(host, port);

	QHostInfo info = QHostInfo::fromName(host);
	if(info.addresses().size() == 0)
		return;

	this->address = info.addresses().first();
	if(this->address.protocol() != QAbstractSocket::IPv4Protocol)
	{
		foreach(const QHostAddress &addr, info.addresses())
		{
			if(addr.protocol() == QAbstractSocket::IPv4Protocol)
				this->address = addr;
		}
	}
	this->port = port;
}

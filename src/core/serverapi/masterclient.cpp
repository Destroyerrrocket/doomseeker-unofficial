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

#include "log.h"
#include "plugins/engineplugin.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "serverapi/playerslist.h"
#include "datapaths.h"

#include <QDataStream>
#include <QErrorMessage>
#include <QFile>
#include <QHostInfo>
#include <QMessageBox>
#include <QUdpSocket>

class MasterClient::PrivData
{
	public:
		QHostAddress address;

		bool timeouted;
		bool enabled;
		unsigned short port;
		QList<ServerPtr> servers;

		QFile *cache;
};

MasterClient::MasterClient()
{
	d = new PrivData();
	d->cache = NULL;
	d->timeouted = false;
	d->enabled = true;
	d->port = 0;
}

MasterClient::~MasterClient()
{
	emptyServerList();
	resetPacketCaching();
	if (d->cache != NULL)
	{
		delete d->cache;
	}
	delete d;
}

void MasterClient::clearServers()
{
	d->servers.clear();
}

bool MasterClient::isAddressSame(const QHostAddress &address, unsigned short port) const
{
	return (d->address == address && d->port == port);
}

void MasterClient::emptyServerList()
{
	foreach (ServerPtr server, d->servers)
	{
		server->disconnect();
	}
	d->servers.clear();
}

QString MasterClient::engineName() const
{
	if (plugin() == NULL)
	{
		return "";
	}
	return plugin()->data()->name;
}

bool MasterClient::isEnabled() const
{
	return d->enabled;
}

bool MasterClient::isTimeouted() const
{
	return d->timeouted;
}

void MasterClient::notifyResponse(Response response)
{
	switch(response)
	{
		default:
			break;
		case RESPONSE_BANNED:
		{
			Message msg = Message(Message::Type::BANNED_FROM_MASTERSERVER);
			emit message(engineName(), msg.contents(), true);
			emit messageImportant(msg);
			break;
		}
		case RESPONSE_WAIT:
			emit message(engineName(), tr("Could not fetch a new server list from the "
				"master because not enough time has past."), true);
			break;
		case RESPONSE_BAD:
			emit message(engineName(), tr("Bad response from master server."), true);
			break;
		case RESPONSE_OLD:
			emit message(engineName(),
				tr("Could not fetch a new server list. The protocol you are using is too old. "
				"An update may be available."), true);
			break;
	}
}

int MasterClient::numPlayers() const
{
	int players = 0;
	foreach(ServerPtr server, d->servers)
	{
		if (server != NULL)
		{
			players += server->players().numClients();
		}
	}
	return players;
}

int MasterClient::numServers() const
{
	return d->servers.size();
}

ServerPtr MasterClient::operator[](int index) const
{
	return d->servers[index];
}

bool MasterClient::preparePacketCache(bool write)
{
	if(d->cache == NULL || d->cache->isWritable() != write)
	{
		if(plugin() == NULL)
		{
			return false;
		}

		if(d->cache == NULL)
		{
			QString cacheFile = gDefaultDataPaths->programsDataDirectoryPath() + "/"
				+ QString(plugin()->data()->name).replace(' ', "");
			d->cache = new QFile(cacheFile);
		}
		else
		{
			d->cache->close();
		}

		if(!d->cache->open(write ? QIODevice::WriteOnly|QIODevice::Truncate : QIODevice::ReadOnly))
		{
			resetPacketCaching();
			return false;
		}
	}
	return d->cache != NULL;
}

void MasterClient::pushPacketToCache(const QByteArray &data)
{
	if(!preparePacketCache(true))
	{
		return;
	}

	QDataStream strm(d->cache);
	strm << static_cast<quint16>(data.size());
	strm << data;
}

MasterClient::Response MasterClient::readResponse(const QByteArray &data)
{
	pushPacketToCache(data);
	return readMasterResponse(data);
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
	QDataStream strm(d->cache);
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

void MasterClient::registerNewServer(ServerPtr server)
{
	server->setSelf(server.toWeakRef());
	d->servers << server;
}

void MasterClient::resetPacketCaching()
{
	if(d->cache != NULL)
	{
		delete d->cache;
		d->cache = NULL;
	}
}

void MasterClient::refreshStarts()
{
	setTimeouted(false);
	emptyServerList();
	resetPacketCaching();
}

bool MasterClient::sendRequest(QUdpSocket *socket)
{
	if(d->address.isNull())
		return false;

	// Make request
	QByteArray request = createServerListRequest();
	if(request.isEmpty())
		return false;
	socket->writeDatagram(request, d->address, d->port);
	return true;
}

const QList<ServerPtr> &MasterClient::servers() const
{
	return d->servers;
}

void MasterClient::setEnabled(bool b)
{
	d->enabled = b;
}

void MasterClient::setTimeouted(bool b)
{
	d->timeouted = b;
}

void MasterClient::timeoutRefresh()
{
	// Avoid timeouting more than once. This would cause errors.
	if (!isTimeouted())
	{
		setTimeouted(true);

		emit message(tr("Master server timeout"), tr("Connection timeout (%1:%2).")
			.arg(d->address.toString()).arg(d->port), true);
		readPacketCache();

		timeoutRefreshEx();
	}
}

void MasterClient::timeoutRefreshEx()
{
}

void MasterClient::updateAddress()
{
	QString host;
	unsigned short port;
	plugin()->masterHost(host, port);

	QHostInfo info = QHostInfo::fromName(host);
	if(info.addresses().size() == 0)
		return;

	d->address = info.addresses().first();
	if (d->address.protocol() != QAbstractSocket::IPv4Protocol)
	{
		foreach(const QHostAddress &addr, info.addresses())
		{
			if(addr.protocol() == QAbstractSocket::IPv4Protocol)
				d->address = addr;
		}
	}
	d->port = port;
}

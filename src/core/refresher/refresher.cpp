//------------------------------------------------------------------------------
// refresher.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "refresher.h"

#include "configuration/queryspeed.h"
#include "refresher/canrefreshserver.h"
#include "serverapi/masterclient.h"
#include "serverapi/mastermanager.h"
#include "plugins/pluginloader.h"
#include "serverapi/server.h"
#include "configuration/doomseekerconfig.h"

#include <QHash>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QPointer>
#include <QTimer>
#include <QThread>
#include <QRunnable>
#include <QSet>
#include <QUdpSocket>

class ServerRefreshTime
{
	public:
		QPointer<Server> server;
		QTime time;

		ServerRefreshTime(QPointer<Server> server)
		{
			this->server = server;
			time.start();
		}

		bool operator==(const ServerRefreshTime& other)
		{
			return server == other.server;
		}
};

class Refresher::Data
{
	public:
		typedef QHash<MasterClient*, MasterClientInfo*> MasterHashtable;
		typedef QHash<MasterClient*, MasterClientInfo*>::iterator MasterHashtableIt;

		QTime batchTime;
		bool bSleeping;
		bool bKeepRunning;
		int delayBetweenResends;
		MasterHashtable registeredMasters;

		QList<QPointer<Server> > unchallengedServers;
		QList<ServerRefreshTime> refreshingServers;
		QUdpSocket* socket;
		QSet<MasterClient*> unchallengedMasters;

		bool hasAnyServers() const
		{
			return !unchallengedServers.isEmpty() || !refreshingServers.isEmpty();
		}

		bool isServerRegistered(Server *server) const
		{
			return unchallengedServers.contains(server) ||
				refreshingServers.contains(ServerRefreshTime(server));
		}

		QPointer<Server> popNextUnchallengedServer()
		{
			while (!unchallengedServers.isEmpty())
			{
				QPointer<Server> server = unchallengedServers.takeFirst();
				if (!server.isNull())
				{
					return server;
				}
			}
			return NULL;
		}
};

////////////////////////////////////////////////////////////////////////////////

class Refresher::MasterClientInfo
{
	public:
		MasterClientInfo(Refresher* parent)
		{
			connect(&lastChallengeTimer, SIGNAL(timeout()),
				parent, SLOT(attemptTimeoutMasters()));
			lastChallengeTimer.setSingleShot(true);
			lastChallengeTimer.setInterval(MASTER_SERVER_TIMEOUT_DELAY);
		}

		void fireLastChallengeSentTimer()
		{
			lastChallengeTimer.start();
		}

		bool isLastChallengeTimerActive() const
		{
			return lastChallengeTimer.isActive();
		}

	private:
		QTimer lastChallengeTimer;
};

////////////////////////////////////////////////////////////////////////////////

Refresher* Refresher::staticInstance = NULL;
QMutex Refresher::instanceMutex;

Refresher::Refresher()
{
	d = new Data;
	d->bSleeping = true;
	d->bKeepRunning = true;
	d->delayBetweenResends = 1000;
	d->socket = NULL;
}

Refresher::~Refresher()
{
	delete d->socket;
	delete d;
}

void Refresher::attemptTimeoutMasters()
{
	QList<MasterClient*> masters = d->registeredMasters.keys();
	foreach (MasterClient* master, masters)
	{
		MasterClientInfo* pMasterInfo = d->registeredMasters[master];
		if (!pMasterInfo->isLastChallengeTimerActive())
		{
			master->timeoutRefresh();
		}
	}
}

Refresher *Refresher::instance()
{
	if (staticInstance == NULL)
	{
		QMutexLocker locker(&instanceMutex);
		if (staticInstance == NULL)
		{
			staticInstance = new Refresher();
		}
	}
	return staticInstance;
}

bool Refresher::isInstantiated()
{
	return staticInstance != NULL;
}

void Refresher::deinstantiate()
{
	QMutexLocker locker(&instanceMutex);
	if (isInstantiated())
	{
		delete staticInstance;
		staticInstance = NULL;
	}
}

Server* Refresher::findRefreshingServer(const QHostAddress& address,
	unsigned short port)
{
	foreach (const ServerRefreshTime& refreshOp, d->refreshingServers)
	{
		if (refreshOp.server.isNull())
		{
			continue;
		}
		if (refreshOp.server->address() == address && refreshOp.server->port() == port)
		{
			return refreshOp.server;
		}
	}
	return NULL;
}

bool Refresher::isAnythingToRefresh() const
{
	return d->hasAnyServers() || !d->registeredMasters.isEmpty() || !d->unchallengedMasters.isEmpty();
}

void Refresher::masterFinishedRefreshing()
{
	MasterClient* pMaster = static_cast<MasterClient*>(sender());
	const QList<ServerPtr>& servers = pMaster->servers();
	foreach (ServerPtr pServer, servers)
	{
		registerServer(pServer.data());
	}

	unregisterMaster(pMaster);

	if (servers.size() == 0 && !isAnythingToRefresh())
	{
		d->bSleeping = true;
		emit sleepingModeEnter();
	}

	emit finishedQueryingMaster(pMaster);
}

void Refresher::purgeNullServers()
{
	d->refreshingServers.removeAll(ServerRefreshTime(NULL));
	d->unchallengedServers.removeAll(NULL);
}

void Refresher::quit()
{
	d->bKeepRunning = false;
}

void Refresher::registerMaster(MasterClient* pMaster)
{
	if (!d->registeredMasters.contains(pMaster))
	{
		MasterClientInfo* pMasterInfo = new MasterClientInfo(this);
		this->connect(pMaster, SIGNAL(listUpdated()), SLOT(masterFinishedRefreshing()));

		d->registeredMasters.insert(pMaster, pMasterInfo);
		d->unchallengedMasters.insert(pMaster);
		emit block();

		if (d->registeredMasters.size() == 1)
		{
			if (d->bSleeping)
			{
				d->bSleeping = false;
				emit sleepingModeExit();
			}
			QTimer::singleShot(20, this, SLOT(sendMasterQueries()));
		}
	}
}

bool Refresher::registerServer(Server* server)
{
	bool hadAnyServers = d->hasAnyServers();
	purgeNullServers();
	if (!d->isServerRegistered(server))
	{
		CanRefreshServer refreshChecker(server);
		if (!refreshChecker.shouldRefresh())
		{
			return false;
		}
		d->unchallengedServers.append(server);
		if (!server->isCustom() && !server->isLan())
		{
			emit block();
		}

		server->refreshStarts();
		if (!hadAnyServers && d->hasAnyServers())
		{
			if (d->bSleeping)
			{
				d->bSleeping = false;
				emit sleepingModeExit();
			}
			QTimer::singleShot(20, this, SLOT(sendServerQueries()));
		}
	}
	return true;
}

void Refresher::readAllPendingDatagrams()
{
	while(d->socket->hasPendingDatagrams() && d->bKeepRunning)
	{
		readPendingDatagram();
	}
}

void Refresher::readPendingDatagram()
{
	QHostAddress address;
	quint16 port;
	qint64 size = d->socket->pendingDatagramSize();
	char* data = new char[size];
	d->socket->readDatagram(data, size, &address, &port);

	QByteArray dataArray(data, size);
	delete[] data;

	if (tryReadDatagramByMasterClient(address, port, dataArray))
	{
		return;
	}
	tryReadDatagramByServer(address, port, dataArray);
}

void Refresher::sendMasterQueries()
{
	while(!d->unchallengedMasters.isEmpty())
	{
		MasterClient* pMaster = *d->unchallengedMasters.begin();

		MasterClientInfo* pMasterInfo = d->registeredMasters[pMaster];
		pMasterInfo->fireLastChallengeSentTimer();

		pMaster->refreshStarts();
		pMaster->sendRequest(d->socket);
		d->unchallengedMasters.remove(pMaster);
	}
}

void Refresher::sendServerQueries()
{
	if (!d->bKeepRunning)
	{
		return;
	}

	purgeNullServers();
	if (d->hasAnyServers())
	{
		startNewServerRefresh();
		resendCurrentServerRefreshesIfTimeout();
		// Call self. This will continue until there's nothing more
		// to refresh. Also make sure that there is at least some
		// delay between calls or Doomseeker will hog CPU.
		QTimer::singleShot(qMax(1, gConfig.doomseeker.querySpeed().intervalBetweenServers),
			this, SLOT(sendServerQueries()));
	}
	else
	{
		if (!isAnythingToRefresh())
		{
			d->bSleeping = true;
			emit sleepingModeEnter();
		}
	}
}

void Refresher::setDelayBetweenResends(int delay)
{
	d->delayBetweenResends = qMax(delay, 100);
}

bool Refresher::start()
{
	QUdpSocket* socket = new QUdpSocket();
	this->connect(socket, SIGNAL(readyRead()),
		SLOT(readAllPendingDatagrams()));
	if (socket->bind())
	{
		d->socket = socket;
		return true;
	}
	else
	{
		delete socket;
	}
	return false;
}

void Refresher::startNewServerRefresh()
{
	// Copy the list as the original will be modified.
	// We don't want to confuse the foreach.
	QPointer<Server> server = d->popNextUnchallengedServer();
	if (!server.isNull())
	{
		if(server->sendRefreshQuery(d->socket))
		{
			d->refreshingServers.append(server);
		}
	}
}

void Refresher::resendCurrentServerRefreshesIfTimeout()
{
	for (int i = 0; i < d->refreshingServers.size(); ++i)
	{
		ServerRefreshTime &refreshOp = d->refreshingServers[i];
		if (refreshOp.time.elapsed() > d->delayBetweenResends)
		{
			if (refreshOp.server->sendRefreshQuery(d->socket))
			{
				refreshOp.time.start();
			}
			else
			{
				d->refreshingServers.removeOne(refreshOp);
				// The collection on which we iterate just got shortened
				// so let's back up by one step.
				--i;
			}
		}
	}
}

bool Refresher::tryReadDatagramByMasterClient(QHostAddress& address,
	unsigned short port, QByteArray& packet)
{
	foreach (MasterClient* pMaster, d->registeredMasters.keys())
	{
		if (!d->bKeepRunning)
		{
			return true;
		}
		if (pMaster->isAddressSame(address, port))
		{
			MasterClient::Response response = pMaster->readResponse(packet);
			switch(response)
			{
				case MasterClient::RESPONSE_BANNED:
				case MasterClient::RESPONSE_WAIT:
				case MasterClient::RESPONSE_BAD:
				case MasterClient::RESPONSE_OLD:
					pMaster->notifyResponse(response);
					unregisterMaster(pMaster);
					return true;
				case MasterClient::RESPONSE_REPLY:
					pMaster->sendRequest(d->socket);
					return true;
				default:
					return true;
			}
		}
	}
	return false;
}

bool Refresher::tryReadDatagramByServer(const QHostAddress& address,
	unsigned short port, QByteArray& packet)
{
	if (!d->bKeepRunning)
	{
		return true;
	}
	Server* server = findRefreshingServer(address, port);
	if (server != NULL)
	{
		// Store the state of request read.
		int response = server->readRefreshQueryResponse(packet);
		switch(response)
		{
			case Server::RESPONSE_REPLY:
				if(server->sendRefreshQuery(d->socket))
				{
					// Reset timer
					ServerRefreshTime refreshOp(server);
					d->refreshingServers.removeAll(refreshOp);
					d->refreshingServers.append(refreshOp);
					break;
				}
				response = Server::RESPONSE_BAD;
				// Intentional fall through
			default:
				d->refreshingServers.removeAll(ServerRefreshTime(server));
				server->refreshStops(static_cast<Server::Response>(response));
				return true;

			case Server::RESPONSE_PENDING:
			{
				// Reset timer
				ServerRefreshTime refreshOp(server);
				d->refreshingServers.removeAll(refreshOp);
				d->refreshingServers.append(refreshOp);
				break;
			}
		}
	}
	return false;
}

void Refresher::unregisterMaster(MasterClient* pMaster)
{
	Data::MasterHashtableIt it = d->registeredMasters.find(pMaster);
	if (it != d->registeredMasters.end())
	{
		MasterClientInfo* pMasterInfo = it.value();
		delete pMasterInfo;
		d->registeredMasters.erase(it);
	}
}

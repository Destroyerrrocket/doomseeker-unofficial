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

#include "serverapi/masterclient.h"
#include "serverapi/masterclientsignalproxy.h"
#include "serverapi/mastermanager.h"
#include "plugins/pluginloader.h"
#include "serverapi/server.h"
#include "configuration/doomseekerconfig.h"

#include <QHash>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>
#include <QThread>
#include <QRunnable>
#include <QSet>
#include <QUdpSocket>

class ServerRefreshTime
{
	public:
		Server* server;
		QTime time;

		ServerRefreshTime(Server* server)
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

		/**
		 * This will keep list of ALL servers to make sure that no server is
		 * registered twice.
		 */
		QSet<Server*> registeredServers;
		QList<ServerRefreshTime> refreshingServers;
		QUdpSocket*	 socket;
		QSet<MasterClient*> unchallengedMasters;
};

////////////////////////////////////////////////////////////////////////////////

class Refresher::MasterClientInfo
{
	public:
		MasterClientInfo(MasterClient* pMaster, Refresher* pParent)
		{
			pParentThread = pParent;
			pLastChallengeTimer = NULL;
			pReceiver = new MasterClientSignalProxy(pMaster);
			connect(pReceiver, SIGNAL( listUpdated(MasterClient*) ), pParent, SLOT( masterFinishedRefreshing(MasterClient*) ) );

			numOfChallengesSent = 0;
		}
		~MasterClientInfo()
		{
			delete pReceiver;
			if (pLastChallengeTimer != NULL)
			{
				delete pLastChallengeTimer;
			}
		}

		void fireLastChallengeSentTimer()
		{
			// This was previously done with an object of QTimer
			// instead of a pointer.
			// Unfortunately on Windows it produced a cross-threading
			// warning messages saying that timer cannot be started
			// from a different thread.
			if (pLastChallengeTimer == NULL)
			{
				pLastChallengeTimer = new QTimer();
				connect(pLastChallengeTimer, SIGNAL(timeout()),
					pParentThread, SLOT(attemptTimeoutMasters()));
				pLastChallengeTimer->setSingleShot(true);
				pLastChallengeTimer->setInterval(MASTER_SERVER_TIMEOUT_DELAY);
			}

			pLastChallengeTimer->start();
		}

		bool isLastChallengeTimerActive() const
		{
			if (pLastChallengeTimer == NULL)
			{
				return false;
			}

			return pLastChallengeTimer->isActive();
		}

		int numOfChallengesSent;

	protected:
		MasterClientSignalProxy* pReceiver;
		Refresher* pParentThread;
		QTimer* pLastChallengeTimer;
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
		if (refreshOp.server->address() == address && refreshOp.server->port() == port)
		{
			return refreshOp.server;
		}
	}
	return NULL;
}

bool Refresher::isAnythingToRefresh() const
{
	return !d->refreshingServers.isEmpty() || !d->registeredServers.isEmpty()
		|| !d->registeredMasters.isEmpty() || !d->unchallengedMasters.isEmpty();
}

bool Refresher::hasFreeServerRefreshSlots() const
{
	return d->refreshingServers.size() < gConfig.doomseeker.queryBatchSize;
}

void Refresher::masterFinishedRefreshing(MasterClient* pMaster)
{
	const QList<Server*>& servers = pMaster->servers();
	foreach (Server* pServer, servers)
	{
		registerServer(pServer);
	}

	unregisterMaster(pMaster);

	if (servers.size() == 0 && !isAnythingToRefresh())
	{
		d->bSleeping = true;
		emit sleepingModeEnter();
	}

	emit finishedQueryingMaster(pMaster);
}

void Refresher::quit()
{
	d->bKeepRunning = false;
}

void Refresher::registerMaster(MasterClient* pMaster)
{
	if (!d->registeredMasters.contains(pMaster))
	{
		MasterClientInfo* pMasterInfo = new MasterClientInfo(pMaster, this);

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
	if (!d->registeredServers.contains(server))
	{
		if (!server->isRefreshable())
		{
			return false;
		}
		d->registeredServers.insert(server);
		if (!server->isCustom())
		{
			emit block();
		}

		server->refreshStarts();
		if (d->registeredServers.size() == 1)
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
		++pMasterInfo->numOfChallengesSent;
		pMasterInfo->fireLastChallengeSentTimer();

		pMaster->refresh();
		d->unchallengedMasters.remove(pMaster);
	}
}

void Refresher::sendServerQueries()
{
	if (!d->bKeepRunning)
	{
		return;
	}

	if (!d->registeredServers.isEmpty())
	{
		startNewServerRefreshesIfFreeSlots();
		resendCurrentServerRefreshesIfTimeout();
		// Call self. This will continue until there's nothing more
		// to refresh. Also make sure that there is at least some
		// delay between calls or Doomseeker will hog CPU.
		QTimer::singleShot(qMax(1U, gConfig.doomseeker.queryBatchDelay),
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

bool Refresher::shouldBlockRefreshingProcess() const
{
	if (!d->registeredMasters.isEmpty())
	{
		return true;
	}

	foreach(const Server* pServer, d->registeredServers)
	{
		if (!pServer->isCustom())
		{
			return true;
		}
	}

	return false;
}

bool Refresher::start()
{
	QUdpSocket* socket = new QUdpSocket();
	this->connect(socket, SIGNAL(readyRead()),
		SLOT(readAllPendingDatagrams()));
	if (socket->bind())
	{
		d->socket = socket;
		MasterClient::pGlobalUdpSocket = socket;
		return true;
	}
	else
	{
		delete socket;
	}
	return false;
}

void Refresher::startNewServerRefreshesIfFreeSlots()
{
	// Copy the list as the original will be modified.
	// We don't want to confuse the foreach.
	QSet<Server*> servers = d->registeredServers;
	foreach (Server* server, servers)
	{
		if (!hasFreeServerRefreshSlots())
		{
			break;
		}
		if (!d->refreshingServers.contains(server))
		{
			if(server->sendRefreshQuery(d->socket))
			{
				d->refreshingServers.append(server);
			}
			else
			{
				d->registeredServers.remove(server);
			}
		}
	}
}

void Refresher::resendCurrentServerRefreshesIfTimeout()
{
	for (int i = 0; i < d->refreshingServers.size(); ++i)
	{
		ServerRefreshTime& refreshOp = d->refreshingServers[i];
		if (refreshOp.time.elapsed() > d->delayBetweenResends)
		{
			if (refreshOp.server->sendRefreshQuery(d->socket))
			{
				refreshOp.time.start();
			}
			else
			{
				d->refreshingServers.removeOne(refreshOp);
				d->registeredServers.remove(refreshOp.server);
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
		if (pMaster->readMasterResponse(address, port, packet))
		{
			return true;
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
		d->refreshingServers.removeOne(server);
		d->registeredServers.remove(server);

		// Store the state of request read.
		int response = server->readRefreshQueryResponse(packet);
		server->refreshStops(static_cast<Server::Response>(response));
		return true;
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

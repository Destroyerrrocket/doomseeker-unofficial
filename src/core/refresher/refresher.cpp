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

#include "masterserver/masterclient.h"
#include "masterserver/masterclientsignalproxy.h"
#include "masterserver/mastermanager.h"
#include "plugins/pluginloader.h"
#include "serverapi/server.h"
#include "configuration/doomseekerconfig.h"

#include <QHash>
#include <QList>
#include <QMutex>
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

class RefreshingThread::Data
{
	public:
		typedef QHash<MasterClient*, MasterClientInfo*> MasterHashtable;
		typedef QHash<MasterClient*, MasterClientInfo*>::iterator MasterHashtableIt;

		Controller *controller;

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

		/**
		 * Mutex used by methods of this class.
		 */
		QMutex thisMutex;
};

////////////////////////////////////////////////////////////////////////////////

/**
 * The actual thread in the refreshing thread.  This must be separate in order
 * to keep the signal/slot system wroking properly.
 */
class RefreshingThread::Controller : public QThread
{
	public:
		Controller()
		{
			worker = new RefreshingThread(this);
		}

		RefreshingThread *refreshingThread() const
		{
			return worker;
		}

		void run()
		{
			// Allocate the new socket when the thread is executed.
			worker->d->socket = new QUdpSocket();
			worker->d->socket->bind();
			MasterClient::pGlobalUdpSocket = worker->d->socket;

			connect(worker->d->socket, SIGNAL(readyRead()), worker, SLOT(readAllPendingDatagrams()));

			exec();

			// Remember to delete the socket when thread finishes.
			delete worker->d->socket;
		}

	private:
		RefreshingThread *worker;
};

////////////////////////////////////////////////////////////////////////////////

class RefreshingThread::MasterClientInfo
{
	public:
		MasterClientInfo(MasterClient* pMaster, RefreshingThread* pParent)
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
		RefreshingThread* pParentThread;
		QTimer* pLastChallengeTimer;
};

////////////////////////////////////////////////////////////////////////////////

RefreshingThread::RefreshingThread(Controller *controller)
{
	d = new Data;
	d->controller = controller;
	moveToThread(controller);

	d->bSleeping = true;
	d->bKeepRunning = true;
	d->delayBetweenResends = 1000;
	d->socket = NULL;
}

RefreshingThread::~RefreshingThread()
{
	// If quit() wasn't called yet, call it now.
	if (d->bKeepRunning)
	{
		d->controller->exit();
	}
	delete d->controller;
}

void RefreshingThread::attemptTimeoutMasters()
{
	Data::MasterHashtableIt it;
	for (it = d->registeredMasters.begin(); it != d->registeredMasters.end(); ++it)
	{
		MasterClientInfo* pMasterInfo = it.value();

		if (!pMasterInfo->isLastChallengeTimerActive())
		{
			MasterClient* pMaster = it.key();
			pMaster->timeoutRefresh();
		}
	}
}

RefreshingThread *RefreshingThread::createRefreshingThread()
{
	return (new Controller())->refreshingThread();
}

Server* RefreshingThread::findRefreshingServer(const QHostAddress& address,
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

bool RefreshingThread::isAnythingToRefresh() const
{
	return !d->refreshingServers.isEmpty() || !d->registeredServers.isEmpty()
		|| !d->registeredMasters.isEmpty() || !d->unchallengedMasters.isEmpty();
}

bool RefreshingThread::isRunning() const
{
	return d->controller->isRunning();
}

bool RefreshingThread::hasFreeServerRefreshSlots() const
{
	return d->refreshingServers.size() < gConfig.doomseeker.queryBatchSize;
}

void RefreshingThread::masterFinishedRefreshing(MasterClient* pMaster)
{
	const QList<Server*>& servers = pMaster->serverList();
	foreach (Server* pServer, servers)
	{
		registerServer(pServer);
	}

	d->thisMutex.lock();
	unregisterMaster(pMaster);

	if (servers.size() == 0 && !isAnythingToRefresh())
	{
		d->bSleeping = true;
		emit sleepingModeEnter();
	}
	d->thisMutex.unlock();

	emit finishedQueryingMaster(pMaster);
}

void RefreshingThread::quit()
{
	d->bKeepRunning = false;

	d->controller->exit();
}

void RefreshingThread::registerMaster(MasterClient* pMaster)
{
	d->thisMutex.lock();

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

	d->thisMutex.unlock();
}

void RefreshingThread::registerServer(Server* server)
{
	d->thisMutex.lock();

	if (!d->registeredServers.contains(server))
	{
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

	d->thisMutex.unlock();
}

void RefreshingThread::readAllPendingDatagrams()
{
	while(d->socket->hasPendingDatagrams() && d->bKeepRunning)
	{
		d->thisMutex.lock();
		readPendingDatagram();
		d->thisMutex.unlock();
	}
}

void RefreshingThread::readPendingDatagram()
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

void RefreshingThread::sendMasterQueries()
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

void RefreshingThread::sendServerQueries()
{
	const unsigned SERVER_BATCH_SIZE = gConfig.doomseeker.queryBatchSize;

	if (!d->bKeepRunning)
	{
		return;
	}

	if (!d->registeredServers.isEmpty())
	{
		d->thisMutex.lock();
		startNewServerRefreshesIfFreeSlots();
		resendCurrentServerRefreshesIfTimeout();
		d->thisMutex.unlock();
		// Call self. This will continue until there's nothing more
		// to refresh.
		QTimer::singleShot(gConfig.doomseeker.queryBatchDelay, this,
			SLOT(sendServerQueries()));
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

void RefreshingThread::setDelayBetweenResends(int delay)
{
	d->delayBetweenResends = qMax(delay, 100);
}

bool RefreshingThread::shouldBlockRefreshingProcess() const
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

void RefreshingThread::start() const
{
	d->controller->start();
}

void RefreshingThread::startNewServerRefreshesIfFreeSlots()
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

void RefreshingThread::resendCurrentServerRefreshesIfTimeout()
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

bool RefreshingThread::tryReadDatagramByMasterClient(QHostAddress& address,
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

bool RefreshingThread::tryReadDatagramByServer(const QHostAddress& address,
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

		server->bPingIsSet = false;

		// Store the state of request read.
		int response = server->readRequest(packet);

		// Set the current ping, if plugin didn't do so already.
		if (!server->bPingIsSet)
		{
			server->currentPing = server->time.elapsed();
		}

		server->refreshStops();

		// Emit the response returned by readRequest.
		server->emitUpdated(response);
		return true;
	}
	return false;
}

void RefreshingThread::unregisterMaster(MasterClient* pMaster)
{
	Data::MasterHashtableIt it = d->registeredMasters.find(pMaster);
	if (it != d->registeredMasters.end())
	{
		MasterClientInfo* pMasterInfo = it.value();
		delete pMasterInfo;
		d->registeredMasters.erase(it);
	}
}

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

class RefreshingThread::Data
{
	public:
		typedef QHash<MasterClient*, MasterClientInfo*>				MasterHashtable;
		typedef QHash<MasterClient*, MasterClientInfo*>::iterator	MasterHashtableIt;

		Controller				*controller;

		QTime					batchTime;
		bool					bSleeping;
		bool					bKeepRunning;
		int						delayBetweenResends;
		QList<ServerBatch>		registeredBatches;
		MasterHashtable			registeredMasters;

		/**
		 *	This will keep list of ALL servers to make sure that no server is
		 *	registered twice.
		 */
		QSet<Server*>			registeredServers;
		QUdpSocket*				socket;
		QList<Server*>			unbatchedServers;
		QSet<MasterClient*>		unchallengedMasters;

		/**
		 *	Mutex used by methods of this class.
		 */
		QMutex					thisMutex;
};

////////////////////////////////////////////////////////////////////////////////

/**
 * The actual thread in the refreshing thread.  This must be separate in order
 * to keep the signal/slot system wroking properly.
 */
class RefreshingThread::Controller : public QThread, public QRunnable
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
			bool bFirstQuery = true;

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
		
		void						fireLastChallengeSentTimer()
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
		
		bool						isLastChallengeTimerActive() const
		{
			if (pLastChallengeTimer == NULL)
			{
				return false;
			}
		
			return pLastChallengeTimer->isActive();
		}

		int							numOfChallengesSent;

	protected:
		MasterClientSignalProxy*	pReceiver;
		RefreshingThread*			pParentThread;
		QTimer*						pLastChallengeTimer;
};

////////////////////////////////////////////////////////////////////////////////

class RefreshingThread::ServerBatch
{
	public:
		QList<Server*>	servers;
		QTime			time;

		/**
		 * @param rejectedServers - servers that were removed from this
		 *		batch due to timeout. These should be removed from registered
		 *		servers list in the RefreshingThread.
		 */
		void sendQueries(QUdpSocket *socket, QList<Server*>& rejectedServers, int resendDelay=1000, bool firstQuery=false)
		{
			rejectedServers.clear();
			//printf("Batch size: %u, Delay: %d\n", servers.size(), resendDelay);
			if(firstQuery || resendDelay - time.elapsed() <= 0)
			{
				//printf("SENT!\n");
				time.start();

				// sendRefreshQuery will clean up after a fail
				// There's no need to call methods like
				// Server::refreshStops() explicitly.
				foreach(Server *server, servers)
				{
					if(!server->sendRefreshQuery(socket))
					{
						servers.removeOne(server);
						rejectedServers.append(server);
					}
				}
			}
		}
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

bool RefreshingThread::isAnythingToRefresh() const
{
	int value = d->unbatchedServers.count() | d->registeredBatches.count() | d->registeredMasters.count() | d->unchallengedMasters.count();

	return value != 0;
}

bool RefreshingThread::isRunning() const
{
	return d->controller->isRunning();
}

void RefreshingThread::masterFinishedRefreshing(MasterClient* pMaster)
{
	const QList<Server*>& servers = pMaster->serverList();
	foreach(Server* pServer, servers)
	{
		registerServer(pServer);
	}

	d->thisMutex.lock();
	unregisterMaster(pMaster);

	if(servers.size() == 0 && !isAnythingToRefresh())
	{
		d->bSleeping = true;
		emit sleepingModeEnter();
	}
	d->thisMutex.unlock();

	emit finishedQueryingMaster(pMaster);
}

Server*	RefreshingThread::obtainServerFromBatch(ServerBatch& batch, const QHostAddress& address, quint16 port)
{
	for(int j = 0; j < batch.servers.size(); ++j)
	{
		Server *server = batch.servers[j];
		if(server->port() == port && server->address() == address)
		{
			return server;
		}
	}

	return NULL;
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

		if(d->registeredMasters.size() == 1)
		{
			if(!d->bSleeping)
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
		d->unbatchedServers.append(server);

		if (!server->isCustom())
		{
			emit block();
		}

		server->refreshStarts();

		if(d->registeredServers.size() == 1)
		{
			if(!d->bSleeping)
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
	tryReadDatagramByServerBatch(address, port, dataArray);
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

unsigned RefreshingThread::sendQueriesForBatch(ServerBatch& batch, int resetDelay, bool firstQuery)
{
	QList<Server*> rejectedServers;
	batch.sendQueries(d->socket, rejectedServers, d->delayBetweenResends, false);

	// Now erase all rejected servers from the list of registered
	// servers.
	foreach(Server* server, rejectedServers)
	{
		d->registeredServers.remove(server);
	}

	return batch.servers.size();
}

void RefreshingThread::sendServerQueries()
{
	const unsigned SERVER_BATCH_SIZE = gConfig.doomseeker.queryBatchSize;

	while (d->bKeepRunning && (d->unbatchedServers.size() != 0 || d->registeredBatches.size() != 0))
	{
		//qDebug() << d->unbatchedServers.size() << " unbatched servers.";
		unsigned int querySlotsInUse = 0;
		for(int i = 0; i < d->registeredBatches.size(); ++i)
		{
			//printf("Sending queries from batch: %u\n", i);
			if(d->registeredBatches[i].servers.size() == 0)
			{
				d->registeredBatches.removeAt(i--);
				//printf("Empty batch %u removed, now batches size is: %u\n", i + 1, d->registeredBatches.size());
				if(!isAnythingToRefresh())
				{
					d->bSleeping = true;
					emit sleepingModeEnter();
				}
				continue;
			}
			querySlotsInUse += sendQueriesForBatch(d->registeredBatches[i], d->delayBetweenResends, false);
		}

		//qDebug() << querySlotsInUse << " Servers queried.";
		if(d->unbatchedServers.size() != 0 && querySlotsInUse < SERVER_BATCH_SIZE &&
			d->batchTime.elapsed() >= gConfig.doomseeker.queryBatchDelay)
		{
			d->batchTime.start();
			d->thisMutex.lock();
			ServerBatch batch;
			// Select a batch of servers to query.
			batch.servers = d->unbatchedServers.mid(0, SERVER_BATCH_SIZE-querySlotsInUse);
			//qDebug() << "Batching " << batch.servers.size() << " servers.";
			sendQueriesForBatch(batch, d->delayBetweenResends, true);

			d->registeredBatches.append(batch);
			foreach(Server *server, batch.servers)
			{
				d->unbatchedServers.removeOne(server);
			}
			d->thisMutex.unlock();
		}
		else
			d->socket->waitForReadyRead(100); // This could probably be more properly determined by searching for the nearest timeout in the batches, but this seems to work fairly well.
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

bool RefreshingThread::tryReadDatagramByServerBatch(const QHostAddress& address,
	unsigned short port, QByteArray& packet)
{
	for (int i = 0; i < d->registeredBatches.size(); ++i)
	{
		ServerBatch& serverBatch = d->registeredBatches[i];
		Server *server = obtainServerFromBatch(serverBatch, address, port);
		if (!d->bKeepRunning)
		{
			return true;
		}

		if (server != NULL)
		{
			serverBatch.servers.removeOne(server);
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

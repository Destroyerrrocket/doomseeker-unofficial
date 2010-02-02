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

RefreshingThread::RefreshingThread()
{
	bKeepRunning = true;
	delayBetweenResends = 1000;
	socket = NULL;
}

RefreshingThread::~RefreshingThread()
{
	// If quit() wasn't called already, call it now.
	if (bKeepRunning)
	{
		quit();
	}
}

void RefreshingThread::gotoSleep()
{
	//printf("Going to sleep!\n");
	// Signal that the work is finished
	emit sleepingModeEnter();

	// Release the mutex and wait until woken up.
	thisWaitCondition.wait(&thisMutex);

	// Signal that the work has begun.
	emit sleepingModeExit();

	// Since thread wakes up immediatelly after a single server is
	// registered, assume that more servers will be registered soon
	// and give the main thread some time to do that.
	thisMutex.unlock();
	msleep(20);
	thisMutex.lock();
}

Server*	RefreshingThread::obtainServerFromBatch(ServerBatch& batch, const QHostAddress& address, qint16 port)
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
	bKeepRunning = false;
	thisWaitCondition.wakeAll();
}

void RefreshingThread::registerMaster(MasterClient* master)
{
	thisMutex.lock();

	if (!registeredMasters.contains(master))
	{
		registeredMasters.insert(master);
		emit block();
		thisWaitCondition.wakeAll();
	}

	thisMutex.unlock();
}

void RefreshingThread::registerServer(Server* server)
{
	thisMutex.lock();

	if (!registeredServers.contains(server))
	{
		registeredServers.insert(server);
		unbatchedServers.append(server);

		if (!server->isCustom())
		{
			emit block();
		}

		thisWaitCondition.wakeAll();
		server->refreshStarts();
	}

	thisMutex.unlock();
}

void RefreshingThread::run()
{
	#define COUNTS (unbatchedServers.count() | registeredBatches.count() | registeredMasters.count())

	QTime time;
	time.start();

	// Allocate the new socket when the thread is executed.
	socket = new QUdpSocket();
	socket->bind();
	bool bFirstQuery = true;

	// Each time this will be increased by SERVER_BATCH_SIZE
	while (bKeepRunning)
	{
		thisMutex.lock();

		// Here thread goes into dormant mode if there is nothing to process.
		//printf("Iteration, u: %u, r: %u, masters: %u\n", unbatchedServers.count(), registeredBatches.count(), registeredMasters.count());
		while(COUNTS == 0 && bKeepRunning)
		{
			gotoSleep();

			// Reset variable when thread goes back to work. We assume
			// this happens when new refresh procedure is being issued.
			// Sending of first query shouldn't be delayed.
			bFirstQuery = true;
		}

		// Kill the thread if RefreshingThread::quit() was called.
		if (!bKeepRunning)
			break;

		// Refresh registered masters.
		sendMasterQueries();
		thisMutex.unlock();
		// Read any received data.
		while(socket->hasPendingDatagrams())
		{
			readPendingDatagrams();
		}


		// Now send the server queries.
		sendServerQueries();
	} // end of main thread loop

	// Remember to delete the socket when thread finishes.
	delete socket;
}

void RefreshingThread::readPendingDatagrams()
{
	QHostAddress address;
	quint16 port;
	qint64 size = socket->pendingDatagramSize();
	char* data = new char[size];
	socket->readDatagram(data, size, &address, &port);

	if (registeredBatches.size() != 0)
	{
		thisMutex.lock();
		for(int i = 0; i < registeredBatches.size(); ++i)
		{
			Server *server = obtainServerFromBatch(registeredBatches[i], address, port);
			if (server != NULL)
			{
				registeredBatches[i].servers.removeOne(server);
				registeredServers.remove(server);
				QByteArray dataArray(data, size);
				server->bPingIsSet = false;

				// Store the state of request read.
				int response = server->readRequest(dataArray);

				// Set the current ping, if plugin didn't do so already.
				if (!server->bPingIsSet)
				{
					server->currentPing = server->time.elapsed();
				}

				server->refreshStops();

				// Emit the response returned by readRequest.
				server->emitUpdated(response);
				break; // exit for loop
			}
		}
		thisMutex.unlock();
	}
	delete[] data;
}

void RefreshingThread::sendMasterQueries()
{
	// Currently master servers are refreshed
	// synchronously which blocks this thread. This shouldn't be
	// a big problem unless master is not responding. Proper master
	// refreshing algorithm will be implemented later, if needed.
	foreach(MasterClient* master, registeredMasters)
	{
		master->refresh();
		registeredMasters.remove(master);
		emit finishedQueryingMaster(master);
	}
}

unsigned RefreshingThread::sendQueriesForBatch(ServerBatch& batch, int resetDelay, bool firstQuery)
{
	QList<Server*> rejectedServers;
	batch.sendQueries(socket, rejectedServers, delayBetweenResends, false);

	// Now erase all rejected servers from the list of registered
	// servers.
	foreach(Server* server, rejectedServers)
	{
		registeredServers.remove(server);
	}

	return batch.servers.size();
}

void RefreshingThread::sendServerQueries()
{
	const unsigned SERVER_BATCH_SIZE = 30;

	if (unbatchedServers.size() != 0 || registeredBatches.size() != 0)
	{
		//qDebug() << unbatchedServers.size() << " unbatched servers.";
		unsigned int querySlotsInUse = 0;
		for(int i = 0; i < registeredBatches.size(); ++i)
		{
			//printf("Sending queries from batch: %u\n", i);
			if(registeredBatches[i].servers.size() == 0)
			{
				registeredBatches.removeAt(i--);
				//printf("Empty batch %u removed, now batches size is: %u\n", i + 1, registeredBatches.size());
				continue;
			}
			querySlotsInUse += sendQueriesForBatch(registeredBatches[i], delayBetweenResends, false);
		}

		//qDebug() << querySlotsInUse << " Servers queried.";
		if(unbatchedServers.size() != 0 && querySlotsInUse < SERVER_BATCH_SIZE)
		{
			thisMutex.lock();
			ServerBatch batch;
			// Select a batch of servers to query.
			batch.servers = unbatchedServers.mid(0, SERVER_BATCH_SIZE-querySlotsInUse);
			//qDebug() << "Batching " << batch.servers.size() << " servers.";
			sendQueriesForBatch(batch, delayBetweenResends, true);

			registeredBatches.append(batch);
			foreach(Server *server, batch.servers)
			{
				unbatchedServers.removeOne(server);
			}
			thisMutex.unlock();
		}
		else
			socket->waitForReadyRead(100); // This could probably be more properly determined by searching for the nearest timeout in the batches, but this seems to work fairly well.
	}
}

////////////////////////////////////////////////////////////////////////////////

void ServerBatch::sendQueries(QUdpSocket *socket, QList<Server*>& rejectedServers, int resendDelay, bool firstQuery)
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

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

	if (!unbatchedServers.contains(server))
	{
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
	const unsigned SERVER_BATCH_SIZE = 30;

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
		while( unbatchedServers.count() == 0 && registeredBatches.count() == 0 && registeredMasters.count() == 0 && bKeepRunning)
		{
			// Signal that the work is finished
			emit sleepingModeEnter();

			// Release the mutex and wait until woken up.
			thisWaitCondition.wait(&thisMutex);

			// Reset variable when thread goes back to work. We assume
			// this happens when new refresh procedure is being issued.
			// Sending of first query shouldn't be delayed.
			bFirstQuery = true;

			// Signal that the work has begun.
			emit sleepingModeExit();

			// Since thread wakes up immediatelly after a single server is
			// registered, assume that more servers will be registered soon
			// and give the main thread some time to do that.
			thisMutex.unlock();
			msleep(20);
			thisMutex.lock();
		}

		// Kill the thread if RefreshingThread::quit() was called.
		if (!bKeepRunning)
			break;

		// Refresh registered masters. Currently master servers are refreshed
		// synchronously which blocks this thread. This shouldn't be
		// a big problem unless master is not responding. Proper master
		// refreshing algorithm will be implemented later, if needed.
		foreach(MasterClient* master, registeredMasters)
		{
			master->refresh();
			registeredMasters.remove(master);
			emit finishedQueryingMaster(master);
		}
		thisMutex.unlock();

		// Read any received data.
		while(socket->hasPendingDatagrams())
		{
			QHostAddress address;
			quint16 port;
			qint64 size = socket->pendingDatagramSize();
			char* data = new char[size];
			socket->readDatagram(data, size, &address, &port);

			if (registeredBatches.size() != 0)
			{
				thisMutex.lock();
				for(unsigned int i = 0;i < registeredBatches.size();i++)
				{
					for(unsigned int j = 0;j < registeredBatches[i].servers.size();j++)
					{
						Server *server = registeredBatches[i].servers[j];
						if(server->port() == port && server->address() == address)
						{
							registeredBatches[i].servers.removeAt(j);
							j--;
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
							continue;
						}
					}
				}
				thisMutex.unlock();
			}
			delete[] data;
		}

		// Now send the server queries.
		if (unbatchedServers.size() != 0 || registeredBatches.size() != 0)
		{
			//qDebug() << unbatchedServers.size() << " unbatched servers.";
			unsigned int querySlotsInUse = 0;
			for(unsigned int i = 0;i < registeredBatches.size();i++)
			{
				if(registeredBatches[i].servers.size() == 0)
				{
					registeredBatches.removeAt(i);
					continue;
				}
				registeredBatches[i].sendQueries(socket, delayBetweenResends, false);
				querySlotsInUse += registeredBatches[i].servers.size();
			}

			//qDebug() << querySlotsInUse << " Servers queried.";
			if(unbatchedServers.size() != 0 && querySlotsInUse < SERVER_BATCH_SIZE)
			{
				thisMutex.lock();
				Batch batch;
				// Select a batch of servers to query.
				batch.servers = unbatchedServers.mid(0, SERVER_BATCH_SIZE-querySlotsInUse);
				//qDebug() << "Batching " << batch.servers.size() << " servers.";
				batch.sendQueries(socket, delayBetweenResends, true);

				registeredBatches.append(batch);
				foreach(Server *server, batch.servers)
					unbatchedServers.removeOne(server);
				thisMutex.unlock();
			}
			else
				socket->waitForReadyRead(100); // This could probably be more properly determined by searching for the nearest timeout in the batches, but this seems to work fairly well.
		}
	} // end of main thread loop

	// Remember to delete the socket when thread finishes.
	delete socket;
}

void RefreshingThread::Batch::sendQueries(QUdpSocket *socket, int resendDelay, bool firstQuery)
{
	if(firstQuery || resendDelay - time.elapsed() <= 0)
	{
		time.start();

		// sendRefreshQuery will clean up after a fail
		// There's no need to call methods like
		// Server::refreshStops() explicitly.
		foreach(Server *server, servers)
		{
			if(!server->sendRefreshQuery(socket))
			{
				servers.removeOne(server);
			}
		}
	}
}

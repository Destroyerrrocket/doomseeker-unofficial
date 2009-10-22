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

	if (!registeredServers.contains(server))
	{
		registeredServers.insert(server);

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
	QTime time;
	time.start();

	// Allocate the new socket when the thread is executed.
	socket = new QUdpSocket();
	socket->bind();
	bool bFirstQuery = true;

	while (bKeepRunning)
	{
		thisMutex.lock();

		// Here thread goes into dormant mode if there is nothing to process.
		while( registeredServers.count() == 0 && registeredMasters.count() == 0 && bKeepRunning)
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

			if (registeredServers.size() != 0)
			{
				thisMutex.lock();
				foreach(Server *server, registeredServers)
				{
					if(server->port() == port && server->address() == address)
					{
						registeredServers.remove(server);
						QByteArray dataArray(data, size);
						server->bPingIsSet = false;

						// Store the state of request read.
						bool bIsGood = server->readRequest(dataArray);

						// Set the current ping, if plugin didn't do so already.
						if (!server->bPingIsSet)
						{
							server->currentPing = server->time.elapsed();
						}

						server->refreshStops();

						// If readRequest() returned true, emit the
						// RESPONSE_GOOD signal. Otherwise do nothing, the
						// plugin should decide which response to emit.
						if (bIsGood)
						{
							server->emitUpdated(Server::RESPONSE_GOOD);
						}
						continue;
					}
				}
				thisMutex.unlock();
			}
			delete[] data;
		}

		// Now send the server queries.
		if (registeredServers.size() != 0)
		{
			int timeout = delayBetweenResends - time.elapsed();

			// If we're not currently waiting for a server's data, we'll
			// slow down our timeout.
			if(timeout < 1)
				timeout = 1;

			bool bSendQueries = bFirstQuery;
			if (!bFirstQuery)
			{
				// If there are some datagrams waiting to be read no
				// packets will be sent in the current run.
				bSendQueries = !socket->waitForReadyRead(timeout);
			}
			else
			{
				bFirstQuery = false;
			}

			if(bSendQueries && bKeepRunning)
			{
				time.start();

				thisMutex.lock();
				foreach(Server *server, registeredServers)
				{
					// sendRefreshQuery will clean up after a fail
					// There's no need to call methods like
					// Server::refreshStops() explicitly.
					if(!server->sendRefreshQuery(socket))
					{
						registeredServers.remove(server);
					}
					else
					{
						// Prevent from sending too many requests at once.
						msleep(2);
					}
				}
				thisMutex.unlock();
			}
		}
	} // end of main thread loop

	// Remember to delete the socket when thread finishes.
	delete socket;
}

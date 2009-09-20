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
		thisWaitCondition.wakeAll();
		server->refreshStarts();
	}

	thisMutex.unlock();
}

void RefreshingThread::run()
{
	// Allocate the new socket when the thread is executed.
	socket = new QUdpSocket();

	while (bKeepRunning)
	{
		thisMutex.lock();
		while( registeredServers.count() == 0 && registeredMasters.count() == 0 && bKeepRunning)
		{
			thisWaitCondition.wait(&thisMutex);
		}

		if (!bKeepRunning)
			break;

		thisMutex.unlock();
	}

	// Remember to delete the socket when thread finishes.
	delete socket;
}

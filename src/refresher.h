//------------------------------------------------------------------------------
// refresher.h
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

#ifndef __REFRESHER_H_
#define __REFRESHER_H_

#include <QMutex>
#include <QThread>
#include <QRunnable>
#include <QSet>
#include <QWaitCondition>

#include "masterclient.h"
#include "server.h"

class RefreshingThread : public QThread, public QRunnable
{
	Q_OBJECT

	public:
		RefreshingThread();
		~RefreshingThread();

		/**
		 *	This will set bKeepRunning to false which will tell the refreshing
		 *	thread to exit gracefully.
		 */
		void	quit();

		/**
		 *	Registers a new master server to be queried. All masters are stored
		 *	in a hash table, therefore it's impossible to register the same
		 *	object twice.
		 */
		void 	registerMaster(MasterClient* master);

		/**
		 *	Registers a new server to be queried. All servers are stored
		 *	in a hash table, therefore it's impossible to register the same
		 *	object twice.
		 */
		void 	registerServer(Server* server);
		void 	run();

	protected:
		bool					bKeepRunning;
		QSet<MasterClient*>		registeredMasters;
		QSet<Server*>			registeredServers;
		QUdpSocket*				socket;

		/**
		 *	Mutex used by methods of this class.
		 */
		QMutex					thisMutex;

		/**
		 *	Wait condition used by the methods of this class. Used to wake
		 *	up the sleeping thread after the registerServer() is called.
		 */
		QWaitCondition			thisWaitCondition;
};

#endif

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

#include <QHash>
#include <QMutex>
#include <QThread>
#include <QTime>
#include <QRunnable>
#include <QSet>
#include <QUdpSocket>
#include <QWaitCondition>

class MasterClient;
class MasterClientReceiver;
class Server;

class ServerBatch
{
	public:
		QList<Server*>	servers;
		QTime			time;

		/**
		 * @param rejectedServers - servers that were removed from this
		 *		batch due to timeout. These should be removed from registered
		 *		servers list in the RefreshingThread.
		 */
		void			sendQueries(QUdpSocket *socket, QList<Server*>& rejectedServers, int resendDelay=1000, bool firstQuery=false);
};

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
		void 	registerMaster(MasterClient* pMaster);

		/**
		 *	Registers a new server to be queried. All servers are stored
		 *	in a hash table, therefore it's impossible to register the same
		 *	object twice.
		 */
		void 	registerServer(Server* server);
		void 	run();

		/**
		 *	Sets delay between subsequent queries send to the servers.
		 *	Default value is 1000. Minimum value is 100.
		 */
		void	setDelayBetweenResends(int delay) { delayBetweenResends = qMax(delay, 100); }

	signals:
		/**
		 *	Emitted when a master client of non-custom server is registered.
		 */
		void	block();

		void	finishedQueryingMaster(MasterClient* master);

		/**
		 *	Emitted when refreshing thread doesn't have anything more to do and
		 *	goes into sleeping mode.
		 */
		void 	sleepingModeEnter();

		/**
		 *	Emitted when refreshing thread wakes up from sleeping mode and
		 *	begins refreshing work.
		 */
		void	sleepingModeExit();

	protected:
		class MasterClientInfo
		{
			public:
				MasterClientInfo(MasterClient* pMaster, RefreshingThread* pParent);
				~MasterClientInfo();

				int							numOfChallengesSent;
				QTime						timeLastChallengeSent;

			protected:
				MasterClientReceiver*		pReceiver;
		};

		typedef QHash<MasterClient*, MasterClientInfo*>				MasterHashtable;
		typedef QHash<MasterClient*, MasterClientInfo*>::iterator	MasterHashtableIt;

		static const int							MASTER_SERVER_TIMEOUT_DELAY = 10000;

		bool										bKeepRunning;
		int											delayBetweenResends;
		QList<ServerBatch>							registeredBatches;
		MasterHashtable								registeredMasters;

		/**
		 *	This will keep list of ALL servers to make sure that no server is
		 *	registered twice.
		 */
		QSet<Server*>								registeredServers;
		QUdpSocket*									socket;
		QList<Server*>								unbatchedServers;
		QSet<MasterClient*>							unchallengedMasters;

		/**
		 *	Mutex used by methods of this class.
		 */
		QMutex					thisMutex;

		/**
		 *	Wait condition used by the methods of this class. Used to wake
		 *	up the sleeping thread after the registerServer() is called.
		 */
		QWaitCondition			thisWaitCondition;

		void					attemptTimeoutMasters();

		void					gotoSleep();

		bool					isAnythingToRefresh() const;

		/**
		 *	@return NULL if server of given address:port is not in the batch.
		 */
		Server*					obtainServerFromBatch(ServerBatch& batch, const QHostAddress& address, quint16 port);

		void					readPendingDatagrams();

		void					sendMasterQueries();

		/**
		 *	@return Query slots used by this batch.
		 */
		unsigned				sendQueriesForBatch(ServerBatch& batch, int resetDelay, bool firstQuery);

		void					sendServerQueries();

		/**
		 *	@brief Returns true if there are any master clients or non-custom
		 *	servers registered.
		 */
		bool					shouldBlockRefreshingProcess() const;

		void					unregisterMaster(MasterClient* pMaster);

	protected slots:
		void					masterFinishedRefreshing(MasterClient* pMaster);
};

#endif

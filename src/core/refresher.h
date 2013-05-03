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

#include <QObject>

class MasterClient;
class Server;
class QHostAddress;

class RefreshingThread : public QObject
{
	Q_OBJECT

	friend class Server;

	public:
		~RefreshingThread();

		bool	isRunning() const;

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

		/**
		 *	Sets delay between subsequent queries send to the servers.
		 *	Default value is 1000. Minimum value is 100.
		 */
		void	setDelayBetweenResends(int delay);

		void	start() const;

		static RefreshingThread *createRefreshingThread();
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
		class Controller;
		class Data;
		class MasterClientInfo;
		class ServerBatch;

		RefreshingThread(Controller *controller);

		bool					isAnythingToRefresh() const;

		/**
		 *	@return NULL if server of given address:port is not in the batch.
		 */
		Server*					obtainServerFromBatch(ServerBatch& batch, const QHostAddress& address, quint16 port);

		void					readPendingDatagram();

		/**
		 *	@return Query slots used by this batch.
		 */
		unsigned				sendQueriesForBatch(ServerBatch& batch, int resetDelay, bool firstQuery);

		/**
		 *	@brief Returns true if there are any master clients or non-custom
		 *	servers registered.
		 */
		bool					shouldBlockRefreshingProcess() const;

		void					unregisterMaster(MasterClient* pMaster);

	protected slots:
		void					attemptTimeoutMasters();

		void					masterFinishedRefreshing(MasterClient* pMaster);

		void					readAllPendingDatagrams();

		void					sendMasterQueries();

		void					sendServerQueries();

	private:
		static const int	MASTER_SERVER_TIMEOUT_DELAY = 10000;

		Data *d;

		// TODO: Constify 'address' and 'packet' args.
		/**
		 * @return true if any further attempts to parse the packet
		 *         should be stopped.
		 */
		bool tryReadDatagramByMasterClient(QHostAddress& address,
			unsigned short port, QByteArray& packet);
		/**
		 * @return true if any further attempts to parse the packet
		 *         should be stopped.
		 */
		bool tryReadDatagramByServerBatch(const QHostAddress& address,
			unsigned short port, QByteArray& packet);
};

#endif

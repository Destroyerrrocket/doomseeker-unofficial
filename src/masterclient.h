//------------------------------------------------------------------------------
// masterclient.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __MASTERSERVER_H__
#define __MASTERSERVER_H__

#include <QObject>
#include <QHostAddress>
#include <QList>

#include "server.h"

/**
 * Abstract class base for all MasterClients.  This is expected to fetch a list
 * of IP addresses which will be turned into Servers.
 */
class MAIN_EXPORT MasterClient : public QObject
{
	Q_OBJECT

	public:
		MasterClient(QHostAddress address, unsigned short port);
		virtual ~MasterClient();

		/**
		 *	Serves as an informative role for MasterManager.
		 *	If the master client is disabled, master manager will omit
		 *	it during the refresh.
		 */
		bool					isEnabled() const { return enabled; }
		int						numPlayers() const;
		int						numServers() const { return servers.size(); }
		Server					*operator[] (int index) const { return servers[index]; }
		QList<Server*>			&serverList() { return servers; }
		const QList<Server*>	&serverList() const { return servers; }

		bool					hasServer(const Server*);

	public slots:
		/**
		 * Requests an updated server list from the master, this should emit
		 * listUpdated if the list has changed.
		 *
		 * This function is virtual since MasterManager overrides it.
		 */
		virtual void	refresh();

		/**
		 *	@see isEnabled()
		 */
		void					setEnabled(bool b) { enabled = b; }

	signals:
		void			listUpdated();

		/**
		 *	Plugins may use this to make Doomseeker display custom messages.
		 *	Errors are always displayed in the message box while non-errors
		 *	are currently ignored.
		 */
		void			message(const QString& title, const QString& content, bool isError);

	protected:
		/**
		 * Clears the server list.
		 */
		void			emptyServerList();
		/**
		 * Informs the user that they have been banned from the master server.
		 */
		void			notifyBanned(const QString& engineName);
		/**
		 * Tells the user that the master server will not respond to their
		 * query becuase they tried to refresh too quickly.
		 */
		void			notifyDelay(const QString& engineName);
		/**
		 * Tells the user they need to update since the protocol is too old.
		 */
		void			notifyUpdate(const QString& engineName);

		virtual bool	readRequest(QByteArray &data, bool &expectingMorePackets)=0;
		virtual bool	sendRequest(QByteArray &data)=0;

		QList<Server *>	servers;

		QHostAddress	address;
		bool			enabled;
		unsigned short	port;
};

class CustomServers;

class MasterManager : public MasterClient
{
	Q_OBJECT

	public:
		MasterManager();
		~MasterManager();

		void			addMaster(MasterClient *master);
		CustomServers*	customServs() { return customServers; }


	public slots:
		void	refresh();

	protected:
		bool	readRequest(QByteArray &data, bool &expectingMorePackets) { return true; }
		bool	sendRequest(QByteArray &data) { return true; }

		QList<MasterClient *>	masters;
		CustomServers*			customServers;

	protected slots:
		/**
		 *	message() signal of every master is connected to this slot.
		 *	Doomseeker uses message() signal of MasterManager for convenience
		 */
		void	readMasterMessage(const QString& title, const QString& content, bool isError)
		{
			emit message(title, content, isError);
		}
};

#endif /* __MASTERSERVER_H__ */

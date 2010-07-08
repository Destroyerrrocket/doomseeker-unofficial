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

#include "serverapi/server.h"

struct PluginInfo;

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

		/**
		 *	This is supposed to return the plugin this MasterClient belongs to.
		 *	If it doesn't belong to any plugin then return NULL.
		 *	New instances of PluginInfo shouldn't be created here. Instead
		 *	each plugin should keep a global instance of PluginInfo (singleton?)
		 *	and a pointer to this instance should be returned.
		 */
		virtual const PluginInfo*		plugin() const = 0;

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
		 *	Messages are dumped into the log.
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
class MasterClientMessageReceiver;

class MasterManager : public MasterClient
{
	Q_OBJECT

	public:
		MasterManager();
		~MasterManager();

		void								addMaster(MasterClient *master);
		CustomServers*						customServs() { return customServers; }

		const PluginInfo*					plugin() const { return NULL; }


	public slots:
		void								refresh();
		
	signals:
		void								masterMessage(MasterClient* pSender, const QString& title, const QString& content, bool isError);

	protected:
		bool								readRequest(QByteArray &data, bool &expectingMorePackets) { return true; }
		bool								sendRequest(QByteArray &data) { return true; }

		CustomServers*						customServers;
		QList<MasterClient *>				masters;		
		QList<MasterClientMessageReceiver*>	mastersMessageReceivers;	
	
	protected slots:
		void								readMasterMessage(MasterClient* pSender, const QString& title, const QString& content, bool isError)
		{
			emit masterMessage(pSender, title, content, isError);
		}
};

/**
 *	@brief Designed to preserve the information about the MasterClient
 *	instance that is sending the message.
 */
class MasterClientMessageReceiver : public QObject
{
	Q_OBJECT

	public:
		MasterClient*	pMaster;
		
		MasterClientMessageReceiver(MasterClient* pMaster)
		{
			this->pMaster = pMaster;
			
			connect(pMaster, SIGNAL( message(const QString&, const QString&, bool) ), this, SLOT( readMasterMessage(const QString&, const QString&, bool) ) );
		}
		
	protected slots:
		void			readMasterMessage(const QString& title, const QString& content, bool isError)
		{
			emit message(pMaster, title, content, isError);
		}
	
	signals:
		void			message(MasterClient* pSender, const QString& title, const QString& content, bool isError);
};	

#endif /* __MASTERSERVER_H__ */

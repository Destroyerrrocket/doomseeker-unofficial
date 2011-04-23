//------------------------------------------------------------------------------
// mastermanager.h
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
#ifndef __MASTERMANAGER_H__
#define __MASTERMANAGER_H__

#include "masterclient.h"
#include "serverapi/message.h"

class CustomServers;
class MasterClientReceiver;

/**
 *	@brief Manager class for a number of MasterClient instances.
 *
 *	MasterManager will emit listUpdated() signal once the last MasterClient
 *	that is on the mastersBeingRefreshed list emits its own listUpdate() signal.
 */
class MasterManager : public MasterClient
{
	Q_OBJECT

	public:
		MasterManager();
		~MasterManager();

		void								addMaster(MasterClient *master);
		void								clearServersList() { servers.clear(); }
		CustomServers*						customServs() { return customServers; }

		int									numMasters() const { return masters.size(); }

		const PluginInfo*					plugin() const { return NULL; }

		MasterClient*						operator[](int index) { return masters[index]; }

		bool								readMasterResponse(QByteArray &data) { return false; }

		/**
		 *	@brief Redirects the response to appropriate MasterClient.
		 */
		bool								readMasterResponse(QHostAddress& address, unsigned short port, QByteArray &data);


	public slots:
		void								refresh();

	signals:
		/**
		 *	@brief Emitted for every MasterClient that emits listUpdated()
		 *	signal.
		 */
		void								listUpdatedForMaster(MasterClient* pSender);
		void								masterMessage(MasterClient* pSender, const QString& title, const QString& content, bool isError);
		void								masterMessageImportant(MasterClient* pSender, const Message& objMessage);
		void								newServerBatchReceivedFromMaster(MasterClient* pSender, const QList<Server* >& servers);

	protected:
		CustomServers*						customServers;
		QList<MasterClient *>				masters;
		QSet<MasterClient *>				mastersBeingRefreshed;
		QList<MasterClientReceiver*>		mastersReceivers;

		bool								getServerListRequest(QByteArray &data) { return false; }
		void								timeoutRefreshEx();

	protected slots:
		void								masterListUpdated(MasterClient* pSender);

		void								readMasterMessage(MasterClient* pSender, const QString& title, const QString& content, bool isError)
		{
			emit masterMessage(pSender, title, content, isError);
		}

		/// Forwards the signal to newServerBatchReceivedFromMaster();
		void								newServerBatchReceivedSlot(MasterClient* pSender, const QList<Server* >& servers)
		{
			emit newServerBatchReceivedFromMaster(pSender, servers);
		}
};

/**
 *	@brief Designed to preserve the information about the MasterClient
 *	instance that is sending the signal.
 */
class MasterClientReceiver : public QObject
{
	Q_OBJECT

	public:
		MasterClient*	pMaster;

		MasterClientReceiver(MasterClient* pMaster)
		{
			this->pMaster = pMaster;

			connect(pMaster, SIGNAL( listUpdated() ), this, SLOT( listUpdatedSlot() ) );
			connect(pMaster, SIGNAL( message(const QString&, const QString&, bool) ), this, SLOT( readMasterMessage(const QString&, const QString&, bool) ) );
			connect(pMaster, SIGNAL( messageImportant(const Message&) ), this, SLOT( readMasterMessageImportant(const Message&) ) );
			connect(pMaster, SIGNAL( newServerBatchReceived(const QList<Server* >&) ), this, SLOT( newServerBatchReceivedSlot(const QList<Server* >&) ) );
		}

	signals:
		void			listUpdated(MasterClient* pSender);
		void			message(MasterClient* pSender, const QString& title, const QString& content, bool isError);
		void			messageImportant(MasterClient* pSender, const Message& objMessage);
		void			newServerBatchReceived(MasterClient* pSender, const QList<Server* >& servers);

	protected slots:
		void			listUpdatedSlot()
		{
			emit listUpdated(pMaster);
		}

		void			readMasterMessage(const QString& title, const QString& content, bool isError)
		{
			emit message(pMaster, title, content, isError);
		}

		void			readMasterMessageImportant(const Message& objMessage)
		{
			emit messageImportant(pMaster, objMessage);
		}

		void			newServerBatchReceivedSlot(const QList<Server* >& servers)
		{
			emit newServerBatchReceived(pMaster, servers);
		}
};

#endif

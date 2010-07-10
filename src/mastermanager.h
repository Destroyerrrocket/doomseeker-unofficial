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

#endif

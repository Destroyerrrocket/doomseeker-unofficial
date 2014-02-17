//------------------------------------------------------------------------------
// mastermanager.cpp
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
#include "mastermanager.h"

#include "serverapi/masterclientsignalproxy.h"
#include "serverapi/message.h"
#include "serverapi/server.h"
#include "customservers.h"

// TODO: I don't think that MasterManager should store a duplicate of each
// server (~Zalewa).

MasterManager::MasterManager() : MasterClient()
{
	customServers = new CustomServers();
}

MasterManager::~MasterManager()
{
	clearServers();

	for (int i = 0; i < mastersReceivers.size(); ++i)
	{
		delete mastersReceivers[i];
	}

	for(int i = 0;i < masters.size();i++)
		delete masters[i];

	delete customServers;
}

void MasterManager::addMaster(MasterClient *master)
{
	if(master == NULL)
		return;

	masters.append(master);
	master->setEnabled(true);

	MasterClientSignalProxy* pMasterReceiver = new MasterClientSignalProxy(master);
	connect(pMasterReceiver, SIGNAL( listUpdated(MasterClient*) ),
		this, SLOT( masterListUpdated(MasterClient*) ) );
	connect(pMasterReceiver, SIGNAL( message(MasterClient*, const QString&, const QString&, bool) ),
		this, SIGNAL( masterMessage(MasterClient*, const QString&, const QString&, bool) ) );
	connect(pMasterReceiver, SIGNAL( messageImportant(MasterClient*, const Message&) ),
		this, SIGNAL( masterMessageImportant(MasterClient*, const Message&) ));
	connect(pMasterReceiver, SIGNAL( newServerBatchReceived(MasterClient*, const QList<Server* >&) ),
		this, SLOT( newServerBatchReceivedSlot(MasterClient*, const QList<Server* >&) ) );

	mastersReceivers.append(pMasterReceiver);
}

void MasterManager::masterListUpdated(MasterClient* pSender)
{
	foreach(ServerPtr pServer, pSender->servers())
	{
		registerNewServer(pServer);
	}

	emit listUpdatedForMaster(pSender);
	mastersBeingRefreshed.remove(pSender);
	if (mastersBeingRefreshed.isEmpty())
	{
		emit listUpdated();
	}
}

// [BL] is this actually called anymore?
bool MasterManager::readMasterResponse(QHostAddress& address, unsigned short port, QByteArray &data)
{
	for (int i = 0; i < masters.size(); ++i)
	{
		if (masters[i]->isAddressSame(address, port))
		{
			masters[i]->pushPacketToCache(data);
			if(masters[i]->readMasterResponse(data))
			{
				masters[i]->resetPacketCaching();
				return true;
			}
			return false;
		}
	}

	return false;
}

void MasterManager::refresh()
{
	setTimeouted(false);

	clearServers();

	for(int i = 0;i < masters.size();i++)
	{
		if(!masters[i]->isEnabled())
		{
			continue;
		}

		mastersBeingRefreshed.insert(masters[i]);
		masters[i]->refresh();
	}
}

void MasterManager::timeoutRefreshEx()
{
	foreach(MasterClient* pMaster, mastersBeingRefreshed)
	{
		pMaster->timeoutRefresh();
	}

	mastersBeingRefreshed.clear();
}

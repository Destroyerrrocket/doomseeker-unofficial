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
#include "customservers.h"

// TODO: I don't think that MasterManager should store a duplicate of each
// server (~Zalewa).

MasterManager::MasterManager() : MasterClient()
{
	customServers = new CustomServers();
}

MasterManager::~MasterManager()
{
	servers.clear();
	
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
	
	MasterClientReceiver* pMasterReceiver = new MasterClientReceiver(master);
	connect(pMasterReceiver, SIGNAL( listUpdated(MasterClient*) ), this, SLOT( masterListUpdated(MasterClient*) ) );
	connect(pMasterReceiver, SIGNAL( message(MasterClient*, const QString&, const QString&, bool) ), this, SLOT( readMasterMessage(MasterClient*, const QString&, const QString&, bool) ) );
	connect(pMasterReceiver, SIGNAL( newServerBatchReceived(MasterClient*, const QList<Server* >&) ), this, SLOT( newServerBatchReceivedSlot(MasterClient*, const QList<Server* >&) ) );
	mastersReceivers.append(pMasterReceiver);
	
}

void MasterManager::masterListUpdated(MasterClient* pSender)
{
	const QList<Server*>& serversFromMaster = pSender->serverList();
	foreach(Server* pServer, serversFromMaster)
	{
		servers.append(pServer);
	}

	emit listUpdatedForMaster(pSender);
	mastersBeingRefreshed.remove(pSender);
	if (mastersBeingRefreshed.isEmpty())
	{
		emit listUpdated();
	}
}

bool MasterManager::readMasterResponse(QHostAddress& address, unsigned short port, QByteArray &data)
{
	for (int i = 0; i < masters.size(); ++i)
	{
		if (masters[i]->isAddressDataCorrect(address, port))
		{
			return masters[i]->readMasterResponse(data);
		}
	}
	
	return false;
}

void MasterManager::refresh()
{
	bTimeouted = false;

	// Don't delete the servers yet!
	servers.clear();

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

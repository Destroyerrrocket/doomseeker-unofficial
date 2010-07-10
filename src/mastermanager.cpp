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

MasterManager::MasterManager() : MasterClient(QHostAddress(), 0)
{
	customServers = new CustomServers();
}

MasterManager::~MasterManager()
{
	servers.clear();
	
	for (int i = 0; i < mastersMessageReceivers.size(); ++i)
	{
		delete mastersMessageReceivers[i];
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
	
	MasterClientMessageReceiver* pMasterMessageReceiver = new MasterClientMessageReceiver(master);
	connect(pMasterMessageReceiver, SIGNAL( message(MasterClient*, const QString&, const QString&, bool) ), this, SLOT( readMasterMessage(MasterClient*, const QString&, const QString&, bool) ) );
	mastersMessageReceivers.append(pMasterMessageReceiver);
	
}

void MasterManager::refresh()
{
	// Don't delete the servers yet!
	servers.clear();

	for(int i = 0;i < masters.size();i++)
	{
		if(!masters[i]->isEnabled())
		{
			continue;
		}

		masters[i]->refresh();
		// Qt 4.4 doesn't have list appending.
		foreach(Server *server, masters[i]->serverList())
		{
			servers.append(server);
		}
	}

	//emit listUpdated();
}

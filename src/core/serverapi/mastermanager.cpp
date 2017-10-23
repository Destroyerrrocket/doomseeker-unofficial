//------------------------------------------------------------------------------
// mastermanager.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 Braden "Blzut3" Obrzut <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#include <cassert>

#include "mastermanager.h"

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

	this->connect(master, SIGNAL(listUpdated()), SLOT(masterListUpdated()));
	this->connect(master, SIGNAL(message(QString, QString, bool)),
		SLOT(forwardMasterMessage(QString, QString, bool)));
	this->connect(master, SIGNAL(messageImportant(Message)),
		SLOT(forwardMasterMessageImportant(Message)));
}

QList<ServerPtr> MasterManager::allServers() const
{
	QList<ServerPtr> result;
	foreach (MasterClient *master, masters)
	{
		result << master->servers();
	}
	result << customServers->servers();
	return result;
}

void MasterManager::masterListUpdated()
{
	MasterClient *master = static_cast<MasterClient*>(sender());
	foreach(ServerPtr pServer, master->servers())
	{
		registerNewServer(pServer);
	}

	emit listUpdatedForMaster(master);
	mastersBeingRefreshed.remove(master);
	if (mastersBeingRefreshed.isEmpty())
	{
		emit listUpdated();
	}
}

MasterManager::Response MasterManager::readMasterResponse(const QByteArray &data)
{
	assert(0 && "MasterManager::readMasterResponse should not get called.");
	return RESPONSE_BAD;
}

void MasterManager::refreshStarts()
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
		masters[i]->refreshStarts();
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

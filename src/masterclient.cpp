//------------------------------------------------------------------------------
// masterclient.cpp
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

#include "masterclient.h"
#include "main.h"

#include <QErrorMessage>
#include <QMessageBox>
#include <QUdpSocket>

MasterClient::MasterClient(QHostAddress address, unsigned short port) : QObject(), address(address), port(port)
{
}

MasterClient::~MasterClient()
{
	emptyServerList();
}

void MasterClient::emptyServerList()
{
	for(int i = 0;i < servers.size();i++)
		delete servers[i];
	servers.clear();
}

bool MasterClient::hasServer(const Server* server)
{
	for (int i = 0; i < servers.count(); ++i)
	{
		if (server == servers[i])
			return true;
	}

	return false;
}

void MasterClient::notifyBanned()
{
	QMessageBox::critical(NULL, tr("Banned"), tr("You have been banned from the master server."), QMessageBox::Ok, QMessageBox::Ok);
}

void MasterClient::notifyDelay()
{
	QErrorMessage message;
	message.showMessage(tr("Could not fetch a new server list from the master because not enough time has past."));
}

void MasterClient::refresh()
{
	// Connect to the server
	QUdpSocket socket;
	socket.connectToHost(address, port);
	if(!socket.waitForConnected(1000))
	{
		printf("%s\n", socket.errorString().toAscii().data());
		return;
	}

	// Make request
	QByteArray request;
	if(!sendRequest(request))
		return;
	socket.write(request);
	if(!socket.waitForReadyRead(10000))
		return;

	// get data
	QByteArray data = socket.readAll();
	if(!readRequest(data))
		return;

	socket.close();

	emit listUpdated();
}

////////////////////////////////////////////////////////////////////////////////
MasterManager::MasterManager() : MasterClient(QHostAddress(), 0)
{
	loadMastersFromPlugins();
}

MasterManager::~MasterManager()
{
	for(int i = 0;i < masters.size();i++)
		delete masters[i];
}

void MasterManager::addMaster(MasterClient *master)
{
	if(master == NULL)
		return;

	masters.append(master);
}

void MasterManager::loadMastersFromPlugins()
{
	for(int i = 0;i < Main::enginePlugins.numPlugins();i++)
	{
		const EnginePlugin *plugin = Main::enginePlugins[i]->info->pInterface;
		addMaster(plugin->masterClient());
	}
}

void MasterManager::refresh()
{
	emptyServerList();

	for(int i = 0;i < masters.size();i++)
	{
		masters[i]->refresh();
		// Qt 4.4 doesn't have list appending.
		foreach(Server *server, masters[i]->serverList())
			servers.append(server);
	}

	emit listUpdated();
}

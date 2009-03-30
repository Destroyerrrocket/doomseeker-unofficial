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

#include <QErrorMessage>
#include <QMessageBox>

MasterClient::MasterClient(QHostAddress address, unsigned short port) : QObject(), address(address), port(port)
{
}

void MasterClient::emptyServerList()
{
	for(int i = 0;i < servers.size();i++)
		delete servers[i];
	servers.clear();
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

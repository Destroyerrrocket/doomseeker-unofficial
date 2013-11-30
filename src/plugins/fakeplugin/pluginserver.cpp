//------------------------------------------------------------------------------
// chocolatedoomserver.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include "pluginserver.h"
#include "pluginengineplugin.h"
#include "global.h"
#include "main.h"
#include "serverapi/playerslist.h"

PluginServer::PluginServer(const QHostAddress& address, quint16 port)
: Server(address, port)
{
}

const EnginePlugin* PluginServer::plugin() const
{
	return PluginEnginePlugin::staticInstance();
}

Server::Response PluginServer::readRequest(QByteArray &data)
{
	if (data.left(4) != "FAKF")
	{
		return RESPONSE_BAD;
	}

	this->setName(QString("Fake Server %1").arg(this->port()));
	return RESPONSE_GOOD;
}

bool PluginServer::sendRequest(QByteArray &data)
{
	data = QByteArray("FAKF", 4);
	return true;
}

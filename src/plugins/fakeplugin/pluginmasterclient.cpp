//------------------------------------------------------------------------------
// odamexmasterclient.cpp
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

#include "global.h"
#include "pluginmasterclient.h"
#include "pluginengineplugin.h"
#include "pluginserver.h"
#include <QTimer>

PluginMasterClient::PluginMasterClient()
: MasterClient()
{
	// Master responder cannot be started in Plugin's init procedure
	// or the plugin will fail to load in VC++ debug mode with
	// "Access Violation" error. Fortunately, this constructor is executed
	// somewhere later, and is a safe place to start the responder.
	// Remember that responder must be started in a thread that can handle
	// Qt events.
	PluginEnginePlugin* plugin = (PluginEnginePlugin*) PluginEnginePlugin::staticInstance();
	if (!plugin->isMasterResponderInstantiated())
	{
		plugin->startMasterResponder();
	}
}

PluginMasterClient::~PluginMasterClient()
{
}

bool PluginMasterClient::getServerListRequest(QByteArray &data)
{
	data = "FAKE";
	data.resize(4);
	return true;
}

const EnginePlugin* PluginMasterClient::plugin() const
{
	return PluginEnginePlugin::staticInstance();
}

bool PluginMasterClient::readMasterResponse(QByteArray &data)
{
	QStringList ports = QString(data).split(";");
	foreach (const QString& portEncoded, ports)
	{
		quint16 port = portEncoded.toUShort();
		if (port != 0)
		{
			PluginServer* server = new PluginServer(QHostAddress("127.0.0.1"), port);
			servers.append(server);
		}
	}
	emit listUpdated();
	return true;
}

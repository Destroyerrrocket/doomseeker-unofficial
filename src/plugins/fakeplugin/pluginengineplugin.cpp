//------------------------------------------------------------------------------
// engineplugin.cpp
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

#include "plugins/engineplugin.h"

#include "responder/masterresponder.h"
#include "responder/respondercfg.h"
#include "pluginengineplugin.h"
#include "pluginmasterclient.h"
#include "pluginserver.h"
#include <cassert>

class PluginEnginePlugin::PrivData
{
	public:
		MasterResponder* masterResponder;
};
///////////////////////////////////////////////////////////////////////////////
INSTALL_PLUGIN(PluginEnginePlugin)

QString masterAddress()
{
	return QString("127.0.0.1:%1").arg(ResponderCfg::masterServerPort());
}

PluginEnginePlugin::PluginEnginePlugin()
{
	init("Fake Plugin", NULL,
		EP_Author, "The Doomseeker Team",
		EP_Version, 1,

		EP_HasMasterServer,
		EP_DefaultMaster, masterAddress().toAscii().constData(),
		EP_Done
	);

	d = new PrivData();
	d->masterResponder = NULL;
}

PluginEnginePlugin::~PluginEnginePlugin()
{
	delete d->masterResponder;
	delete d;
}

MasterClient *PluginEnginePlugin::masterClient() const
{
	return new PluginMasterClient();
}

Server* PluginEnginePlugin::server(const QHostAddress &address, unsigned short port) const
{
	return new PluginServer(address, port);
}

bool PluginEnginePlugin::isMasterResponderInstantiated() const
{
	return d->masterResponder != NULL;
}

void PluginEnginePlugin::startMasterResponder()
{
	assert(!isMasterResponderInstantiated());
	d->masterResponder = new MasterResponder();
	d->masterResponder->bind(ResponderCfg::masterServerPort());
}

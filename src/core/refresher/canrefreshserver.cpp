//------------------------------------------------------------------------------
// canrefreshserver.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "canrefreshserver.h"

#include "plugins/engineplugin.h"
#include "serverapi/server.h"

class CanRefreshServer::PrivData
{
public:
	const Server *server;

	const EnginePlugin *plugin() const
	{
		return server->plugin();
	}

	const EnginePlugin::Data *pluginData() const
	{
		return plugin()->data();
	}

	qint64 msSinceLastRefresh() const
	{
		return server->timeMsSinceLastRefresh();
	}

	qint64 secsSinceLastRefresh() const
	{
		return msSinceLastRefresh() / 1000;
	}

	bool wasAlreadyRefreshed() const
	{
		return msSinceLastRefresh() >= 0;
	}
};

CanRefreshServer::CanRefreshServer(const Server *server)
{
	d = new PrivData();
	d->server = server;
}

CanRefreshServer::~CanRefreshServer()
{
	delete d;
}

bool CanRefreshServer::hasEnoughTimeSinceLastRefreshPassed() const
{
	if (!d->wasAlreadyRefreshed())
	{
		return true;
	}

	if (d->server->lastResponse() == Server::RESPONSE_TIMEOUT)
	{
		return d->secsSinceLastRefresh() > 1;
	}
	else
	{
		return d->secsSinceLastRefresh() >= d->pluginData()->refreshThreshold;
	}
}

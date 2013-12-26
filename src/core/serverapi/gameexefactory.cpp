//------------------------------------------------------------------------------
// gameexefactory.cpp
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
#include "gameexefactory.h"

#include "plugins/engineplugin.h"
#include "serverapi/exefile.h"

class GameExeFactory::PrivData
{
public:
	ExeRetriever* offline;
	EnginePlugin* plugin;
	ExeRetriever* server;
};

GameExeFactory::GameExeFactory(EnginePlugin* plugin)
{
	d = new PrivData();
	d->plugin = plugin;
	d->offline = NULL;
	d->server = NULL;
}

GameExeFactory::~GameExeFactory()
{
	delete d;
}

EnginePlugin* GameExeFactory::plugin()
{
	return d->plugin;
}

ExeFile* GameExeFactory::offline()
{
	ExeFile* f = NULL;
	if (d->offline != NULL)
	{
		f = d->offline->get();
	}
	else
	{
		f = new ExeFile();
		f->setProgramName(d->plugin->data()->name);
		f->setExeTypeName(tr("offline"));
		f->setConfigKey("BinaryPath");
	}
	return f;
}

ExeFile* GameExeFactory::server()
{
	ExeFile* f = NULL;
	if (d->server != NULL)
	{
		f = d->server->get();
	}
	else
	{
		f = new ExeFile();
		f->setProgramName(d->plugin->data()->name);
		f->setExeTypeName(tr("server"));
		if (d->plugin->data()->clientOnly)
		{
			f->setConfigKey("BinaryPath");
		}
		else
		{
			f->setConfigKey("ServerBinaryPath");
		}
	}
	return f;
}

void GameExeFactory::setOfflineExeRetriever(ExeRetriever* retriever)
{
	d->offline = retriever;
}

void GameExeFactory::setServerExeRetriever(ExeRetriever* retriever)
{
	d->server = retriever;
}

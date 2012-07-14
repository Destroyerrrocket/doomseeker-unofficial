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
// Copyright (C) 2011 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "gui/configuration/engineconfigurationbasebox.h"
#include "irc/entities/ircnetworkentity.h"
#include "plugins/engineplugin.h"
#include "log.h"
#include "strings.h"

#include <cstdarg>
#include <QPixmap>

EnginePlugin::Data::Data()
{
	// Init the defaults.
	allDMFlags = NULL;
	allowsConnectPassword = false;
	allowsEmail = false;
	allowsJoinPassword = false;
	allowsMOTD = false;
	allowsRConPassword = false;
	allowsURL = false;
	createDMFlagsPagesAutomatic = true;
	defaultServerPort = 10666;
	demoExtensionAutomatic = true;
	demoExtension = "lmp";
	gameModes = NULL;
	gameModifiers = NULL;
	hasMasterServer = false;
	icon = NULL;
	inGameFileDownloads = false;
	pConfig = NULL;
	refreshThreshold = 2;
	supportsRandomMapRotation = false;
	valid = true;
	version = 0;
}

////////////////////////////////////////////////////////////////////////////////

EnginePlugin::EnginePlugin()
{
	d = new Data;
}

EnginePlugin::~EnginePlugin()
{
	delete d->icon;
	delete d->pConfig;
	delete d;
}

ConfigurationBaseBox* EnginePlugin::configuration(QWidget *parent) const
{
	return new EngineConfigurationBaseBox(this, *d->pConfig, parent);
}

void EnginePlugin::init(const char* name, const char* const icon[], ...)
{
	d->name = name;
	d->icon = new QPixmap(icon);
	d->scheme = d->name.replace(' ', "");

	va_list va;
	va_start(va, icon);

	int feature;
	while((feature = va_arg(va, int)) != EP_Done)
	{
		switch(feature)
		{
			default:
				// Since we don't know if the feature has arguments we must abort.
				gLog << QString("%1 plugin attempted to use unknown feature.").arg(name);
				d->valid = false;
				return;

			case EP_Author:
				d->author = va_arg(va, const char*);
				break;
			case EP_Version:
				d->version = va_arg(va, unsigned int);
				break;

			case EP_AllDMFlags:
				d->allDMFlags = va_arg(va, const DMFlags*);
				break;
			case EP_AllowsConnectPassword:
				d->allowsConnectPassword = true;
				break;
			case EP_AllowsEmail:
				d->allowsEmail = true;
				break;
			case EP_AllowsURL:
				d->allowsURL = true;
				break;
			case EP_AllowsJoinPassword:
				d->allowsJoinPassword = true;
				break;
			case EP_AllowsRConPassword:
				d->allowsRConPassword = true;
				break;
			case EP_AllowsMOTD:
				d->allowsMOTD = true;
				break;
			case EP_DefaultMaster:
				d->defaultMaster = va_arg(va, const char*);
				break;
			case EP_DefaultServerPort:
				d->defaultServerPort = va_arg(va, unsigned int);
				break;
			case EP_DemoExtension:
				d->demoExtensionAutomatic = va_arg(va, unsigned int);
				d->demoExtension = va_arg(va, const char*);
				break;
			case EP_DontCreateDMFlagsPagesAutomatic:
				d->createDMFlagsPagesAutomatic = false;
				break;
			case EP_GameModes:
				d->gameModes = va_arg(va, QList<GameMode>*);
				break;
			case EP_GameModifiers:
				d->gameModifiers = va_arg(va, QList<GameCVar>*);
				break;
			case EP_HasMasterServer:
				d->hasMasterServer = true;
				break;
			case EP_InGameFileDownloads:
				d->inGameFileDownloads = true;
				break;
			case EP_IRCChannel:
			{
				// Either create an entity or put the channel in an existing one.
				IRCNetworkEntity entity;
				entity.description = va_arg(va, const char*);
				entity.address = va_arg(va, const char*);
				entity.autojoinChannels << va_arg(va, const char*);

				if(d->ircChannels.contains(entity))
				{
					IRCNetworkEntity &existingEntity = d->ircChannels[d->ircChannels.indexOf(entity)];
					existingEntity.autojoinChannels << entity.autojoinChannels[0];
				}
				else
					d->ircChannels << entity;
				break;
			}
			case EP_SupportsRandomMapRotation:
				d->supportsRandomMapRotation = true;
				break;
			case EP_URLScheme:
				d->scheme = va_arg(va, const char*);
				break;
			case EP_RefreshThreshold:
				d->refreshThreshold = va_arg(va, unsigned int);
				break;
		}
	}

	va_end(va);
}

void EnginePlugin::masterHost(QString &host, unsigned short &port) const
{
	QString str = d->pConfig->setting("Masterserver");
	Strings::translateServerAddress(str, host, port, d->defaultMaster);
}

void EnginePlugin::setConfig(IniSection &ini) const
{
	d->pConfig = new IniSection(ini);

	ini.createSetting("Masterserver", data()->defaultMaster);

	setupConfig(ini);
}

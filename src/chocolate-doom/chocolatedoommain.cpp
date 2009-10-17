//------------------------------------------------------------------------------
// chocolatedoommain.cpp
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

#include "main.h"
#include "sdeapi/pluginloader.hpp"

#include "chocolate-doom/chocolatedoomserver.h"
#include "chocolate-doom/engineChocolateDoomConfig.h"

const // clear warnings
#include "chocolate-doom/chocolatedoom.xpm"

static GeneralEngineInfo ChocolateDoomEngineInfo =
{
	2342,								// Default port
	NULL,								// List of game modes
	0,									// Number of game modes
	NULL,								// List of DMFlags sections
	0,									// Number of DMFlags sections
	false,								// Allows URL
	false,								// Allows E-Mail
	false,								// Allows connect password
	false,								// Allows join password
	false,								// Allows rcon password
	false,								// Allows MOTD
	false,								// Supports random map rotation
	NULL,								// Game modifiers
	0,									// Number of game modifiers
	false,								// Has Master Server
};

class PLUGIN_EXPORT OdamexEnginePlugin : public EnginePlugin
{
	public:
		QString					binaryClient() const
		{
			return Main::config->setting("ChocolateDoomBinaryPath")->string();
		}

		QString					binaryServer() const
		{
			return Main::config->setting("ChocolateDoomServerBinaryPath")->string();
		}

		ConfigurationBoxInfo	*configuration(Config *cfg, QWidget *parent) const
		{
			return EngineChocolateDoomConfigBox::createStructure(cfg, parent);
		}

		const GeneralEngineInfo&	generalEngineInfo() const
		{
			return ChocolateDoomEngineInfo;
		}

		virtual QList<GameCVar>	limits(const GameMode&) const
		{
			return QList<GameCVar>();
		}

		QPixmap			icon() const
		{
			return QPixmap(chocolatedoom_xpm);
		}

		MasterClient	*masterClient() const
		{
			return NULL;
		}

		Server*			server(const QHostAddress &address, unsigned short port) const
		{
			return new ChocolateDoomServer(address, port);
		}
};

static OdamexEnginePlugin chocolatedoom_engine_plugin;
static const PluginInfo chocolatedoom_info = {"Chocolate Doom", "Chocolate Doom server query plugin.", "The Skulltag Team", {0,1,0,0}, MAKEID('E','N','G','N'), &chocolatedoom_engine_plugin};
extern "C" PLUGIN_EXPORT const PluginInfo *doomSeekerInit()
{
	return &chocolatedoom_info;
}

extern "C" PLUGIN_EXPORT void doomSeekerInitConfig()
{
}

//------------------------------------------------------------------------------
// odamexmain.cpp
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

#include <QHostInfo>

#include "global.h"
#include "main.h"
#include "masterclient.h"
#include "strings.h"
#include "sdeapi/pluginloader.hpp"

#include "odamex/engineOdamexConfig.h"
#include "odamex/odamexmasterclient.h"
#include "odamex/odamexserver.h"

const // clear warnings
#include "odamex/odamex.xpm"

static GeneralEngineInfo OdamexEngineInfo =
{
	10666,								// Default port
	OdamexServer::GAME_MODES,			// List of game modes
	NUM_ODAMEX_GAME_MODES,				// Number of game modes
	&OdamexServer::DM_FLAGS,			// List of DMFlags sections
	1,									// Number of DMFlags sections
	true,								// Allows URL
	true,								// Allows E-Mail
	true,								// Allows connect password
	true,								// Allows join password
	true,								// Allows rcon password
	true,								// Allows MOTD
	true,								// Supports random map rotation
	NULL,								// Game modifiers
	0,									// Number of game modifiers
	true,								// Has Master Server
};

class PLUGIN_EXPORT OdamexEnginePlugin : public EnginePlugin
{
	public:
		QString					binaryClient() const
		{
			return Main::config->setting("OdamexBinaryPath")->string();
		}

		QString					binaryServer() const
		{
			return Main::config->setting("OdamexServerBinaryPath")->string();
		}

		ConfigurationBoxInfo	*configuration(Config *cfg, QWidget *parent) const
		{
			return EngineOdamexConfigBox::createStructure(cfg, parent);
		}

		const GeneralEngineInfo&	generalEngineInfo() const
		{
			return OdamexEngineInfo;
		}

		virtual QList<GameCVar>	limits(const GameMode&) const
		{
			return QList<GameCVar>();
		}

		QPixmap			icon() const
		{
			return QPixmap(odamex_xpm);
		}

		MasterClient	*masterClient() const
		{
			// Get server address.
			QString host;
			short int port = 0;

			SettingsData* setting = Main::config->setting("OdamexMasterserver");
			QString str = setting->string();
			Strings::translateServerAddress(str, host, port, "master1.odamex.net", 15000);

			QHostInfo info = QHostInfo::fromName(host);
			if(info.addresses().size() == 0)
				return NULL;
			return new OdamexMasterClient(info.addresses().first(), port);
		}

		Server*			server(const QHostAddress &address, unsigned short port) const
		{
			return (new OdamexServer(address, port));
		}
};

static OdamexEnginePlugin odamex_engine_plugin;
static const PluginInfo odamex_info = {"Odamex", "Odamex server query plugin.", "The Skulltag Team", {0,4,0,0}, MAKEID('E','N','G','N'), &odamex_engine_plugin};
extern "C" PLUGIN_EXPORT const PluginInfo *doomSeekerInit()
{
	return &odamex_info;
}

extern "C" PLUGIN_EXPORT void doomSeekerInitConfig()
{
	Main::config->createSetting("OdamexMasterserver", "master1.odamex.net:15000");
}

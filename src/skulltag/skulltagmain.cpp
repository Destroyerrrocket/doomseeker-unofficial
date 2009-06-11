//------------------------------------------------------------------------------
// skulltagmain.cpp
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
#include "sdeapi/config.hpp"
#include "sdeapi/pluginloader.hpp"

#include "skulltag/skulltagmasterclient.h"
#include "skulltag/skulltagserver.h"
#include "skulltag/engineSkulltagConfig.h"

const // clear warnings
#include "skulltag/skulltag.xpm"

class PLUGIN_EXPORT SkulltagEnginePlugin : public EnginePlugin
{
	public:
		ConfigurationBoxInfo *configuration(Config *cfg, QWidget *parent) const
		{
			return EngineSkulltagConfigBox::createStructure(cfg, parent);
		}

		short			defaultServerPort() const { return 10666; }

		QPixmap			icon() const
		{
			return QPixmap(skulltag_xpm);
		}

		MasterClient	*masterClient() const
		{
			// Get server address.
			QString host;
			short int port = 0;

			SettingsData* setting = Main::config->setting("SkulltagMasterserver");
			QString str = setting->string();
			Main::translateServerAddress(str, host, port, "skulltag.servegame.com", 15300);

			QHostInfo info = QHostInfo::fromName(host);
			if(info.addresses().size() == 0)
				return NULL;
			return new SkulltagMasterClient(info.addresses().first(), port);
		}

		Server*			server(const QHostAddress &address, unsigned short port) const
		{
			return (new SkulltagServer(address, port));
		}
};

static SkulltagEnginePlugin skulltag_engine_plugin;
static const PluginInfo skulltag_info = {"Skulltag", "Skulltag server query plugin.", "The Skulltag Team", {0,1,0,0}, MAKEID('E','N','G','N'), &skulltag_engine_plugin};
extern "C" PLUGIN_EXPORT const PluginInfo *doomSeekerInit()
{
	return &skulltag_info;
}

extern "C" PLUGIN_EXPORT void doomSeekerInitConfig()
{
	Main::config->createSetting("SkulltagMasterserver", "skulltag.servegame.com:15300");
}

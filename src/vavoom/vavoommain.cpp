//------------------------------------------------------------------------------
// vavoommain.cpp
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
#include "gui/configuration/engineconfigurationbasebox.h"
#include "sdeapi/pluginloader.hpp"

#include "vavoom/vavoommain.h"
#include "vavoom/vavoommasterclient.h"
#include "vavoom/vavoomserver.h"

const // clear warnings
#include "vavoom/vavoom.xpm"

class PLUGIN_EXPORT VavoomEnginePlugin : public EnginePlugin
{
	public:
		const DMFlags*					allDMFlags() const { return NULL; }

		bool							allowsURL() const { return true; }
		bool							allowsEmail() const { return true; }
		bool							allowsConnectPassword() const { return true; }
		bool							allowsJoinPassword() const { return true; }
		bool							allowsRConPassword() const { return true; }
		bool							allowsMOTD() const { return true; }


		ConfigurationBaseBox*			configuration(IniSection &cfg, QWidget *parent) const
		{
			return new EngineConfigurationBaseBox(VavoomMain::get(), cfg, parent);
		}

		unsigned short					defaultServerPort() const { return 26000; }
		const QList<GameMode>*			gameModes() const { return NULL; }
		const QList<GameCVar>*			gameModifiers() const { return NULL; }
		bool							hasMasterServer() const { return true; }

		virtual QList<GameCVar>	limits(const GameMode&) const
		{
			return QList<GameCVar>();
		}

		QPixmap			icon() const
		{
			return QPixmap(vavoom_xpm);
		}

		MasterClient	*masterClient() const
		{
			return new VavoomMasterClient();
		}

		void			masterHost(QString &host, unsigned short &port) const
		{
			QString str = Main::ini->setting("Vavoom", "Masterserver");
			Strings::translateServerAddress(str, host, port, "127.0.0.1", 26001);
		}

		Server*			server(const QHostAddress &address, unsigned short port) const
		{
			return (new VavoomServer(address, port));
		}

		bool							supportsRandomMapRotation() const { return false; }
};

static VavoomEnginePlugin vavoom_engine_plugin;
const PluginInfo VavoomMain::info = {"Vavoom", "Vavoom server query plugin.", "The Skulltag Team", {0,3,0,0}, MAKEID('E','N','G','N'), &vavoom_engine_plugin};
extern "C" PLUGIN_EXPORT const PluginInfo *doomSeekerInit()
{
	return VavoomMain::get();
}

extern "C" PLUGIN_EXPORT void doomSeekerInitConfig(IniSection &config)
{
	config.createSetting("Masterserver", "127.0.0.1:26001");
}

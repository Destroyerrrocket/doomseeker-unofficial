//------------------------------------------------------------------------------
// zdaemonmain.cpp
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include <QHostInfo>

#include "global.h"
#include "main.h"
#include "masterclient.h"
#include "strings.h"
#include "gui/configuration/engineconfigurationbasebox.h"
#include "sdeapi/pluginloader.hpp"

#include "zdaemon/zdaemongameinfo.h"
#include "zdaemon/zdaemonmain.h"
#include "zdaemon/zdaemonmasterclient.h"
#include "zdaemon/zdaemonserver.h"

const // clear warnings
#include "zdaemon/zdaemon.xpm"

class PLUGIN_EXPORT ZDaemonEnginePlugin : public EnginePlugin
{
	public:
		const DMFlags*					allDMFlags() const { return ZDaemonGameInfo::dmFlags(); }

		bool							allowsURL() const { return true; }
		bool							allowsEmail() const { return true; }
		bool							allowsConnectPassword() const { return true; }
		bool							allowsJoinPassword() const { return true; }
		bool							allowsRConPassword() const { return true; }
		bool							allowsMOTD() const { return true; }


		ConfigurationBaseBox*			configuration(IniSection &cfg, QWidget *parent) const
		{
			return new EngineConfigurationBaseBox(ZDaemonMain::get(), cfg, parent);
		}

		unsigned short					defaultServerPort() const { return 10666; }
		const QList<GameMode>*			gameModes() const { return NULL; }
		const QList<GameCVar>*			gameModifiers() const { return NULL; }
		bool							hasMasterServer() const { return true; }

		virtual QList<GameCVar>	limits(const GameMode&) const
		{
			return QList<GameCVar>();
		}

		QPixmap			icon() const
		{
			return QPixmap(zdaemon_xpm);
		}

		MasterClient	*masterClient() const
		{
			return new ZDaemonMasterClient();
		}

		void			masterHost(QString &host, unsigned short &port) const
		{
			QString str = pConfig->setting("Masterserver");
			Strings::translateServerAddress(str, host, port, "master.zdaemon.org", 15300);
		}

		Server*			server(const QHostAddress &address, unsigned short port) const
		{
			return (new ZDaemonServer(address, port));
		}

		bool							supportsRandomMapRotation() const { return false; }
};

static ZDaemonEnginePlugin zdaemon_engine_plugin;
PluginInfo ZDaemonMain::info = {"ZDaemon", "ZDaemon server query plugin.", "The Skulltag Team", {0,3,0,0}, MAKEID('E','N','G','N'), &zdaemon_engine_plugin};
extern "C" PLUGIN_EXPORT PluginInfo *doomSeekerInit()
{
	return ZDaemonMain::get();
}

extern "C" PLUGIN_EXPORT void doomSeekerInitConfig(IniSection &config)
{
	config.createSetting("Masterserver", "master.zdaemon.org:15300");
	//config.createSetting("Masterserver", "altdeath.com:80");
}

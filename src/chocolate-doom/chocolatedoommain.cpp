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
#include "gui/configuration/engineconfigurationbasebox.h"
#include "sdeapi/pluginloader.hpp"

#include "chocolate-doom/chocolatedoommain.h"
#include "chocolate-doom/chocolatedoombinaries.h"
#include "chocolate-doom/chocolatedoomserver.h"

const // clear warnings
#include "chocolate-doom/chocolatedoom.xpm"

class PLUGIN_EXPORT ChocolateDoomEnginePlugin : public EnginePlugin
{
	public:
		const DMFlags*					allDMFlags() const { return NULL; }

		bool							allowsURL() const { return false; }
		bool							allowsEmail() const { return false; }
		bool							allowsConnectPassword() const { return false; }
		bool							allowsJoinPassword() const { return false; }
		bool							allowsRConPassword() const { return false; }
		bool							allowsMOTD() const { return false; }

		ConfigurationBaseBox*			configuration(IniSection &cfg, QWidget *parent) const
		{
			return new EngineConfigurationBaseBox(ChocolateDoomMain::get(), cfg, parent);
		}

		unsigned short					defaultServerPort() const { return 2342; }
		const QList<GameMode>*			gameModes() const { return NULL; }
		const QList<GameCVar>*			gameModifiers() const { return NULL; }
		bool							hasMasterServer() const { return false; }

		QList<GameCVar>					limits(const GameMode& mode) const { return QList<GameCVar>(); }

		QPixmap							icon() const
		{
			return QPixmap(chocolatedoom_xpm);
		}

		MasterClient*					masterClient() const
		{
			return NULL;
		}
		void							masterHost(QString &host, unsigned short &port) const {}

		Server*							server(const QHostAddress &address, unsigned short port) const
		{
			return new ChocolateDoomServer(address, port);
		}

		bool							supportsRandomMapRotation() const { return false; }
};

static ChocolateDoomEnginePlugin chocolatedoom_engine_plugin;
const PluginInfo ChocolateDoomMain::chocolatedoom_info = {"Chocolate Doom", "Chocolate Doom server query plugin.", "The Skulltag Team", {0,3,0,0}, MAKEID('E','N','G','N'), &chocolatedoom_engine_plugin};
extern "C" PLUGIN_EXPORT const PluginInfo *doomSeekerInit()
{
	return ChocolateDoomMain::get();
}

extern "C" PLUGIN_EXPORT void doomSeekerInitConfig(IniSection &config)
{
}

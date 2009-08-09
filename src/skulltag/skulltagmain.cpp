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

static GeneralEngineInfo SkulltagEngineInfo =
{
	10666,								// Default server port
	SkulltagServer::GAME_MODES,			// List of game modes
	NUM_SKULLTAG_GAME_MODES,			// Number of game modes
	SkulltagServer::DM_FLAGS,			// List of DMFlags sections
	3,									// Number of DMFlags sections
	true,								// Allows URL
	true,								// Allows E-Mail
	true,								// Allows connect password
	true,								// Allows join password
	true,								// Allows rcon password
	true,								// Allows MOTD
	true,								// Supports random map rotation
	SkulltagServer::GAME_MODIFIERS,		// Game modifiers
	NUM_SKULLTAG_GAME_MODIFIERS,		// Number of game modifiers
};

class PLUGIN_EXPORT SkulltagEnginePlugin : public EnginePlugin
{
	public:
		QString					binaryClient() const
		{
			return Main::config->setting("SkulltagBinaryPath")->string();
		}

		QString					binaryServer() const
		{
			#ifdef Q_OS_WIN32
				return binaryClient();
			#else
				return Main::config->setting("SkulltagServerBinaryPath")->string();
			#endif
		}

		ConfigurationBoxInfo *configuration(Config *cfg, QWidget *parent) const
		{
			return EngineSkulltagConfigBox::createStructure(cfg, parent);
		}

		const GeneralEngineInfo&	generalEngineInfo() const
		{
			return SkulltagEngineInfo;
		}

		virtual QList<GameCVar>	limits(const GameMode& gm) const
		{
			QList<GameCVar> gl;

			int m = gm.modeIndex();

			if (m != GameMode::SGMICooperative
			&&	m != SkulltagServer::GAMEMODE_INVASION
			&&	m != SkulltagServer::GAMEMODE_SURVIVAL)
			{
				gl << GameCVar(QObject::tr("Time limit:"), "timelimit");
			}

			if (m == GameMode::SGMIDeathmatch
			||	m == SkulltagServer::GAMEMODE_DUEL
			||	m == GameMode::SGMITeamDeathmatch
			||	m == SkulltagServer::GAMEMODE_TERMINATOR)
			{
				gl << GameCVar(QObject::tr("Frag limit:"), "fraglimit");
			}

			if (m == GameMode::SGMICTF
			||	m == SkulltagServer::GAMEMODE_DOMINATION
			||	m == SkulltagServer::GAMEMODE_ONEFLAGCTF
			||	m == SkulltagServer::GAMEMODE_POSSESSION
			||	m == SkulltagServer::GAMEMODE_SKULLTAG
			||	m == SkulltagServer::GAMEMODE_TEAMGAME
			||	m == SkulltagServer::GAMEMODE_TEAMPOSSESSION)
			{
				gl << GameCVar(QObject::tr("Point limit:"), "pointlimit");
			}

			if (m == SkulltagServer::GAMEMODE_DUEL
			||	m == SkulltagServer::GAMEMODE_LASTMANSTANDING
			||	m == SkulltagServer::GAMEMODE_TEAMLMS)
			{
				gl << GameCVar(QObject::tr("Win limit:"), "winlimit");
			}

			if (m == SkulltagServer::GAMEMODE_DUEL)
			{
				gl << GameCVar(QObject::tr("Duel limit:"), "duellimit");
			}

			gl << GameCVar(QObject::tr("Max. lives:"), "sv_maxlives");

			return gl;
		}

		QPixmap						icon() const
		{
			return QPixmap(skulltag_xpm);
		}

		MasterClient				*masterClient() const
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

		Server*						server(const QHostAddress &address, unsigned short port) const
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
	// Default to where the automatic installations install to.
#ifdef Q_OS_WIN32
	Main::config->createSetting("SkulltagBinaryPath", "C:\\Program Files\\Skulltag\\Skulltag.exe");
#else
	Main::config->createSetting("SkulltagBinaryPath", "/usr/games/skulltag/skulltag");
	Main::config->createSetting("SkulltagServerBinaryPath", "/usr/games/skulltag/skulltag-server");
#endif

	Main::config->createSetting("SkulltagMasterserver", "skulltag.servegame.com:15300");
}

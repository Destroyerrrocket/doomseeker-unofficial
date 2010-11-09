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

#include "datapaths.h"
#include "global.h"
#include "main.h"
#include "pathfinder.h"
#include "strings.h"
#include "irc/ircnetworkentity.h"
#include "sdeapi/config.hpp"
#include "sdeapi/pluginloader.hpp"

#include "skulltag/huffman/huffman.h"
#include "skulltag/skulltagbinaries.h"
#include "skulltag/skulltaggameinfo.h"
#include "skulltag/skulltagmain.h"
#include "skulltag/skulltagmasterclient.h"
#include "skulltag/skulltagserver.h"
#include "skulltag/engineSkulltagConfig.h"

const // clear warnings
#include "skulltag/skulltag.xpm"

class PLUGIN_EXPORT SkulltagEnginePlugin : public EnginePlugin
{
	public:
		const DMFlags*					allDMFlags() const
		{
			return SkulltagGameInfo::dmFlags();
		}

		bool							allowsURL() const { return true; }
		bool							allowsEmail() const { return true; }
		bool							allowsConnectPassword() const { return true; }
		bool							allowsJoinPassword() const { return true; }
		bool							allowsRConPassword() const { return true; }
		bool							allowsMOTD() const { return true; }

		ConfigurationBaseBox *configuration(IniSection &cfg, QWidget *parent) const
		{
			return new EngineSkulltagConfigBox(SkulltagMain::get(), cfg, parent);
		}

		unsigned short					defaultServerPort() const { return 10666; }

		const QList<GameMode>*			gameModes() const
		{
			return SkulltagGameInfo::gameModes();
		}

		const QList<GameCVar>*			gameModifiers() const
		{
			return SkulltagGameInfo::gameModifiers();
		}

		bool							hasMasterServer() const { return true; }

		QList<GameCVar>	limits(const GameMode& gm) const
		{
			QList<GameCVar> gl;

			int m = gm.modeIndex();

			if (m != GameMode::SGMICooperative
			&&	m != SkulltagGameInfo::GAMEMODE_INVASION
			&&	m != SkulltagGameInfo::GAMEMODE_SURVIVAL)
			{
				gl << GameCVar(QObject::tr("Time limit:"), "timelimit");
			}

			if (m == GameMode::SGMIDeathmatch
			||	m == SkulltagGameInfo::GAMEMODE_DUEL
			||	m == GameMode::SGMITeamDeathmatch
			||	m == SkulltagGameInfo::GAMEMODE_TERMINATOR)
			{
				gl << GameCVar(QObject::tr("Frag limit:"), "fraglimit");
			}

			if (m == GameMode::SGMICTF
			||	m == SkulltagGameInfo::GAMEMODE_DOMINATION
			||	m == SkulltagGameInfo::GAMEMODE_ONEFLAGCTF
			||	m == SkulltagGameInfo::GAMEMODE_POSSESSION
			||	m == SkulltagGameInfo::GAMEMODE_SKULLTAG
			||	m == SkulltagGameInfo::GAMEMODE_TEAMGAME
			||	m == SkulltagGameInfo::GAMEMODE_TEAMPOSSESSION)
			{
				gl << GameCVar(QObject::tr("Point limit:"), "pointlimit");
			}

			if (m == SkulltagGameInfo::GAMEMODE_DUEL
			||	m == SkulltagGameInfo::GAMEMODE_LASTMANSTANDING
			||	m == SkulltagGameInfo::GAMEMODE_TEAMLMS)
			{
				gl << GameCVar(QObject::tr("Win limit:"), "winlimit");
			}

			if (m == SkulltagGameInfo::GAMEMODE_DUEL)
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
			return new SkulltagMasterClient();
		}

		void						masterHost(QString &host, unsigned short &port) const
		{
			QString str = pConfig->setting("Masterserver");
			Strings::translateServerAddress(str, host, port, "skulltag.servegame.com", 15300);
		}

		void						registerIRCServer(QVector<IRCNetworkEntity> &networks) const
		{
			IRCNetworkEntity entity;
			entity.address = "irc.skulltag.net";
			entity.description = "Skulltag";
			entity.autojoinChannels << "#skulltag";

			if(!networks.contains(entity))
				networks << entity;
		}

		Server*						server(const QHostAddress &address, unsigned short port) const
		{
			return (new SkulltagServer(address, port));
		}

		bool						supportsRandomMapRotation() const { return true; }
};

static SkulltagEnginePlugin skulltag_engine_plugin;
PluginInfo SkulltagMain::info = {"Skulltag", "Skulltag server query plugin.", "The Skulltag Team", {0,7,0,0}, MAKEID('E','N','G','N'), &skulltag_engine_plugin};
extern "C" PLUGIN_EXPORT PluginInfo *doomSeekerInit()
{
	HUFFMAN_Construct();
	return SkulltagMain::get();
}

extern "C" PLUGIN_EXPORT void doomSeekerInitConfig(IniSection &config)
{
	// Default to where the automatic installations install to.
#ifdef Q_OS_WIN32
	QString programFilesDirectory = DataPaths::programFilesDirectory(DataPaths::x86);
	QString trimPattern = QString("\\/");
	QString paths = Strings::trimr(programFilesDirectory, trimPattern);
	
	paths += "\\Skulltag;" + Main::workingDirectory + ";.";

	PathFinder pf(paths);
	config.createSetting("BinaryPath", pf.findFile("skulltag.exe"));
#else
	PathFinder pf(QString("/usr/games/skulltag;/usr/local/games/skulltag;/usr/share/games/skulltag;") + Main::workingDirectory + ";.");
	config.createSetting("BinaryPath", pf.findFile("skulltag"));
	config.createSetting("ServerBinaryPath", pf.findFile("skulltag-server"));
#endif

	config.createSetting("Masterserver", "skulltag.servegame.com:15300");
	config.createSetting("EnableTesting", true);
}

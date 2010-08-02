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
#include "strings.h"
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

		ConfigurationBoxInfo *configuration(IniSection *cfg, QWidget *parent) const
		{
			return EngineSkulltagConfigBox::createStructure(SkulltagMain::get(), cfg, parent);
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
			QString str = Main::ini->setting("Skulltag", "Masterserver")->strValue();
			Strings::translateServerAddress(str, host, port, "skulltag.servegame.com", 15300);
		}

		Server*						server(const QHostAddress &address, unsigned short port) const
		{
			return (new SkulltagServer(address, port));
		}

		bool						supportsRandomMapRotation() const { return true; }
};

static SkulltagEnginePlugin skulltag_engine_plugin;
const PluginInfo SkulltagMain::info = {"Skulltag", "Skulltag server query plugin.", "The Skulltag Team", {0,6,0,0}, MAKEID('E','N','G','N'), &skulltag_engine_plugin};
extern "C" PLUGIN_EXPORT const PluginInfo *doomSeekerInit()
{
	HUFFMAN_Construct();
	return SkulltagMain::get();
}

extern "C" PLUGIN_EXPORT void doomSeekerInitConfig(IniSection *config)
{
	// Default to where the automatic installations install to.
#ifdef Q_OS_WIN32
	QString programFilesPath = DataPaths::programFilesDirectory(DataPaths::x86);
	Strings::trimr(programFilesPath, "\\");
	
	QString defaultSkulltagBinaryPath = programFilesPath + "\\Skulltag\\Skulltag.exe";

	config->createSetting("SkulltagBinaryPath", defaultSkulltagBinaryPath);
#else
	config->createSetting("SkulltagBinaryPath", "/usr/games/skulltag/skulltag");
	config->createSetting("SkulltagServerBinaryPath", "/usr/games/skulltag/skulltag-server");
#endif

	config->createSetting("SkulltagMasterserver", "skulltag.servegame.com:15300");
	config->createSetting("SkulltagEnableTesting", true);
}

//------------------------------------------------------------------------------
// skulltagengineplugin.cpp
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

#include "datapaths.h"
#include "main.h"
#include "pathfinder.h"
#include "plugins/engineplugin.h"
#include "strings.h"

#include "huffman/huffman.h"
#include "skulltagbinaries.h"
#include "skulltaggameinfo.h"
#include "skulltagengineplugin.h"
#include "skulltagmasterclient.h"
#include "skulltagserver.h"
#include "engineSkulltagConfig.h"

INSTALL_PLUGIN(SkulltagEnginePlugin)

SkulltagEnginePlugin::SkulltagEnginePlugin()
{
	HUFFMAN_Construct();

	const // clear warnings
	#include "skulltag.xpm"

	init("Skulltag", skulltag_xpm,
		EP_Author, "The Doomseeker Team",
		EP_Version, 7,

		EP_AllowsURL,
		EP_AllowsEmail,
		EP_AllowsConnectPassword,
		EP_AllowsJoinPassword,
		EP_AllowsRConPassword,
		EP_AllowsMOTD,
		EP_DefaultServerPort, 10666,
		EP_HasMasterServer,
		EP_DefaultMaster, "skulltag.servegame.com:15300",
		EP_SupportsRandomMapRotation,
		EP_GameModes, SkulltagGameInfo::gameModes(),
		EP_GameModifiers, SkulltagGameInfo::gameModifiers(),
		EP_AllDMFlags, SkulltagGameInfo::dmFlags(),
		EP_IRCChannel, "Skulltag", "irc.skulltag.net", "#skulltag",
		EP_Done
	);
}

void SkulltagEnginePlugin::setupConfig(IniSection &config)
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

ConfigurationBaseBox *SkulltagEnginePlugin::configuration(QWidget *parent) const
{
	return new EngineSkulltagConfigBox(staticInstance(), *data()->pConfig, parent);
}

QList<GameCVar>	SkulltagEnginePlugin::limits(const GameMode& gm) const
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

MasterClient *SkulltagEnginePlugin::masterClient() const
{
	return new SkulltagMasterClient();
}

Server* SkulltagEnginePlugin::server(const QHostAddress &address, unsigned short port) const
{
	return new SkulltagServer(address, port);
}

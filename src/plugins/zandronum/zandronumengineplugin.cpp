//------------------------------------------------------------------------------
// zandronumengineplugin.cpp
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net> (skulltagengineplugin.cpp)
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include <datapaths.h>
#include <pathfinder/pathfinder.h>
#include <plugins/engineplugin.h>
#include <strings.h>

#include "huffman/huffman.h"
#include "createserverdialogpages/flagspage.h"
#include "zandronumbinaries.h"
#include "zandronumgamehost.h"
#include "zandronumgameinfo.h"
#include "zandronumengineplugin.h"
#include "zandronummasterclient.h"
#include "zandronumserver.h"
#include "enginezandronumconfigbox.h"

INSTALL_PLUGIN(ZandronumEnginePlugin)

ZandronumEnginePlugin::ZandronumEnginePlugin()
{
	HUFFMAN_Construct();

	const // clear warnings
	#include "zandronum.xpm"

	init("Zandronum", zandronum_xpm,
		EP_Author, "The Doomseeker Team",
		EP_Version, 15,

		EP_AllowsURL,
		EP_AllowsEmail,
		EP_AllowsConnectPassword,
		EP_AllowsJoinPassword,
		EP_AllowsRConPassword,
		EP_AllowsMOTD,
#if defined(Q_OS_WIN32) || defined(Q_OS_MAC)
		EP_ClientOnly,
#endif
		EP_DontCreateDMFlagsPagesAutomatic,
		EP_DefaultServerPort, 10666,
		EP_HasMasterServer,
		EP_DefaultMaster, "master.zandronum.com:15300",
		EP_SupportsRandomMapRotation,
		EP_GameModes, ZandronumGameInfo::gameModes(),
		EP_GameModifiers, ZandronumGameInfo::gameModifiers(),
		EP_IRCChannel, "Zandronum", "irc.zandronum.com", "#zandronum",
		EP_RefreshThreshold, 10,
		EP_DemoExtension, false, "cld",
		EP_URLScheme, "zan",
		EP_Done
	);
}

void ZandronumEnginePlugin::setupConfig(IniSection &config) const
{
	// Default to where the automatic installations install to.
#ifdef Q_OS_WIN32
	QString programFilesDirectory = DataPaths::programFilesDirectory(DataPaths::x86);
	QString trimPattern = QString("\\/");
	QString paths = Strings::trimr(programFilesDirectory, trimPattern);

	paths += "\\Zandronum;" + gDefaultDataPaths->workingDirectory() + ";.";

	PathFinder pf(paths.split(";"));
	config.createSetting("BinaryPath", pf.findFile("zandronum.exe"));
#else
	QString paths = QString("/usr/bin;/usr/local/bin;/usr/share/bin;/usr/games/zandronum;/usr/local/games/zandronum;/usr/share/games/zandronum;") + gDefaultDataPaths->workingDirectory() + ";.";
	PathFinder pf(paths.split(";"));
	config.createSetting("BinaryPath", pf.findFile("zandronum"));
	config.createSetting("ServerBinaryPath", pf.findFile("zandronum-server"));
#endif

	config.createSetting("Masterserver", data()->defaultMaster);
	config.createSetting("EnableTesting", true);
	config.createSetting("AllowServersToDisplayMyCountry", false);
}

ConfigurationBaseBox *ZandronumEnginePlugin::configuration(QWidget *parent) const
{
	return new EngineZandronumConfigBox(staticInstance(), *data()->pConfig, parent);
}

QList<CreateServerDialogPage*> ZandronumEnginePlugin::createServerDialogPages(
	CreateServerDialog* pDialog)
{
	QList<CreateServerDialogPage*> pages;

	pages << new FlagsPage(pDialog);

	return pages;
}

GameHost* ZandronumEnginePlugin::gameHost()
{
	return new ZandronumGameHost();
}

QList<GameCVar> ZandronumEnginePlugin::limits(const GameMode& gm) const
{
	QList<GameCVar> gl;

	int m = gm.index();

	if (m != GameMode::SGM_Cooperative
	&&	m != ZandronumGameInfo::GAMEMODE_INVASION
	&&	m != ZandronumGameInfo::GAMEMODE_SURVIVAL)
	{
		gl << GameCVar(QObject::tr("Time limit:"), "+timelimit");
	}

	if (m == GameMode::SGM_Deathmatch
	||	m == ZandronumGameInfo::GAMEMODE_DUEL
	||	m == GameMode::SGM_TeamDeathmatch
	||	m == ZandronumGameInfo::GAMEMODE_TERMINATOR)
	{
		gl << GameCVar(QObject::tr("Frag limit:"), "+fraglimit");
	}

	if (m == GameMode::SGM_CTF
	||	m == ZandronumGameInfo::GAMEMODE_DOMINATION
	||	m == ZandronumGameInfo::GAMEMODE_ONEFLAGCTF
	||	m == ZandronumGameInfo::GAMEMODE_POSSESSION
	||	m == ZandronumGameInfo::GAMEMODE_SKULLTAG
	||	m == ZandronumGameInfo::GAMEMODE_TEAMGAME
	||	m == ZandronumGameInfo::GAMEMODE_TEAMPOSSESSION)
	{
		gl << GameCVar(QObject::tr("Point limit:"), "+pointlimit");
	}

	if (m == ZandronumGameInfo::GAMEMODE_DUEL
	||	m == ZandronumGameInfo::GAMEMODE_LASTMANSTANDING
	||	m == ZandronumGameInfo::GAMEMODE_TEAMLMS)
	{
		gl << GameCVar(QObject::tr("Win limit:"), "+winlimit");
	}

	if (m == ZandronumGameInfo::GAMEMODE_DUEL)
	{
		gl << GameCVar(QObject::tr("Duel limit:"), "+duellimit");
	}

	gl << GameCVar(QObject::tr("Max. lives:"), "+sv_maxlives");

	return gl;
}

MasterClient *ZandronumEnginePlugin::masterClient() const
{
	return new ZandronumMasterClient();
}

ServerPtr ZandronumEnginePlugin::mkServer(const QHostAddress &address, unsigned short port) const
{
	return ServerPtr(new ZandronumServer(address, port));
}

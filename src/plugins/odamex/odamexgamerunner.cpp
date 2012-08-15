//------------------------------------------------------------------------------
// odamexgamerunner.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "datapaths.h"
#include "main.h"
#include "odamexgamerunner.h"
#include "odamexgameinfo.h"
#include "odamexserver.h"

OdamexGameRunner::OdamexGameRunner(const OdamexServer* server)
: GameRunner(server),
  odamexServer(server)
{
}

bool OdamexGameRunner::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword, const QString &wadTargetDirectory)
{
	if(!GameRunner::connectParameters(args, pf, iwadFound, connectPassword, wadTargetDirectory))
		return false;

	const QStringList& dehPatches = odamexServer->dehs();
	if(dehPatches.count() > 0)
	{
		args << "-deh";
		foreach(QString patch, dehPatches)
		{
			QString file = pf.findFile(patch.toLower());
			args << file;
		}
	}

	if(iwadFound)
	{
#ifdef Q_OS_WIN32
		const char* const PATH_SEPARATOR = ";";
#else
		const char* const PATH_SEPARATOR = ":";
#endif
		QString waddir;

		// Waddir - Work around for an Odamex bug.
		// Also, we want to pass the wadseeker target directory here so in the
		// case where the server changes wads Odamex can download to it as well.
		args << "-waddir";
		QString iwad = server->iwadName();
		if(!wadTargetDirectory.isEmpty())
			waddir = wadTargetDirectory + PATH_SEPARATOR;
		waddir += pf.findFile(iwad.toLower());
		waddir.truncate(waddir.length() - iwad.length());
		for(int i = 0;i < server->numWads();i++)
		{
			QString wad = server->wad(i).name;
			QString pwaddir = pf.findFile(wad.toLower());
			pwaddir.truncate(pwaddir.length() - wad.length());
			if(!pwaddir.isEmpty())
				waddir += PATH_SEPARATOR + pwaddir;
		}
		args << waddir;
	}
	return true;
}

void OdamexGameRunner::hostProperties(QStringList& args) const
{
	args << "-skill" << QString::number(server->gameSkill() + 1); // from 1 to 5

	const QStringList& mapsList = server->mapsList();
	if (!mapsList.isEmpty())
	{
		foreach (QString map, mapsList)
		{
			args << "+addmap" << map;
		}
	}
	args << "+shufflemaplist" << QString::number( static_cast<int>(server->randomMapRotation()) );

	unsigned int modeNum;
	switch(server->gameMode().modeIndex())
	{
		default:
		case GameMode::SGMICooperative: modeNum = 0; break;
		case GameMode::SGMIDeathmatch: modeNum = 1; break;
		case GameMode::SGMITeamDeathmatch: modeNum = 2; break;
		case GameMode::SGMICTF: modeNum = 3; break;
	}
	args << "+sv_gametype" << QString::number(modeNum);

	if (!server->map().isEmpty())
	{
		args << "+map" << server->map();
	}

	args << "+join_password" << "\"" + server->joinPassword() + "\"";
	args << "+rcon_password" << "\"" + server->rconPassword() + "\"";
	args << "+sv_email" << "\"" + server->eMail() + "\"";
	args << "+sv_hostname" << "\"" + server->name() + "\"";
	args << "+sv_maxclients" << QString::number(server->maximumClients());
	args << "+sv_maxplayers" << QString::number(server->maximumPlayers());
	args << "+sv_website" << "\"" + server->website() + "\"";

	QString motd = server->messageOfTheDay();
	args << "+sv_motd" << "\"" + motd.replace("\n", "\\n") + "\"";

	args << "+sv_usemasters" << QString::number(static_cast<int>( server->isBroadcastingToMaster() ));
}

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
#include "odamexgamerunner.h"

#include <datapaths.h>
#include "odamexgameinfo.h"
#include "odamexserver.h"

OdamexGameClientRunner::OdamexGameClientRunner(QSharedPointer<OdamexServer> server)
: GameClientRunner(server)
{
	this->server = server;
	setArgForDemoRecord("-netrecord");
	set_addConnectCommand(&OdamexGameClientRunner::addConnectCommand);
	set_addExtra(&OdamexGameClientRunner::addExtra);
	set_addPassword(&OdamexGameClientRunner::addPassword);
}

void OdamexGameClientRunner::addConnectCommand()
{
	GameClientRunner::addConnectCommand_default();
	if(server->isLocked())
		args() << connectPassword();
}

void OdamexGameClientRunner::addExtra()
{
	const QStringList& dehPatches = server->dehs();
	if(dehPatches.count() > 0)
	{
		args() << "-deh";
		foreach(QString patch, dehPatches)
		{
			QString file = pathFinder().findFile(patch.toLower());
			args() << file;
		}
	}

	if (isIwadFound())
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
		args() << "-waddir";
		QString iwad = server->iwad();
		if (!wadTargetDirectory().isEmpty())
		{
			waddir = wadTargetDirectory() + PATH_SEPARATOR;
		}
		waddir += pathFinder().findFile(iwad.toLower());
		waddir.truncate(waddir.length() - iwad.length());
		for (int i = 0;i < server->numWads();i++)
		{
			QString wad = server->wad(i).name();
			QString pwaddir = pathFinder().findFile(wad.toLower());
			pwaddir.truncate(pwaddir.length() - wad.length());
			if(!pwaddir.isEmpty())
				waddir += PATH_SEPARATOR + pwaddir;
		}
		args() << waddir;
	}
}

void OdamexGameClientRunner::addPassword()
{
	// Odamex specifies the connect password as part of the connect command
	// so we should no-op this function.
}

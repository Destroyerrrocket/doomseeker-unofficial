//------------------------------------------------------------------------------
// gamerunnerstructs.h
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
#ifndef __GAME_RUNNER_STRUCTS_H_
#define __GAME_RUNNER_STRUCTS_H_

#include "global.h"
#include "serverstructs.h"
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>

/**
 * @brief Host launch information for Server class.
 *
 * Create Server dialog uses this to setup host information.
 * However things that can be set through the Server class,
 * like MOTD, max. clients, max. players, server name, etc. should
 * be set through Server class' setters.
 */
struct MAIN_EXPORT HostInfo
{
	QString executablePath; /// if empty, serverBinary() will be used
	QString demoPath; /// Use if running in DEMO mode.
	QString iwadPath;
	QStringList pwadsPaths;
	QStringList customParameters;
	DMFlags dmFlags;

	/**
	 * @brief Contents of this list will be passed as "+consoleCommand value"
	 *        to the command line.
	 */
	QList<GameCVar> cvars;

	~HostInfo()
	{
		foreach(DMFlagsSection* sec, dmFlags)
			delete sec;
	}
};

#endif

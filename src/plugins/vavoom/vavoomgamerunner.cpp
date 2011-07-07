//------------------------------------------------------------------------------
// vavoomgamerunner.cpp
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
#include "main.h"
#include "vavoomgamerunner.h"
#include "vavoomgameinfo.h"
#include "vavoommain.h"
#include "vavoomserver.h"

VavoomGameRunner::VavoomGameRunner(const VavoomServer* server)
: GameRunner(server)
{
}

bool VavoomGameRunner::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword)
{
	if(!GameRunner::connectParameters(args, pf, iwadFound, connectPassword))
		return false;

	// Remove original -iwad command
	int iwadArg = args.indexOf("-iwad");
	args.removeAt(iwadArg);
	args.removeAt(iwadArg);

	// What an odd thing to have to do "-iwaddir /path/to/iwads/ -doom2"
	QString iwad = server->iwadName();
	QString iwadLocation = pf.findFile(iwad.toLower());
	QString iwadDir = iwadLocation.left(iwadLocation.length() - iwad.length());
	QString iwadParam = iwadLocation.mid(iwadDir.length());
	iwadParam.truncate(iwadParam.indexOf(QChar('.')));
	args << "-iwaddir";
	args << iwadDir;
	args << ("-" + iwadParam);
	return true;
}

const EnginePlugin* VavoomGameRunner::plugin() const
{
	return VavoomMain::get();
}

//------------------------------------------------------------------------------
// chocolatedoombinaries.cpp
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
#include "chocolate-doom/chocolatedoomgamerunner.h"
#include "chocolate-doom/chocolatedoomserver.h"
#include "gui/createserver.h"
#include "serverapi/playerslist.h"
#include "main.h"

ChocolateDoomGameRunner::ChocolateDoomGameRunner(const ChocolateDoomServer* server)
: GameRunner(server)
{
}

bool ChocolateDoomGameRunner::connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound, const QString &connectPassword)
{
	if(server->playersList()->size() > 0)
		return GameRunner::connectParameters(args, pf, iwadFound, connectPassword);
	else
	{
		QString tmp;
		CreateServerDlg *csd = new CreateServerDlg();
		csd->makeSetupServerDialog(plugin());
		if(csd->exec() == QDialog::Accepted)
		{
			csd->commandLineArguments(tmp, args);
			return GameRunner::connectParameters(args, pf, iwadFound, connectPassword);
		}
		else
			return false;
	}
	return true;
}

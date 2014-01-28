//------------------------------------------------------------------------------
// chocolatedoomgamerunner.cpp
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
#include "chocolatedoomgameinfo.h"
#include "chocolatedoomgamerunner.h"
#include "chocolatedoomserver.h"
#include "gui/createserverdialog.h"
#include "serverapi/playerslist.h"
#include "main.h"

ChocolateDoomGameClientRunner::ChocolateDoomGameClientRunner(ChocolateDoomServer* server)
: GameClientRunner(server)
{
	this->server = server;
}

void ChocolateDoomGameClientRunner::createCommandLineArguments()
{
	if(server->players()->size() > 0)
	{
		GameClientRunner::createCommandLineArguments();
	}
	else
	{
		QString tmp;
		CreateServerDialog *csd = new CreateServerDialog();
		csd->setAttribute(Qt::WA_DeleteOnClose, false);
		csd->makeSetupServerDialog(plugin());
		if(csd->exec() == QDialog::Accepted)
		{
			csd->commandLineArguments(tmp, args());
			delete csd;
			GameClientRunner::createCommandLineArguments();
		}
		else
		{
			delete csd;
			setJoinError(JoinError(JoinError::Terminate));
		}
	}
}

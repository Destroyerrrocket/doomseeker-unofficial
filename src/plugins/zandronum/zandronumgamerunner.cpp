//------------------------------------------------------------------------------
// zandronumgamerunner.cpp
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "main.h"
#include "zandronumengineplugin.h"
#include "zandronumgamerunner.h"
#include "zandronumgameinfo.h"
#include "zandronumserver.h"

ZandronumGameClientRunner::ZandronumGameClientRunner(ZandronumServer* server)
: GameClientRunner(server)
{
	this->server = server;
	setArgForConnectPassword("+cl_password");
}

void ZandronumGameClientRunner::addExtra()
{
	IniSection& config = *ZandronumEnginePlugin::staticInstance()->data()->pConfig;
	bool bAllowCountryDisplay = config["AllowServersToDisplayMyCountry"];
	args() << "+cl_hidecountry" << QString::number(!bAllowCountryDisplay ? 1 : 0);
}

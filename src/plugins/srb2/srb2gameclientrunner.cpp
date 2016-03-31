//------------------------------------------------------------------------------
// srb2gameclientrunner.cpp
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
// Copyright (C) 2016 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "srb2gameclientrunner.h"

#include <datapaths.h>
#include <serverapi/serverstructs.h>
#include "srb2server.h"

Srb2GameClientRunner::Srb2GameClientRunner(QSharedPointer<Srb2Server> server)
: GameClientRunner(server)
{
	this->server = server;
	setArgForDemoRecord("-record");
	setArgForConnectPassword("-password");
	set_addModFiles(&Srb2GameClientRunner::addModFiles_prefixOnce);
	set_addExtra(&Srb2GameClientRunner::addExtra);
}

void Srb2GameClientRunner::addExtra()
{
}

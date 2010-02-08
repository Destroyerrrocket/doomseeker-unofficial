//------------------------------------------------------------------------------
// tooltipgenerator.cpp
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
#include "tooltipgenerator.h"
#include "main.h"
#include "server.h"
#include "tooltips/gameinfotip.h"
#include "tooltips/generalinfotip.h"
#include "tooltips/playertable.h"

TooltipGenerator::TooltipGenerator(const Server* server)
: pServer(server)
{
}

QString TooltipGenerator::gameInfoTableHTML()
{
	GameInfoTip tip(pServer);
	return tip.generateHTML();
}

QString TooltipGenerator::generalInfoHTML()
{
	GeneralInfoTip tip(pServer);
	return tip.generateHTML();
}

QString TooltipGenerator::playerTableHTML()
{
	PlayerTable table(pServer);
	return table.generateHTML();
}

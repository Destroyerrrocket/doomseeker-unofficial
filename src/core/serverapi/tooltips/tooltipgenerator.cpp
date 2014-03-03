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

#include "serverapi/tooltips/gameinfotip.h"
#include "serverapi/tooltips/generalinfotip.h"
#include "serverapi/tooltips/playertable.h"
#include "serverapi/server.h"

class TooltipGenerator::PrivData
{
	public:
		ServerCPtr server;
};

TooltipGenerator::TooltipGenerator(const ServerCPtr &server)
{
	d = new PrivData();
	d->server = server;
}

TooltipGenerator::~TooltipGenerator()
{
	delete d;
}

QString TooltipGenerator::gameInfoTableHTML()
{
	GameInfoTip tip(server());
	return tip.generateHTML();
}

QString TooltipGenerator::generalInfoHTML()
{
	GeneralInfoTip tip(server());
	return tip.generateHTML();
}

QString TooltipGenerator::playerTableHTML()
{
	PlayerTable table(server());
	return table.generateHTML();
}

ServerCPtr TooltipGenerator::server() const
{
	return d->server;
}

//------------------------------------------------------------------------------
// queryspeed.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "queryspeed.h"

QuerySpeed QuerySpeed::cautious()
{
	QuerySpeed result;
	result.attemptsPerServer = 3;
	result.delayBetweenSingleServerAttempts = 3500;
	result.intervalBetweenServers = 60;
	return result;
}

QuerySpeed QuerySpeed::moderate()
{
	QuerySpeed result;
	result.attemptsPerServer = 3;
	result.delayBetweenSingleServerAttempts = 3000;
	result.intervalBetweenServers = 30;
	return result;
}

QuerySpeed QuerySpeed::aggressive()
{
	QuerySpeed result;
	result.attemptsPerServer = 3;
	result.delayBetweenSingleServerAttempts = 2000;
	result.intervalBetweenServers = 10;
	return result;
}
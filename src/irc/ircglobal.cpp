//------------------------------------------------------------------------------
// ircglobal.cpp
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
#include "ircglobal.h"

#include <QChar>

QString IRCGlobal::COLOR_CHANNEL_ACTION = "#38A800";
QString IRCGlobal::COLOR_NETWORK_ACTION = "#1119FF";

bool IRCGlobal::isChannelDenotingCharacter(char character)
{
	return (character == '&' || character == '#');
}

bool IRCGlobal::isChannelName(const QString& name)
{
	if (name.isEmpty())
	{
		return false;
	}

	QChar c = name[0];
	return isChannelDenotingCharacter(c.toAscii());
}

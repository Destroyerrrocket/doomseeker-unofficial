//------------------------------------------------------------------------------
// ircuserprefix.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ircuserprefix.h"

#include <QMap>

class IRCUserPrefix::PrivData
{
	public:
		QMap<char, char> map;
};

IRCUserPrefix::IRCUserPrefix()
{
	d = new PrivData();
}

IRCUserPrefix::IRCUserPrefix(const IRCUserPrefix &other)
{
	d = new PrivData();
	*d = *other.d;
}

IRCUserPrefix::~IRCUserPrefix()
{
	delete d;
}

IRCUserPrefix &IRCUserPrefix::operator=(const IRCUserPrefix &other)
{
	if (this != &other)
	{
		*d = *other.d;
	}
	return *this;
}

void IRCUserPrefix::assignPrefix(char mode, char prefix)
{
	d->map[mode] = prefix;
}

IRCUserPrefix IRCUserPrefix::mkDefault()
{
	IRCUserPrefix obj;
	obj.assignPrefix('o', '@');
	obj.assignPrefix('h', '%');
	obj.assignPrefix('v', '+');
	return obj;
}

char IRCUserPrefix::modeForPrefix(char prefix) const
{
	foreach (char mode, d->map.keys())
	{
		if (d->map[mode] == prefix)
		{
			return mode;
		}
	}
	return 0;
}

char IRCUserPrefix::prefixForMode(char mode) const
{
	return d->map[mode];
}

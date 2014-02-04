//------------------------------------------------------------------------------
// serverstructs.cpp
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
#include "serverstructs.h"

class PWad::PrivData
{
	public:
		QString name;
		bool optional;
};

COPYABLE_D_POINTERED_DEFINE(PWad);

PWad::PWad(const QString &name, bool optional)
{
	d = new PrivData();
	d->name = name;
	d->optional = optional;
}

PWad::~PWad()
{
	delete d;
}

bool PWad::isOptional() const
{
	return d->optional;
}

const QString& PWad::name() const
{
	return d->name;
}


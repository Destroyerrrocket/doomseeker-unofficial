//------------------------------------------------------------------------------
// oldinivariable.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "oldinivariable.h"

#include <cassert>

OldIniVariable::operator bool() const
{
	return value.toInt() != 0;
}

OldIniVariable::operator int() const
{
	return value.toInt();
}

OldIniVariable::operator unsigned int() const
{
	return value.toUInt();
}

OldIniVariable::operator short() const
{
	return value.toShort();
}

OldIniVariable::operator unsigned short() const
{
	return value.toUShort();
}

OldIniVariable::operator float() const
{
	return value.toFloat();
}

const OldIniVariable &OldIniVariable::operator=(const QString &str)
{
	assert(!isNull());

	value = str;
	return *this;
}

const OldIniVariable &OldIniVariable::operator=(int i)
{
	assert(!isNull());

	value = QString("%1").arg(i);
	return *this;
}

const OldIniVariable &OldIniVariable::operator=(unsigned int i)
{
	assert(!isNull());

	value = QString("%1").arg(i);
	return *this;
}

const OldIniVariable &OldIniVariable::operator=(short i)
{
	assert(!isNull());

	value = QString("%1").arg(i);
	return *this;
}

const OldIniVariable &OldIniVariable::operator=(unsigned short i)
{
	assert(!isNull());

	value = QString("%1").arg(i);
	return *this;
}

const OldIniVariable &OldIniVariable::operator=(bool b)
{
	return *this = static_cast<int>(b);
}

const OldIniVariable &OldIniVariable::operator=(float f)
{
	assert(!isNull());

	value = QString("%1").arg(f);
	return *this;
}

const OldIniVariable &OldIniVariable::operator=(const OldIniVariable &other)
{
	assert(!isNull());

	null = other.null;
	sideComment = other.sideComment;
	topComment = other.topComment;
	key = other.key;
	value = other.value;
	return *this;
}

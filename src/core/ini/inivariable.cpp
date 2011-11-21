//------------------------------------------------------------------------------
// inivariable.cpp
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
#include "inivariable.h"

#include "ini/inisection.h"

#include <cassert>

IniVariable::IniVariable()
{
	this->pConstSection = NULL;
	this->pSection = NULL;
}

IniVariable::IniVariable(IniSection* pSection, const QString& key)
{
	this->pConstSection = pSection;
	this->pSection = pSection;
	this->key = key;
}

IniVariable::IniVariable(const IniSection* pSection, const QString& key)
{
	this->pConstSection = pSection;
	this->pSection = NULL;
	this->key = key;
}

IniVariable::operator bool() const
{
	return value().toInt() != 0;
}

IniVariable::operator int() const
{
	return value().toInt();
}

IniVariable::operator unsigned int() const
{
	return value().toUInt();
}

IniVariable::operator short() const
{
	// It's a mystery why QVariant can't convert to short directly.
	return value().toString().toShort();
}

IniVariable::operator unsigned short() const
{
	// It's a mystery why QVariant can't convert to u-short directly.
	return value().toString().toUShort();
}

IniVariable::operator float() const
{
	return value().toFloat();
}

const IniVariable &IniVariable::operator=(const QString &str)
{
	if (isNull())
	{
		bool breakpoint = true;
	}
	assert(!isNull());

	setValue(str);
	return *this;
}

const IniVariable &IniVariable::operator=(int i)
{
	assert(!isNull());

	setValue(i);
	return *this;
}

const IniVariable &IniVariable::operator=(unsigned int i)
{
	assert(!isNull());

	setValue(i);
	return *this;
}

const IniVariable &IniVariable::operator=(short i)
{
	assert(!isNull());

	setValue(i);
	return *this;
}

const IniVariable &IniVariable::operator=(unsigned short i)
{
	assert(!isNull());

	setValue(i);
	return *this;
}

const IniVariable &IniVariable::operator=(bool b)
{
	return *this = static_cast<int>(b);
}

const IniVariable &IniVariable::operator=(float f)
{
	assert(!isNull());

	setValue(f);
	return *this;
}

const IniVariable &IniVariable::operator=(const IniVariable &other)
{
	assert(!isNull());

	key = other.key;
	pConstSection = other.pConstSection;
	pSection = other.pSection;
	return *this;
}

void IniVariable::setValue(const QVariant& value)
{
	assert(!isNull());
	assert(pSection != NULL);

	if (pSection != NULL)
	{
		pSection->setValue(key, value);
	}
}

QVariant IniVariable::value() const
{
	if (pConstSection != NULL)
	{
		return pConstSection->value(key);
	}

	return QVariant();
}

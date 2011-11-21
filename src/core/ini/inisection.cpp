//------------------------------------------------------------------------------
// inisection.cpp
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
#include "inisection.h"

#include "ini.h"
#include <cassert>
#include <QDebug>

IniSection::IniSection()
{
	d.pIni = NULL;
}

IniSection::IniSection(Ini* pIni, const QString& sectionName)
{
	d.pIni = pIni;
	d.name = sectionName;
}

IniVariable IniSection::createSetting(const QString& name, const QVariant& data)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return IniVariable();
	}

	if (value(name).isNull())
	{
		setValue(name, data);
	}

	return retrieveSetting(name);
}

void IniSection::deleteSetting(const QString& name)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return;
	}

	remove(name);
}

IniVariable IniSection::operator[](const QString& name)
{
	return setting(name);
}

const IniVariable IniSection::operator[](const QString& name) const
{
	return retrieveSetting(name);
}

void IniSection::remove(const QString& key)
{
	d.pIni->removeKey(d.name + "/" + key);
}

IniVariable IniSection::retrieveSetting(const QString& name)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return IniVariable();
	}

	return IniVariable(this, name);
}

const IniVariable IniSection::retrieveSetting(const QString& name) const
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return IniVariable();
	}

	return IniVariable(this, name);
}

IniVariable IniSection::setting(const QString& name)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return IniVariable();
	}

	IniVariable pVariable = retrieveSetting(name);
	if (pVariable.isNull())
	{
		return createSetting(name, QVariant());
	}

	return pVariable;
}

void IniSection::setValue(const QString& key, const QVariant& value)
{
	assert(!isNull());

	if (!isNull())
	{
		d.pIni->setValue(d.name + "/" + key, value);
	}
}

QVariant IniSection::value(const QString& key) const
{
	if (!isNull())
	{
		return d.pIni->value(d.name + "/" + key);
	}

	return QVariant();
}

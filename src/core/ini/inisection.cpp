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
#include <cassert>

IniVariable IniSection::nullVariable((const IniVariable &)IniVariable::makeNull());

IniVariable &IniSection::createSetting(const QString& name, const IniVariable& data)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return nullVariable;
	}

	QString nameLower = name.toLower();

	IniVariablesIt it = variables.find(nameLower);
	if (it != variables.end())
	{
		return *it;
	}

	// Avoid setting a Null variable.
	IniVariable varData;
	varData.key = name;
	varData = data;

	variables.insert(nameLower, varData);
	IniVariable &pNewVariable = *variables.find(nameLower);
	pNewVariable.key = name;

	return pNewVariable;
}

void IniSection::deleteSetting(const QString& name)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return;
	}

	QString nameLower = name.toLower();

	IniVariablesIt it = variables.find(nameLower);
	if (it != variables.end())
	{
		variables.erase(it);
	}
}

IniVariable &IniSection::retrieveSetting(const QString& name)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return nullVariable;
	}

	QString nameLower = name.toLower();

	IniVariablesIt it = variables.find(nameLower);
	if (it == variables.end())
	{
		return nullVariable;
	}

	return *it;
}
const IniVariable &IniSection::retrieveSetting(const QString& name) const
{
	if (name.isEmpty())
	{
		return nullVariable;
	}

	QString nameLower = name.toLower();

	IniVariablesConstIt it = variables.find(nameLower);
	if (it == variables.end())
	{
		return nullVariable;
	}

	return *it;
}

IniVariable &IniSection::setting(const QString& name)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return nullVariable;
	}

	QString nameLower = name.toLower();

	IniVariable& pVariable = retrieveSetting(nameLower);
	if (pVariable.isNull())
		return createSetting(name, IniVariable());
	return pVariable;
}

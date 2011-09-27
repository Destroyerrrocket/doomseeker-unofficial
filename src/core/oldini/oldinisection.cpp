//------------------------------------------------------------------------------
// oldinisection.cpp
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
#include "oldinisection.h"
#include <cassert>

OldIniVariable OldIniSection::nullVariable((const OldIniVariable &)OldIniVariable::makeNull());

OldIniVariable &OldIniSection::createSetting(const QString& name, const OldIniVariable& data)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return nullVariable;
	}

	QString nameLower = name.toLower();

	OldIniVariablesIt it = variables.find(nameLower);
	if (it != variables.end())
	{
		return *it;
	}

	// Avoid setting a Null variable.
	OldIniVariable varData;
	varData.key = name;
	varData = data;

	variables.insert(nameLower, varData);
	OldIniVariable &pNewVariable = *variables.find(nameLower);
	pNewVariable.key = name;

	return pNewVariable;
}

void OldIniSection::deleteSetting(const QString& name)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return;
	}

	QString nameLower = name.toLower();

	OldIniVariablesIt it = variables.find(nameLower);
	if (it != variables.end())
	{
		variables.erase(it);
	}
}

OldIniVariable &OldIniSection::retrieveSetting(const QString& name)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return nullVariable;
	}

	QString nameLower = name.toLower();

	OldIniVariablesIt it = variables.find(nameLower);
	if (it == variables.end())
	{
		return nullVariable;
	}

	return *it;
}
const OldIniVariable &OldIniSection::retrieveSetting(const QString& name) const
{
	if (name.isEmpty())
	{
		return nullVariable;
	}

	QString nameLower = name.toLower();

	OldIniVariablesConstIt it = variables.find(nameLower);
	if (it == variables.end())
	{
		return nullVariable;
	}

	return *it;
}

OldIniVariable &OldIniSection::setting(const QString& name)
{
	assert(!isNull());
	if (name.isEmpty())
	{
		return nullVariable;
	}

	QString nameLower = name.toLower();

	OldIniVariable& pVariable = retrieveSetting(nameLower);
	if (pVariable.isNull())
		return createSetting(name, OldIniVariable());
	return pVariable;
}

const OldIniVariables& OldIniSection::settings() const
{
	return variables;
}

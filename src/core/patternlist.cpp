//------------------------------------------------------------------------------
// patternlist.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "patternlist.h"

#include <QDebug>

bool PatternList::isExactMatchAny(const QString &candidate) const
{
	foreach (const QRegExp &matcher, *this)
	{
		if (matcher.exactMatch(candidate))
		{
			return true;
		}
	}
	return false;
}

PatternList PatternList::deserializeQVariant(const QVariant &var)
{
	PatternList result;
	foreach (const QVariant &element, var.toList())
	{
		result << element.toRegExp();
	}
	return result;
}

QVariant PatternList::serializeQVariant() const
{
	QVariantList var;
	foreach (const QRegExp &pattern, *this)
	{
		var << pattern;
	}
	return var;
}

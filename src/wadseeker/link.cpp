//------------------------------------------------------------------------------
// link.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "link.h"
#include <QString>
#include <QStringList>

bool Link::pathEndsWith(const QStringList& ends)
{
	QString str = url.encodedPath();
	QStringList::const_iterator it;
	for (it = ends.begin(); it != ends.end(); ++it)
	{
		if (str.endsWith(*it, Qt::CaseInsensitive))
		{
			return true;
		}
	}

	return false;
}

/**
 *	@param comparePage		- if not empty checks if URL refers to the same host as this param
 *	@return true if URL points to another server
 */
bool Link::isRemote(const QUrl& comparePage)
{
	QString str1 = url.encodedHost();
	QString str2 = comparePage.encodedHost();

	if (str1.isEmpty())
	{
		return false;
	}

	if (!comparePage.isEmpty())
	{
		if (str1.compare(str2, Qt::CaseInsensitive) != 0)
		{
			return true;
		}
	}
	else
	{
		return true;
	}

	return false;
}

/**
 *	@return true if the URL refers to the same page (for example URLs with '#')
 */
bool Link::isTheSamePage(const QUrl& comparePage)
{
	QString str1 = url.encodedHost();
	QString str2 = comparePage.encodedHost();

	if (!str1.isEmpty() && str1.compare(str2, Qt::CaseInsensitive) != 0)
	{
		return false;
	}

	str1 = url.encodedQuery();
	str2 = comparePage.encodedQuery();
	QString str3 = url.encodedPath();
	QString str4 = comparePage.encodedPath();

	if (str1.compare(str2, Qt::CaseInsensitive) == 0
		&& str3.compare(str4, Qt::CaseInsensitive) == 0)
	{
		return true;
	}

	return false;
}

/**
 * @return true if URL begins from javascript: phrase
 */
bool Link::isJavascriptURL()
{
	return url.toString().startsWith("javascript:", Qt::CaseInsensitive);
}


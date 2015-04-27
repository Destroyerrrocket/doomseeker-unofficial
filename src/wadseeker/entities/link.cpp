//------------------------------------------------------------------------------
// link.cpp
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
#include "link.h"
#include <QString>
#include <QStringList>

Link::Link()
{

}

Link::Link(const QUrl& url, const QString& text)
{
	this->url = url;
	this->text = text;
}

bool Link::isRemote(const QUrl& comparePage)
{
#if QT_VERSION >= 0x050000
	QString str1 = url.host(QUrl::FullyEncoded);
	QString str2 = comparePage.host(QUrl::FullyEncoded);
#else
	QString str1 = url.encodedHost();
	QString str2 = comparePage.encodedHost();
#endif

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

bool Link::isTheSamePage(const QUrl& comparePage)
{
#if QT_VERSION >= 0x050000
	QString str1 = url.host(QUrl::FullyEncoded);
	QString str2 = comparePage.host(QUrl::FullyEncoded);
#else
	QString str1 = url.encodedHost();
	QString str2 = comparePage.encodedHost();
#endif

	if (!str1.isEmpty() && str1.compare(str2, Qt::CaseInsensitive) != 0)
	{
		return false;
	}

#if QT_VERSION >= 0x050000
	str1 = url.query(QUrl::FullyEncoded);
	str2 = comparePage.query(QUrl::FullyEncoded);
	QString str3 = url.path(QUrl::FullyEncoded);
	QString str4 = comparePage.path(QUrl::FullyEncoded);
#else
	str1 = url.encodedQuery();
	str2 = comparePage.encodedQuery();
	QString str3 = url.encodedPath();
	QString str4 = comparePage.encodedPath();
#endif

	if (str1.compare(str2, Qt::CaseInsensitive) == 0
		&& str3.compare(str4, Qt::CaseInsensitive) == 0)
	{
		return true;
	}

	return false;
}

bool Link::isJavascriptURL()
{
	return url.toString().startsWith("javascript:", Qt::CaseInsensitive);
}

bool Link::pathEndsWith(const QStringList& ends)
{
#if QT_VERSION >= 0x050000
	QString str = url.path(QUrl::FullyEncoded);
#else
	QString str = url.encodedPath();
#endif
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

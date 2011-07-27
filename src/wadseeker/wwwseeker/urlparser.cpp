//------------------------------------------------------------------------------
// urlparser.cpp
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
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "urlparser.h"

#include <QFileInfo>

#include "entities/link.h"

UrlParser::UrlParser(const QList<Link>& links)
{
	d.links = links;
}

QList<Link> UrlParser::directLinks(const QString& wantedFilename, const QUrl& baseUrl)
{
	QList<Link> linksList;

	foreach (Link link, d.links)
	{
		if (isDirectLinkToFile(wantedFilename, link))
		{
			if (link.url.isRelative())
			{
				link.url = baseUrl.resolved(link.url);
			}

			linksList << link;
		}
	}

	return linksList;
}


bool UrlParser::hasFileReferenceSomewhere(const QString& wantedFilename, const Link& link)
{
	QString strQuery = link.url.encodedQuery();

	return strQuery.contains(wantedFilename, Qt::CaseInsensitive)
		|| link.text.contains(wantedFilename, Qt::CaseInsensitive);
}

bool UrlParser::isDirectLinkToFile(const QString& wantedFilename, const Link& link)
{
	QFileInfo fi(link.url.encodedPath());
	return (fi.fileName().compare(wantedFilename, Qt::CaseInsensitive) == 0);
}

QList<Link> UrlParser::siteLinks(const QString& wantedFilename, const QUrl& baseUrl)
{
	QList<Link> linksList;

	foreach (Link link, d.links)
	{
		// Make sure direct links are not listed here.
		if (hasFileReferenceSomewhere(wantedFilename, link)
			&& !isDirectLinkToFile(wantedFilename, link))
		{
			if (link.url.isRelative())
			{
				link.url = baseUrl.resolved(link.url);
			}

			linksList << link;
		}
	}

	return linksList;
}

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

#include "link.h"

UrlParser::UrlParser(const QList<Link>& links)
{
	d.links = links;
}

QUrl UrlParser::createAbsoluteUrl(const QUrl& relativeUrl, const QUrl& baseUrl)
{
	// Create a new URL from the processed link.
	QUrl newUrl = relativeUrl;
	if (relativeUrl.authority().isEmpty())
	{
		// If the authority is not defined in the URL
		// we revert to the default http protocol.
		newUrl.setAuthority("http");
	}

	if (relativeUrl.host().isEmpty())
	{
		if (baseUrl.host().isEmpty())
		{
			// Cannot define host. This URL will be useless.
			return QUrl();
		}

		newUrl.setHost(baseUrl.host());
	}

	if (relativeUrl.host().isEmpty() && relativeUrl.port() < 0
		&& baseUrl.port() >= 0)
	{
		// Port to the new URL will only be appended if the URL was
		// not absolute in the first place. If URL was absolute it is
		// most likely not a very good idea to tamper with the port.
		newUrl.setPort(baseUrl.port());
	}

	// If the path in the processed url starts with '/' do not make any
	// changes to the path in the currently created URL.
	if (relativeUrl.host().isEmpty() && relativeUrl.authority().isEmpty()
		&& !relativeUrl.path().startsWith('/'))
	{
		QString path = baseUrl.path();
		path = path.left(path.lastIndexOf('/') + 1);
		path += relativeUrl.path();
		newUrl.setPath(path);
	}

	return newUrl;
}

QList<Link> UrlParser::directLinks(const QStringList& wantedFiles, const QUrl& baseUrl)
{
	QList<Link> linksList;

	foreach (const Link& link, d.links)
	{
		if (isDirectLinkToFile(wantedFiles, link))
		{
			QUrl absoluteUrl = createAbsoluteUrl(link.url, baseUrl);
			if (absoluteUrl.isValid())
			{
				Link absoluteLink = link;
				absoluteLink.url = absoluteUrl;
				linksList << absoluteLink;
			}
		}
	}

	return linksList;
}


bool UrlParser::hasFileReferenceSomewhere(const QStringList& wantedFileNames, const Link& link)
{
	QString strQuery = link.url.encodedQuery();

	for (int i = 0; i < wantedFileNames.count(); ++i)
	{
		if (strQuery.contains(wantedFileNames[i], Qt::CaseInsensitive) || link.text.contains(wantedFileNames[i], Qt::CaseInsensitive) )
		{
			return true;
		}
	}

	return false;
}

bool UrlParser::isDirectLinkToFile(const QStringList& wantedFileNames, const Link& link)
{
	QFileInfo fi(link.url.encodedPath());
	for (int i = 0; i < wantedFileNames.count(); ++i)
	{
		if (fi.fileName().compare(wantedFileNames[i], Qt::CaseInsensitive) == 0)
		{
			return true;
		}
	}

	return false;
}

QList<Link> UrlParser::siteLinks(const QStringList& wantedFiles, const QUrl& baseUrl)
{
	QList<Link> linksList;

	foreach (const Link& link, d.links)
	{
		if (hasFileReferenceSomewhere(wantedFiles, link))
		{
			QUrl absoluteUrl = createAbsoluteUrl(link.url, baseUrl);
			if (absoluteUrl.isValid())
			{
				Link absoluteLink = link;
				absoluteLink.url = absoluteUrl;
				linksList << absoluteLink;
			}
		}
	}
}

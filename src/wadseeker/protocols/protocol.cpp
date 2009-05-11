//------------------------------------------------------------------------------
// protocol.cpp
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
#include "protocol.h"
#include <QDebug>
#include <QFileInfo>

Protocol::Protocol()
{
	port = 0;
}

void Protocol::get(const QUrl& url)
{
	QString scheme = url.scheme();
	if (scheme.isEmpty())
	{
		scheme = this->defaultScheme();
	}
	this->site.setScheme(scheme);
	if (!url.authority().isEmpty())
	{
		this->site.setAuthority(url.authority());
	}

	this->resource = url.encodedPath();
	if (this->resource.isEmpty())
	{
		this->resource = '/';
	}
	else if (this->resource[0] != '/')
	{
		this->resource.prepend('/');
	}

	if (!url.encodedQuery().isNull())
	{
		resource += "?" + url.encodedQuery();
	}

	QFileInfo fi(url.encodedPath());
	emit nameOfCurrentlyDownloadedResource(fi.fileName());

	data.clear();
	sendGet();
}

QUrl Protocol::lastLink() const
{
	return QUrl(site.toString() + resource);
}

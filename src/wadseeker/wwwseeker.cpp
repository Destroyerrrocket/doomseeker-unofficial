//------------------------------------------------------------------------------
// wwwseeker.cpp
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
#include "wwwseeker.h"

#include "htmlparser.h"

#include <QFileInfo>

WWWSeeker::WWWSeeker()
{
	d.bIsWorking = false;
	d.pNetworkAccessManager = new QNetworkAccessManager();
}

WWWSeeker::~WWWSeeker()
{
	if (d.pCurrentNetworkReply != NULL)
	{
		delete d.pCurrentNetworkReply;
	}

	delete d.pNetworkAccessManager;
}

void WWWSeeker::abort()
{
	if (d.pCurrentNetworkReply != NULL)
	{
		d.pCurrentNetworkReply->abort();
	}
}

void WWWSeeker::addNetworkReply(QNetworkReply* pReply);
{
	NetworkQueryInfo* pQueryInfo = new NetworkQueryInfo(pReply);



	d.networkQueries << pQueryInfo;
}

void WWWSeeker::addSiteUrl(const QUrl& url)
{
	if (!wasUrlUsed(url))
	{
		d.sitesUrls << url;
	}
}

void WWWSeeker::addSitesUrls(const QList<QUrl>& urlsList)
{
	foreach (const QUrl& url, urlsList)
	{
		addSiteUrl(url);
	}
}

void WWWSeeker::currentReplyFinished()
{

}

NetworkQueryInfo* WWWSeeker::findNetworkQueryInfo(QNetworkReply* pReply)
{
	foreach (NetworkQueryInfo* info, d.networkQueries)
	{
		if (*info == pReply)
		{
			return info;
		}
	}

	return NULL;
}

void WWWSeeker::setUserAgent(const QString& userAgent)
{
	d.userAgent = userAgent;
}

void WWWSeeker::startSearch(const QStringList& seekedFilenames)
{
	if (isWorking())
	{
		return;
	}

	d.bIsWorking = true;

	d.seekedFilenames = seekedFilenames;
}

const QString& WWWSeeker::userAgent() const
{
	d.userAgent;
}

bool WWWSeeker::wasUrlUsed(const QUrl& url) const
{
	foreach (const QUrl& usedUrl, d.usedUrls)
	{
		if (usedUrl == url)
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////

WWWSeeker::NetworkQueryInfo::NetworkQueryInfo(QNetworkReply* pReply)
{
	this->pReply = pReply;

	if (pReply != NULL)
	{
		pSignalWrapper = new NetworkReplySignalWrapper(pReply);
	}
	else
	{
		pSignalWrapper = NULL;
	}
}

NetworkQueryInfo::~NetworkQueryInfo()
{
	if (pReply != NULL)
	{
		delete pSignalWrapper;
		delete pReply;
	}
}

void WWWSeeker::NetworkQueryInfo::deleteMembersLater()
{
	if (pReply != NULL)
	{
		delete pSignalWrapper;
		pReply->deleteLater();

		pReply = NULL;
	}
}

bool WWWSeeker::NetworkQueryInfo::operator==(const NetworkQueryInfo& other) const
{
	return *this == other.pReply;
}

bool WWWSeeker::NetworkQueryInfo::operator==(const QNetworkReply* pReply) const
{
	return this->pReply == other.pReply;
}

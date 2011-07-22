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
#include "networkreplysignalwrapper.h"

#include <QFileInfo>

WWWSeeker::WWWSeeker()
{
	d.bIsAborting = false;
	d.bIsWorking = false;
	d.maxConcurrentSiteDownloads = 3;
	d.pNetworkAccessManager = new QNetworkAccessManager();
}

WWWSeeker::~WWWSeeker()
{
	foreach (NetworkQueryInfo* pInfo, d.networkQueries)
	{
		pInfo->deleteMembersLater();
		delete pInfo;
	}

	delete d.pNetworkAccessManager;
}

void WWWSeeker::abort()
{
	d.bIsAborting = true;

	if (d.networkQueries.isEmpty())
	{
		emit finished();
	}
	else
	{
		foreach (NetworkQueryInfo* pInfo, d.networkQueries)
		{
			pInfo->pReply->abort();
		}

		d.sitesUrls.clear();
	}
}

void WWWSeeker::addNetworkReply(QNetworkReply* pReply)
{
	NetworkQueryInfo* pQueryInfo = new NetworkQueryInfo(pReply);

	this->connect(pQueryInfo->pSignalWrapper, SIGNAL( finished(QNetworkReply*) ),
		SLOT( networkQueryFinished(QNetworkReply*) ));

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

void WWWSeeker::networkQueryFinished(QNetworkReply* pReply)
{
	NetworkQueryInfo* pQueryInfo = findNetworkQueryInfo(pReply);

	// TODO
	// Get URLs from downloaded site.

	QUrl url = pReply->url();
	printf("Finished URL %s\n", pReply->url().toEncoded().constData());

	pQueryInfo->deleteMembersLater();
	d.networkQueries.removeOne(pQueryInfo);
	delete pQueryInfo;

	emit siteFinished(url, 0, 0);

	if (d.networkQueries.isEmpty() && d.sitesUrls.isEmpty())
	{
		// Work is finished if there are no more site URLs to find.
		d.bIsWorking = false;
		emit finished();
	}
	else
	{
		if (!d.bIsAborting)
		{
			startNextSites();
		}
	}
}

WWWSeeker::NetworkQueryInfo* WWWSeeker::findNetworkQueryInfo(QNetworkReply* pReply)
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

void WWWSeeker::startNextSites()
{
	while (d.networkQueries.size() < d.maxConcurrentSiteDownloads
			&& !d.sitesUrls.isEmpty())
	{
		QUrl url = d.sitesUrls.takeFirst();
		printf("Starting site: %s\n", url.toEncoded().constData());

		d.visitedUrls << url;

		QNetworkRequest request;
		request.setUrl(url);
		request.setRawHeader("User-Agent", d.userAgent.toAscii());

		QNetworkReply* pReply = d.pNetworkAccessManager->get(request);
		addNetworkReply(pReply);

		emit siteStarted(url);
	}
}

void WWWSeeker::startSearch(const QStringList& seekedFilenames)
{
	if (isWorking())
	{
		return;
	}

	d.bIsAborting = false;
	d.bIsWorking = true;

	d.seekedFilenames = seekedFilenames;
	d.visitedUrls.clear();

	startNextSites();
}

const QString& WWWSeeker::userAgent() const
{
	d.userAgent;
}

bool WWWSeeker::wasUrlUsed(const QUrl& url) const
{
	foreach (const QUrl& usedUrl, d.visitedUrls)
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

WWWSeeker::NetworkQueryInfo::~NetworkQueryInfo()
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
	return this->pReply == pReply;
}

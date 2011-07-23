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
#include "urlparser.h"

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

	d.fileSiteUrls.clear();
	d.sitesUrls.clear();
	d.seekedFilenames.clear();

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
		d.seekedFilenames.clear();
	}
}

void WWWSeeker::addFileSiteUrl(const QString& filename, const QUrl& url)
{
	if (!wasUrlUsed(url))
	{
		QString lowerCaseFilename = filename.toLower();

		if (d.fileSiteUrls.contains(lowerCaseFilename))
		{
			// Add to existing URL list.
			QList<QUrl>& urls = d.fileSiteUrls[lowerCaseFilename];
			urls << url;
		}
		else
		{
			// Create a new URL list.
			QList<QUrl> urls;
			urls << url;
			d.fileSiteUrls.insert(filename, urls);

			// Don't forget to update the key list with a new key.
			d.fileSiteKeyRotationList << lowerCaseFilename;
		}
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

void WWWSeeker::deleteNetworkQueryInfo(QNetworkReply* pReply)
{
	NetworkQueryInfo* pInfo = findNetworkQueryInfo(pReply);
	if (pInfo != NULL)
	{
		pInfo->deleteMembersLater();
		d.networkQueries.removeOne(pInfo);
	}
	else
	{
		pReply->deleteLater();
	}
}

bool WWWSeeker::isMoreToSearch() const
{
	if (d.seekedFilenames.isEmpty())
	{
		return false;
	}

	return !d.sitesUrls.isEmpty()
		|| !d.fileSiteUrls.isEmpty();
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

void WWWSeeker::networkQueryFinished(QNetworkReply* pReply)
{
	NetworkQueryInfo* pQueryInfo = findNetworkQueryInfo(pReply);
	QUrl url = pReply->request().url();

	QUrl possibleRedirectUrl = pReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!possibleRedirectUrl.isEmpty()
		&& possibleRedirectUrl != url)
	{
		// Redirect URL cannot be simply added to the site URLs as those URLs
		// will throw away all visited URLs. In at least one case a redirect
		// leads to the same page but with different attachments.
		// In this case let's start a new query omitting the entire limitation
		// system here.
		if (possibleRedirectUrl.isRelative())
		{
			possibleRedirectUrl = url.resolved(possibleRedirectUrl);
		}

		deleteNetworkQueryInfo(pReply);

		if (!d.bIsAborting)
		{
			emit siteRedirect(url, possibleRedirectUrl);
			startNetworkQuery(possibleRedirectUrl);
		}

		printf("Finished URL %s - redirect to %s\n", url.toEncoded().constData(), possibleRedirectUrl.toEncoded().constData());
	}
	else
	{
		// TODO
		// Get URLs from downloaded site.
		QByteArray downloadedData = pReply->readAll();

		// Get all <A HREFs> from HTML.
		HtmlParser html(downloadedData);
		QList<Link> links = html.linksFromHtml();

		// Extrat URLs of interest from <A HREFs>
		UrlParser urlParser(links);

		foreach (const QString& filename, d.seekedFilenames)
		{
			QList<Link> siteLinks = urlParser.siteLinks(filename, url);
			QList<Link> directLinks = urlParser.directLinks(filename, url);

			foreach (const Link& link, siteLinks)
			{
				addFileSiteUrl(filename, link.url);
			}

			foreach (const Link& link, directLinks)
			{

			}
		}

		printf("Finished URL %s - content type %s. Data size: %d\n", url.toEncoded().constData(),
			pReply->header(QNetworkRequest::ContentTypeHeader).toByteArray().constData(), downloadedData.size());

		deleteNetworkQueryInfo(pReply);
		emit siteFinished(url);
	}

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

void WWWSeeker::removeSeekedFilename(const QString& filename)
{
	// Make sure to remove all occurences of the filename.
	QStringList toRemove;

	foreach (const QString& nameOnList, d.seekedFilenames)
	{
		if (nameOnList.compare(filename, Qt::CaseInsensitive) == 0)
		{
			toRemove << nameOnList;
		}
	}

	foreach (const QString& nameToRemove, toRemove)
	{
		d.seekedFilenames.removeAll(nameToRemove);
	}

	if (d.seekedFilenames.isEmpty() && isWorking())
	{
		abort();
	}
}

void WWWSeeker::setUserAgent(const QString& userAgent)
{
	d.userAgent = userAgent;
}

void WWWSeeker::startNetworkQuery(const QUrl& url)
{
	QNetworkRequest request;
	request.setUrl(url);
	request.setRawHeader("User-Agent", d.userAgent.toAscii());

	QNetworkReply* pReply = d.pNetworkAccessManager->get(request);
	addNetworkReply(pReply);
}

void WWWSeeker::startNextSites()
{
	while (d.networkQueries.size() < d.maxConcurrentSiteDownloads
			&& isMoreToSearch())
	{
		QUrl url = takeNextUrl();

		if (url.isValid())
		{
			printf("Starting site: %s\n", url.toEncoded().constData());

			d.visitedUrls << url;
			startNetworkQuery(url);
			emit siteStarted(url);
		}
	}

	if (d.networkQueries.isEmpty() && !isMoreToSearch())
	{
		// No more sites to check, no more queries to wait for a reply.
		emit finished();
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

QUrl WWWSeeker::takeNextUrl()
{
	printf("d.fileSiteKeyRotationList: %s\n", d.fileSiteKeyRotationList.join(", ").toAscii().constData());
	// Try to take one of the sites that are suspected to contain at least
	// one of the seeked files.
	while (!d.fileSiteKeyRotationList.isEmpty())
	{
		QString filename = d.fileSiteKeyRotationList.takeFirst();

		if (d.fileSiteUrls.contains(filename))
		{
			QList<QUrl>& urls = d.fileSiteUrls[filename];
			if (!urls.isEmpty())
			{
				QUrl url = urls.takeFirst();
				if (urls.isEmpty())
				{
					// Remove now empty list from the hash map.
					d.fileSiteUrls.remove(filename);
				}

				return url;
			}
		}
	}

	// Refill the rotation list
	d.fileSiteKeyRotationList = d.fileSiteUrls.keys();

	// Take one of the generic sites.
	while (!d.sitesUrls.isEmpty())
	{
		QUrl url = d.sitesUrls.takeFirst();
		if (!wasUrlUsed(url))
		{
			return url;
		}
	}

	return QUrl();
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

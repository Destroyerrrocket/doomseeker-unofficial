//------------------------------------------------------------------------------
// wadretriever.cpp
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
#include "wadretriever.h"

#include "entities/waddownloadinfo.h"
#include "protocols/entities/networkreplywrapperinfo.h"
#include "protocols/networkreplysignalwrapper.h"
#include "wadretriever/wadinstaller.h"
#include "zip/unarchive.h"

#include <QFileInfo>

WadRetriever::WadRetriever()
{
	d.bIsAborting = false;
	d.maxConcurrentWadDownloads = 3;
	d.pNetworkAccessManager = new QNetworkAccessManager();
}

WadRetriever::~WadRetriever()
{
	// Produces crashes if simply deleted.
	d.pNetworkAccessManager->deleteLater();
	abort();
}

void WadRetriever::abort()
{
	d.bIsAborting = true;

	while (!d.wads.isEmpty())
	{
		delete d.wads.takeFirst();
	}
}

void WadRetriever::addUrl(const WadDownloadInfo& wad, const QUrl& url)
{
	WadRetrieverInfo* pRetrieverInfo = findRetrieverInfo(wad);
	if (pRetrieverInfo != NULL)
	{
		if (!hasUrl(*pRetrieverInfo, url))
		{
			pRetrieverInfo->downloadUrls << url;

			startNextDownloads();
		}
	}
}

QUrl WadRetriever::extractNextValidUrl(WadRetrieverInfo& wadRetrieverInfo)
{
	while (!wadRetrieverInfo.downloadUrls.isEmpty())
	{
		QUrl url = wadRetrieverInfo.downloadUrls.takeFirst();
		if (url.isValid() && !d.usedDownloadUrls.contains(url))
		{
			return url;
		}
	}

	return QUrl();
}

WadRetriever::WadRetrieverInfo* WadRetriever::findRetrieverInfo(const WadDownloadInfo& wad)
{
	QList<WadRetrieverInfo* >::iterator it;
	for (it = d.wads.begin(); it != d.wads.end(); ++it)
	{
		WadRetrieverInfo& wadInfo = **it;
		if (wadInfo == wad)
		{
			return &wadInfo;
		}
	}

	return NULL;
}

WadRetriever::WadRetrieverInfo* WadRetriever::findRetrieverInfo(const QString& wadName)
{
	QList<WadRetrieverInfo* >::iterator it;
	for (it = d.wads.begin(); it != d.wads.end(); ++it)
	{
		WadRetrieverInfo& wadInfo = **it;
		if (wadInfo == wadName)
		{
			return &wadInfo;
		}
	}

	return NULL;
}

WadRetriever::WadRetrieverInfo* WadRetriever::findRetrieverInfo(const QNetworkReply* pNetworkReply)
{
	QList<WadRetrieverInfo* >::iterator it;
	for (it = d.wads.begin(); it != d.wads.end(); ++it)
	{
		WadRetrieverInfo& wadInfo = **it;
		if (wadInfo.pNetworkReply != NULL)
		{
			if (*wadInfo.pNetworkReply == pNetworkReply)
			{
				return &wadInfo;
			}
		}
	}

	return NULL;
}

WadRetriever::WadRetrieverInfo* WadRetriever::getNextWaitingRetrieverInfo()
{
	QList<WadRetrieverInfo* >::iterator it;
	for (it = d.wads.begin(); it != d.wads.end(); ++it)
	{
		WadRetrieverInfo& wadInfo = **it;
		if (wadInfo.pNetworkReply == NULL && !wadInfo.downloadUrls.isEmpty())
		{
			return &wadInfo;
		}
	}

	return NULL;
}

bool WadRetriever::hasUrl(const WadRetrieverInfo& wadRetrieverInfo, const QUrl& url) const
{
	return wadRetrieverInfo.downloadUrls.contains(url) || d.usedDownloadUrls.contains(url);
}

bool WadRetriever::hasWad(const WadDownloadInfo& wad) const
{
	foreach (const WadRetrieverInfo* wadInfo, d.wads)
	{
		if (*wadInfo == wad)
		{
			return true;
		}
	}

	return false;
}

QList< WadDownloadInfo* > WadRetriever::getWadDownloadInfoList()
{
	QList< WadDownloadInfo* > list;

	QList<WadRetrieverInfo* >::iterator it;
	for (it = d.wads.begin(); it != d.wads.end(); ++it)
	{
		list << (*it)->wad;
	}

	return list;
}

bool WadRetriever::isAnyDownloadWorking() const
{
	foreach (const WadRetrieverInfo* wadInfo, d.wads)
	{
		if (wadInfo->pNetworkReply != NULL
		|| !wadInfo->downloadUrls.isEmpty())
		{
			return true;
		}
	}

	return false;
}

bool WadRetriever::isAnyWadPending() const
{
	if (numWadsDownloads() <= 0)
	{
		return false;
	}

	return numWadsDownloads() > numCurrentRunningDownloads();
}

void WadRetriever::networkQueryDownloadProgress(QNetworkReply* pReply, qint64 current, qint64 total)
{
	WadRetrieverInfo* pInfo = findRetrieverInfo(pReply);
	if (pInfo != NULL) // if NULL then WTF
	{
		emit wadDownloadProgress(*pInfo->wad, current, total);
	}
}

void WadRetriever::networkQueryFinished(QNetworkReply* pReply)
{
	WadRetrieverInfo* pInfo = findRetrieverInfo(pReply);
	if (pInfo != NULL)
	{
		NetworkReplyWrapperInfo* pReplyWrapperInfo = pInfo->pNetworkReply;
		emit wadDownloadFinished(*pInfo->wad);

		// Clear for later reuse of pointer.
		pInfo->pNetworkReply = NULL;

		QUrl url = pReply->request().url();
		QUrl possibleRedirectUrl = pReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
		if (!possibleRedirectUrl.isEmpty()
			&& possibleRedirectUrl != url)
		{
			if (possibleRedirectUrl.isRelative())
			{
				possibleRedirectUrl = url.resolved(possibleRedirectUrl);
			}

			if (!d.bIsAborting)
			{
				startNetworkQuery(*pInfo, possibleRedirectUrl);
			}
		}
		else
		{
			// Get filename, if possible extract the filename from the URL,
			// if not, use the wad stored in the pInfo.
			QFileInfo urlPathInfo(url.path());
			QString filename = urlPathInfo.fileName();

			if (filename.isEmpty())
			{
				filename = pInfo->wad->name();
			}

			tryInstall(filename, pReply->readAll());
			if (isAnyWadPending())
			{
				emit pendingUrls();
			}
		}

		// Remember to clean up.
		pReplyWrapperInfo->deleteMembersLater();
		delete pReplyWrapperInfo;
	}
	else
	{
		// if pInfo is NULL then WTF
		pReply->deleteLater();
	}
}

int WadRetriever::numCurrentRunningDownloads() const
{
	int num = 0;

	// Count each retriever info.
	foreach (const WadRetrieverInfo* info, d.wads)
	{
		if (info->pNetworkReply != NULL)
		{
			++num;
		}
	}

	return num;
}

bool WadRetriever::parseInstallerResult(const WadInstaller::WadInstallerResult& result, const QString& filename, bool bWasArchive)
{
	if (result.isError())
	{
		QString errorPrefix;
		if (bWasArchive)
		{
			errorPrefix = tr("Error when extracting archive \"%1\": ").arg(filename);
		}
		else
		{
			errorPrefix = tr("Error when installing file \"%1\": ").arg(filename);
		}

		emit message(errorPrefix + result.error, result.isCriticalError() ? WadseekerLib::CriticalError : WadseekerLib::Error);
		if (result.isCriticalError())
		{
			return false;
		}
	}
	else
	{
		foreach (const QString& installedWadName, result.installedWads)
		{
			WadRetrieverInfo* pInfo = findRetrieverInfo(installedWadName);
			emit wadInstalled(*pInfo->wad);

			removeWadRetrieverInfo(pInfo);
		}

		if (d.wads.isEmpty())
		{
			emit finished();
			return false;
		}
	}

	return true;
}

void WadRetriever::removeWadRetrieverInfo(WadRetrieverInfo* pWadRetrieverInfo)
{
	QList<WadRetrieverInfo* >::iterator it;
	for (it = d.wads.begin(); it != d.wads.end(); ++it)
	{
		WadRetrieverInfo* pInfo = *it;
		if (pInfo == pWadRetrieverInfo)
		{
			d.wads.erase(it);
			delete pWadRetrieverInfo;

			break;
		}
	}
}

void WadRetriever::setNetworkReply(WadRetrieverInfo& wadRetrieverInfo, QNetworkReply* pReply)
{
	NetworkReplyWrapperInfo* pWrapperInfo = new NetworkReplyWrapperInfo(pReply);
	wadRetrieverInfo.pNetworkReply = pWrapperInfo;

	this->connect(pWrapperInfo->pSignalWrapper, SIGNAL( downloadProgress(QNetworkReply*, qint64, qint64) ),
		SLOT( networkQueryDownloadProgress(QNetworkReply*, qint64, qint64) ));
	this->connect(pWrapperInfo->pSignalWrapper, SIGNAL( finished(QNetworkReply*) ),
		SLOT( networkQueryFinished(QNetworkReply*) ));
}

void WadRetriever::setWads(const QList<WadDownloadInfo>& wads)
{
	foreach (const WadDownloadInfo& wad, wads)
	{
		if (!hasWad(wad))
		{
			WadRetrieverInfo* pRetrieverInfo = new WadRetrieverInfo(wad);

			d.wads << pRetrieverInfo;
		}
	}
}

void WadRetriever::startNextDownloads()
{
	if (d.wads.isEmpty())
	{
		emit finished();
	}

	while (numCurrentRunningDownloads() < d.maxConcurrentWadDownloads)
	{
		WadRetrieverInfo* pRetrieverInfo = getNextWaitingRetrieverInfo();
		if (pRetrieverInfo == NULL)
		{
			// Nothing we can do now. Leave.
			break;
		}

		// Take first valid URL to use.
		QUrl url = extractNextValidUrl(*pRetrieverInfo);
		startNetworkQuery(*pRetrieverInfo, url);
	}
}

void WadRetriever::startNetworkQuery(WadRetrieverInfo& wadRetrieverInfo, const QUrl& url)
{
	QNetworkRequest request;
	request.setUrl(url);
	request.setRawHeader("User-Agent", d.userAgent.toAscii());

	QNetworkReply* pReply = d.pNetworkAccessManager->get(request);
	setNetworkReply(wadRetrieverInfo, pReply);

	emit wadDownloadStarted(*wadRetrieverInfo.wad, url);
}

void WadRetriever::tryInstall(const QString& filename, const QByteArray& byteArray)
{
	const bool IS_ARCHIVE = true;
	WadInstaller installer(d.targetSavePath);

	// Install file directly.
	if (findRetrieverInfo(filename) != NULL)
	{
		WadInstaller::WadInstallerResult result = installer.installFile(filename, byteArray);
		if (!parseInstallerResult(result, filename, !IS_ARCHIVE))
		{
			return;
		}
	}

	// Detect archive.
	QFileInfo fileInfo(filename);
	UnArchive* archive = UnArchive::openArchive(fileInfo, byteArray);
	if (archive != NULL)
	{
		WadInstaller::WadInstallerResult result = installer.installArchive(*archive, getWadDownloadInfoList());
		delete archive;

		if (!parseInstallerResult(result, filename, IS_ARCHIVE))
		{
			return;
		}
	}

	if (!d.bIsAborting)
	{
		startNextDownloads();
	}
}

////////////////////////////////////////////////////////////////////////////////

WadRetriever::WadRetrieverInfo::WadRetrieverInfo(const WadDownloadInfo& wad)
{
	this->pNetworkReply = NULL;
	this->wad = new WadDownloadInfo(wad);
}

WadRetriever::WadRetrieverInfo::~WadRetrieverInfo()
{
	if (pNetworkReply != NULL)
	{
		pNetworkReply->deleteMembersLater();
		delete pNetworkReply;
	}

	delete wad;
}

bool WadRetriever::WadRetrieverInfo::operator==(const WadDownloadInfo& wad) const
{
	return *this->wad == wad;
}

bool WadRetriever::WadRetrieverInfo::operator!=(const WadDownloadInfo& wad) const
{
	return *this->wad != wad;
}

bool WadRetriever::WadRetrieverInfo::operator==(const WadRetrieverInfo& other) const
{
	return *this->wad == *other.wad;
}

bool WadRetriever::WadRetrieverInfo::operator!=(const WadRetrieverInfo& other) const
{
	return !(*this == other);
}

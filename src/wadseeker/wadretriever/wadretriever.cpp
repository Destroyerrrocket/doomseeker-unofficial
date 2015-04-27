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
#include "protocols/http.h"
#include "protocols/networkreplysignalwrapper.h"
#include "protocols/urlprovider.h"
#include "wadretriever/wadinstaller.h"
#include "wwwseeker/urlparser.h"
#include "zip/unarchive.h"
#include "ioutils.h"

#include <QBuffer>
#include <QDebug>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTimer>

WadRetriever::WadRetriever()
{
	d.bIsAborting = false;
	d.maxConcurrentWadDownloads = 3;
	d.pNetworkAccessManager = new FixedNetworkAccessManager();
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

void WadRetriever::addMirrorUrls(const WadDownloadInfo& wad, const QList<QUrl>& urls)
{
	WadRetrieverInfo* pRetrieverInfo = findRetrieverInfo(wad);
	if (pRetrieverInfo != NULL)
	{
		// We need to filter out URLs that were used globally.
		QList<QUrl> filteredUrlList;
		foreach (const QUrl& urlCandidate, urls)
		{
			if (!wasUrlUsed(urlCandidate))
			{
				filteredUrlList << urlCandidate;
			}
		}

		if (!filteredUrlList.isEmpty())
		{
			pRetrieverInfo->downloadUrls->addMirrorUrls(filteredUrlList);
			startNextDownloads();
		}
	}
}

void WadRetriever::addUrl(const WadDownloadInfo& wad, const QUrl& url)
{
	WadRetrieverInfo* pRetrieverInfo = findRetrieverInfo(wad);
	if (pRetrieverInfo != NULL)
	{
		if (!hasUrl(*pRetrieverInfo, url))
		{
			(*pRetrieverInfo->downloadUrls) << url;
			// Delay the download a bit in anticipation for more possible
			// links to the file. This allows to actually prioritize
			// urls. Calling this slot multiple times won't start
			// downloads when not supposed to.
			QTimer::singleShot(10, this, SLOT(startNextDownloads()));
		}
	}
}

bool WadRetriever::areAllWadsPendingUrls() const
{
	if (numTotalWadsDownloads() <= 0)
	{
		return false;
	}

	return numDownloadsPendingUrls() == numTotalWadsDownloads();
}


QUrl WadRetriever::extractNextValidUrl(WadRetrieverInfo& wadRetrieverInfo)
{
	// We need to copy the list because we will modify the original.
	QList<QUrl> availableUrls = wadRetrieverInfo.downloadUrls->allAvailableUrls();

	while (!availableUrls.isEmpty())
	{
		QUrl url = availableUrls.takeFirst();
		if (url.isValid() && !wasUrlUsed(url))
		{
			// This URL is valid but it might not be allowed
			// to use it at the current moment.
			if (isUrlAllowedToDownloadATM(url))
			{
				wadRetrieverInfo.downloadUrls->removeUrl(url);
				return url;
			}
		}
		else
		{
			// This URL cannot be used. Get rid of it.
			wadRetrieverInfo.downloadUrls->removeUrl(url);
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

const WadRetriever::WadRetrieverInfo* WadRetriever::findRetrieverInfo(const WadDownloadInfo& wad) const
{
	QList<WadRetrieverInfo* >::const_iterator it;
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

const WadRetriever::WadRetrieverInfo* WadRetriever::findRetrieverInfo(const QString& wadName) const
{
	QList<WadRetrieverInfo* >::const_iterator it;
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

const WadRetriever::WadRetrieverInfo* WadRetriever::findRetrieverInfo(const QNetworkReply* pNetworkReply) const
{
	QList<WadRetrieverInfo* >::const_iterator it;
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

QList< WadRetriever::WadRetrieverInfo* > WadRetriever::getAllCurrentlyRunningDownloadsInfos() const
{
	QList< WadRetrieverInfo* > list;

	QList< WadRetrieverInfo* >::const_iterator it;
	for (it = d.wads.begin(); it != d.wads.end(); ++it)
	{
		if ((*it)->pNetworkReply != NULL)
		{
			list << (*it);
		}
	}

	return list;
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

bool WadRetriever::hasUrl(const WadRetrieverInfo& wadRetrieverInfo, const QUrl& url) const
{
	if (wadRetrieverInfo.downloadUrls->hasOrHadUrl(url))
	{
		return true;
	}

	return wasUrlUsed(url);
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

bool WadRetriever::isAnyDownloadWorking() const
{
	if (numTotalWadsDownloads() <= 0)
	{
		return false;
	}

	return numDownloadsPendingUrls() < numTotalWadsDownloads();
}

bool WadRetriever::isAnyWadPendingUrl() const
{
	return numDownloadsPendingUrls() != 0;
}

bool WadRetriever::isDownloadingWad(const WadDownloadInfo& wad) const
{
	const WadRetrieverInfo* pInfo = findRetrieverInfo(wad);

	return pInfo != NULL && pInfo->pNetworkReply != NULL;
}

bool WadRetriever::isUrlAllowedToDownloadATM(const QUrl& url) const
{
	QList< WadRetrieverInfo* > currentDownloads = getAllCurrentlyRunningDownloadsInfos();
	foreach (const WadRetrieverInfo* pInfo, currentDownloads)
	{
		const QUrl& downloadedUrl = pInfo->pNetworkReply->pReply->request().url();
		if (UrlParser::hasSameHost(downloadedUrl, url))
		{
			return false;
		}
	}

	return true;
}

void WadRetriever::networkQueryDownloadProgress(QNetworkReply* pReply, qint64 current, qint64 total)
{
	WadRetrieverInfo* pInfo = findRetrieverInfo(pReply);
	if (pInfo != NULL) // if NULL then WTF
	{
		emit wadDownloadProgress(*pInfo->wad, current, total);
	}
}

void WadRetriever::networkQueryError(QNetworkReply* pReply, QNetworkReply::NetworkError code)
{
	// We shall ignore OperationCanceledError because this error is caused
	// by a call to QNetworkReply::abort() and it may confuse users.
	// "Why am I getting this error? Is it a bug? Yeah, it is a bug!"
	if (code != QNetworkReply::NoError && code != QNetworkReply::OperationCanceledError)
	{
		WadRetrieverInfo* pInfo = findRetrieverInfo(pReply);
		QString errorString = FixedNetworkAccessManager::networkErrorToString(code);

		emit message(tr("File \"%1\": network error occurred: %2")
			.arg(pInfo->wad->name(), errorString), WadseekerLib::Error);
	}
	qDebug() << "WadRetriever::networkQueryError() " << code;
}

void WadRetriever::networkQueryFinished(QNetworkReply* pReply)
{
	WadRetrieverInfo* pInfo = findRetrieverInfo(pReply);
	if (pInfo != NULL)
	{
		QUrl url = pReply->request().url();
		if (url.isEmpty())
		{
			url = pReply->url();
		}

#ifndef NDEBUG
		{
			QUrl replyUrl = pReply->url();
			QList<QByteArray> headers = pReply->rawHeaderList();
			printf("WadRetriever HEADERS\n");
			printf("Error: %d\n", pReply->error());
			printf("Wadseeker request URL: %s\n", pReply->request().url().toEncoded().constData());
			printf("Request URL %s\n", pReply->request().url().toEncoded().constData());
			printf("Reply URL %s\n", replyUrl.toEncoded().constData());
			foreach (const QByteArray& headerName, headers)
			{
				QByteArray headerData = pReply->rawHeader(headerName);
				printf("%s: %s\n", headerName.constData(), headerData.constData());
			}
			printf("END OF HEADERS\n");

		    qDebug() << "WadRetriever: Finished network query for URL: " << url.toString();
		}
#endif
		emit message(tr("Finished URL: %1").arg(url.toString()),
					WadseekerLib::Notice);

		NetworkReplyWrapperInfo* pReplyWrapperInfo = pInfo->pNetworkReply;
		emit wadDownloadFinished(*pInfo->wad);

		// Clear for later reuse of pointer.
		pInfo->pNetworkReply = NULL;

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
			resolveDownloadFinish(pReply, pInfo);
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
	return getAllCurrentlyRunningDownloadsInfos().size();
}

int WadRetriever::numDownloadsPendingUrls() const
{
	int num = 0;

	// Count each retriever info.
	foreach (const WadRetrieverInfo* info, d.wads)
	{
		if (info->pNetworkReply == NULL &&
			info->downloadUrls->isEmpty())
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
		foreach (const QFileInfo& installedWad, result.installedWads)
		{
			WadRetrieverInfo* pInfo = findRetrieverInfo(installedWad.fileName());
			emit message(tr("Installed WAD: %1 [%2 bytes]").arg(pInfo->wad->name()).arg(installedWad.size()),
				WadseekerLib::Notice);

			pInfo->wad->setSize(installedWad.size());
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

void WadRetriever::resolveDownloadFinish(QNetworkReply* pReply, WadRetrieverInfo* pWadRetrieverInfo)
{
	QUrl url = pReply->request().url();
	if (url.isEmpty())
	{
		url = pReply->url();
	}

	if (pReply->error() != QNetworkReply::NoError)
	{
		emit badUrlDetected(url);
	}

	if (pReply->bytesAvailable() > 0 && pReply->error() == QNetworkReply::NoError)
	{
		// File was downloaded successfully from this URL so we do not
		// need to query the mirrors anymore. If the file is invalid
		// the mirrors will also contain the invalid file and should not be
		// queried.
		pWadRetrieverInfo->downloadUrls->removeUrlAndMirrors(url);

		// Save the contents to temporary file so it can be seeked freely.
		QTemporaryFile tempFile;
		tempFile.open();
		IOUtils::copy(*pReply, tempFile);
		tempFile.seek(0);

		// We need to determine the correct filename.
		QString filename = pWadRetrieverInfo->wad->name();
		Http http(pReply);
		if (!http.attachmentName().trimmed().isEmpty())
		{
			filename = http.attachmentName().trimmed();
		}
		else
		{
			QFileInfo urlInfo(url.path());
			if (!urlInfo.fileName().trimmed().isEmpty())
			{
				filename = urlInfo.fileName().trimmed();
			}
		}

		// Now try to install the file under downloaded filename.
		tryInstall(filename, &tempFile);
	}
	else if (!d.bIsAborting)
	{
		// Abort was commenced, but it might have been only an abort
		// for that particular file and not a global one.
		startNextDownloads();
	}
}

void WadRetriever::setNetworkReply(WadRetrieverInfo& wadRetrieverInfo, QNetworkReply* pReply)
{
	NetworkReplyWrapperInfo* pWrapperInfo = new NetworkReplyWrapperInfo(pReply);
	pWrapperInfo->startConnectionTimeoutTimer();
	pWrapperInfo->setProgressTimeout(NetworkReplyWrapperInfo::SUGGESTED_PROGRESS_TIMEOUT_MSECS);
	wadRetrieverInfo.pNetworkReply = pWrapperInfo;

	this->connect(pWrapperInfo->pSignalWrapper, SIGNAL( downloadProgress(QNetworkReply*, qint64, qint64) ),
		SLOT( networkQueryDownloadProgress(QNetworkReply*, qint64, qint64) ));
	this->connect(pWrapperInfo->pSignalWrapper, SIGNAL( error(QNetworkReply*, QNetworkReply::NetworkError) ),
		SLOT( networkQueryError(QNetworkReply*, QNetworkReply::NetworkError) ));
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

void WadRetriever::skipCurrentUrl(const WadDownloadInfo& wad)
{
	WadRetrieverInfo* pInfo = findRetrieverInfo(wad);
	if (pInfo != NULL && pInfo->pNetworkReply != NULL)
	{
		pInfo->pNetworkReply->pReply->abort();
	}
}

void WadRetriever::startNextDownloads()
{
	if (d.wads.isEmpty())
	{
		emit finished();
		return;
	}

	if (areAllWadsPendingUrls())
	{
		emit pendingUrls();
		return;
	}

	if (d.bIsAborting)
	{
		return;
	}

	foreach (WadRetrieverInfo* pRetrieverInfo, d.wads)
	{
		if (numCurrentRunningDownloads() >= d.maxConcurrentWadDownloads)
		{
			break;
		}

		if (pRetrieverInfo->isAvailableForDownload())
		{
			// Take first valid URL to use.
			QUrl url = extractNextValidUrl(*pRetrieverInfo);
			if (url.isValid())
			{
				startNetworkQuery(*pRetrieverInfo, url);
			}
		}
	}
}

void WadRetriever::startNetworkQuery(WadRetrieverInfo& wadRetrieverInfo, const QUrl& url)
{
	QNetworkRequest request;
	request.setUrl(url);
	request.setRawHeader("User-Agent", d.userAgent.toUtf8());

	d.usedDownloadUrls << url;

#ifndef NDEBUG
	qDebug() << "WadRetriever: Starting network query for URL: " << url.toString();
#endif

	QNetworkReply* pReply = d.pNetworkAccessManager->get(request);
	setNetworkReply(wadRetrieverInfo, pReply);

	emit wadDownloadStarted(*wadRetrieverInfo.wad, url);
}

void WadRetriever::tryInstall(const QString& filename, QIODevice* dataStream)
{
	const bool IS_ARCHIVE = true;
	WadInstaller installer(d.targetSavePath);

	QTemporaryFile tempFile;
	QIODevice* stream = dataStream;
	if (dataStream->isSequential())
	{
		tempFile.open();
		// We need to be able to seek the IO device because it needs
		// to be reusable. If the current IO device cannot be seeked
		// we need to copy its contents into a temp file.
		IOUtils::copy(*dataStream, tempFile);
		tempFile.seek(0);
		stream = &tempFile;
	}

	emit message(tr("Attempting to install file %1 of size %2").arg(filename).arg(stream->size()),
				WadseekerLib::Notice);

#ifndef NDEBUG
	qDebug() << "WadRetriever: Attempting to install file " << filename << " of size " << stream->size();
#endif

	// Install file directly.
	if (findRetrieverInfo(filename) != NULL)
	{
		stream->seek(0);
		WadInstaller::WadInstallerResult result = installer.installFile(filename, stream);
		if (!parseInstallerResult(result, filename, !IS_ARCHIVE))
		{
			return;
		}
	}

	// Detect archive.
	QFileInfo fileInfo(filename);
	stream->seek(0);
	UnArchive* archive = UnArchive::openArchive(fileInfo, stream);
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

bool WadRetriever::wasUrlUsed(const QUrl& url) const
{
	foreach (const QUrl& tmpUrl, d.usedDownloadUrls)
	{
		if (UrlParser::urlEqualsCaseInsensitive(tmpUrl, url))
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////

WadRetriever::WadRetrieverInfo::WadRetrieverInfo(const WadDownloadInfo& wad)
{
	this->downloadUrls = new URLProvider();
	this->pNetworkReply = NULL;
	this->wad = new WadDownloadInfo(wad);
}

WadRetriever::WadRetrieverInfo::~WadRetrieverInfo()
{
	delete downloadUrls;

	if (pNetworkReply != NULL)
	{
		pNetworkReply->deleteMembersLater();
		delete pNetworkReply;
	}

	delete wad;
}

bool WadRetriever::WadRetrieverInfo::isAvailableForDownload() const
{
	return pNetworkReply == NULL && !this->downloadUrls->isEmpty();
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

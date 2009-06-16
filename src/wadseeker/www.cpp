//------------------------------------------------------------------------------
// www.cpp
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
#include "html.h"
#include "link.h"
#include "protocols/ftp.h"
#include "protocols/http.h"
#include "www.h"
#include <QFileInfo>

QString WWW::ignoringMessage = tr("%1 is not a HTML file, nor a wanted file, nor a file with \'.zip\' extension. Ignoring.\n");

WWW::WWW()
{
	currentProtocol = NULL;

	ftp = new Ftp();
	http = new Http();

	connect(ftp, SIGNAL( aborted() ), this, SLOT( protocolAborted() ) );
	connect(ftp, SIGNAL( dataReadProgress(int, int) ), this, SLOT( downloadProgressSlot(int, int) ) );
	connect(ftp, SIGNAL( done(bool, QByteArray&, int, const QString&) ), this, SLOT( protocolDone(bool, QByteArray&, int, const QString&) ) );
	connect(ftp, SIGNAL( message(const QString&, int) ), this, SLOT( messageSlot(const QString&, int) ) );

	connect(http, SIGNAL( aborted() ), this, SLOT( protocolAborted() ) );
	connect(http, SIGNAL( dataReadProgress(int, int) ), this, SLOT( downloadProgressSlot(int, int) ) );
	connect(http, SIGNAL( done(bool, QByteArray&, int, const QString&) ), this, SLOT( protocolDone(bool, QByteArray&, int, const QString&) ) );
	connect(http, SIGNAL( message(const QString&, int) ), this, SLOT( messageSlot(const QString&, int) ) );
	connect(http, SIGNAL( redirect(const QUrl&) ), this, SLOT( get(const QUrl&) ) );
}

void WWW::abort()
{
	aborting = true;
	if (currentProtocol != NULL)
	{
		currentProtocol->abort();
	}
	else
	{
		emit aborted();
	}
}

QUrl WWW::constructValidUrl(const QUrl& url)
{
	QUrl returnUrl = url;
	if (returnUrl.scheme().isEmpty())
	{
		returnUrl.setScheme(processedUrl.scheme());
	}

	if (returnUrl.scheme().isEmpty())
	{
		return QUrl();
	}

	if (returnUrl.host().isEmpty())
	{
		returnUrl.setHost(processedUrl.host());
	}

	if (returnUrl.host().isEmpty())
	{
		return QUrl();
	}

	return returnUrl;
}

void WWW::downloadProgressSlot(int done, int total)
{
	emit downloadProgress(done, total);
}

void WWW::get(const QUrl& url)
{
	QUrl urlValid = constructValidUrl(url);
	if (urlValid.isEmpty())
	{
		emit message(tr("Failed to create valid URL out of \"%1\". Ignoring.\n").arg(url.toString()), Wadseeker::Error);
		emit fail();
		return;
	}

	processedUrl = urlValid;

	if (Http::isHTTPLink(urlValid))
	{
		currentProtocol = http;
		http->get(urlValid);
	}
	else if (Ftp::isFTPLink(urlValid))
	{
		currentProtocol = ftp;
		ftp->get(urlValid);
	}
	else
	{
		currentProtocol = NULL;
		message(tr("Protocol for this site is not supported\n"), Wadseeker::Error);
		emit fail();
		return;
	}
}

bool WWW::getUrl(const QUrl& url)
{
	if (!isAbsoluteUrl(url))
		return false;

	QUrl urlValid = url;
	if (urlValid.path().isEmpty())
		urlValid.setPath("/");

	if (Http::isHTTPLink(urlValid))
	{
		currentProtocol = http;
		http->get(urlValid);
	}
	else if (Ftp::isFTPLink(urlValid))
	{
		currentProtocol = ftp;
		ftp->get(urlValid);
	}
	else
	{
		currentProtocol = NULL;
		message(tr("Protocol for this site is not supported\n"), Wadseeker::Error);
		return false;
	}

	return true;
}

bool WWW::isAbsoluteUrl(const QUrl& url)
{
	if (url.scheme().isEmpty() || url.host().isEmpty())
	{
		return false;
	}

	return true;
}

void WWW::messageSlot(const QString& msg, int type)
{
	emit message(msg, type);
}

void WWW::protocolAborted()
{
	currentProtocol = NULL;
	if (!aborting)
	{
		emit fail();
	}
	else
	{
		emit aborted();
	}
}

void WWW::protocolDone(bool success, QByteArray& data, int fileType, const QString& filename)
{
	currentProtocol = NULL;
	if (success)
	{
		emit fileDone(data, filename);
	}
	else
	{
		emit fail();
	}
}

void WWW::setTimeConnectTimeout(int seconds)
{
	Protocol::setTimeConnectTimeoutSeconds(seconds);
}

void WWW::setTimeDownloadTimeout(int seconds)
{
	Protocol::setTimeDownloadTimeoutSeconds(seconds);
}

void WWW::setUserAgent(const QString& agent)
{
	http->setUserAgent(agent);
}
///////////////////////////////////////////////////////////////////////////////
WWWSeeker::WWWSeeker()
{
	connect(http, SIGNAL( nameAndTypeOfReceivedFile(const QString&, int) ), this, SLOT( protocolNameAndTypeOfReceivedFile(const QString&, int) ) );
}

void WWWSeeker::checkNextSite()
{
	QUrl site = nextSite();

	if (site.isEmpty())
	{
		processedUrl = QUrl();
		emit message(tr("No more sites.\n"), Wadseeker::Notice);
		emit fail();
	}
	else
	{
		emit downloadProgress(0, 100);
		get(site);
	}
}

void WWWSeeker::get(const QUrl& url)
{
	QUrl urlValid = constructValidUrl(url);
	if (urlValid.isEmpty())
	{
		emit message(tr("Failed to create valid URL out of \"%1\". Ignoring.\n").arg(url.toString()), Wadseeker::Error);
		checkNextSite();
		return;
	}

	if (checkedLinks.find(urlValid.toString()) != checkedLinks.end())
	{
		checkNextSite();
		return;
	}

	checkedLinks.insert(urlValid.toString());
	processedUrl = urlValid;

	emit message(tr("Next site: %1").arg(urlValid.toString()), Wadseeker::Notice);
	if (Http::isHTTPLink(urlValid))
	{
		currentProtocol = http;
		http->get(urlValid);
	}
	else if (Ftp::isFTPLink(urlValid))
	{
		QFileInfo fi(urlValid.path());
		if (!isWantedFileOrZip(fi.fileName()))
		{
			emit message(ignoringMessage.arg(fi.fileName()), Wadseeker::Notice);
			checkNextSite();
			return;
		}
		else
		{
			currentProtocol = ftp;
			ftp->get(urlValid);
		}
	}
	else
	{
		currentProtocol = NULL;
		message(tr("Protocol for this site is not supported\n"), Wadseeker::Error);
		checkNextSite();
		return;
	}

}

bool WWWSeeker::isWantedFileOrZip(const QString& filename)
{
	if (!primaryFile.isNull())
	{
		if (primaryFile.compare(filename, Qt::CaseInsensitive) == 0)
			return true;
	}

	QFileInfo fi(filename);
	if (fi.suffix().compare("zip", Qt::CaseInsensitive) == 0)
		return true;

	return false;
}

QUrl WWWSeeker::nextSite()
{
	while (true)
	{
		QUrl url;
		if (!customSiteUsed && customSite.isValid())
		{
			processedUrl = QUrl();
			url = customSite;
			customSiteUsed = true;
		}
		else if (!directLinks.isEmpty())
		{
			url = directLinks.takeFirst();
		}
		else if (!siteLinks.isEmpty())
		{
			url = siteLinks.takeFirst();
		}
		else if (currentPrimarySite < primarySites.size())
		{
			processedUrl = QUrl();
			url = primarySites[currentPrimarySite];
			++currentPrimarySite;
		}
		else
		{
			break;
		}

		if (url.isEmpty() || !url.isValid())
			continue;

		url = url.toString().replace("%WADNAME%", primaryFile);

		return url;
	}

	return QUrl();
}

void WWWSeeker::protocolAborted()
{
	currentProtocol = NULL;
	if (!aborting)
	{
		checkNextSite();
	}
	else
	{
		emit aborted();
	}
}

void WWWSeeker::protocolDone(bool success, QByteArray& data, int fileType, const QString& filename)
{
	currentProtocol = NULL;
	if (success)
	{
		emit message(tr("Got file %1.").arg(filename), Wadseeker::Notice);
		if (fileType == Protocol::Html)
		{
			int siteLinksNum, directLinksNum; // CHtml::linksFromHTMLByPattern will zero this
			emit message(tr("Parsing file as HTML looking for links."), Wadseeker::Notice);
			CHtml html(data);
			html.capitalizeHTMLTags();
			html.linksFromHTMLByPattern(filesToFind, siteLinks, directLinks, processedUrl, siteLinksNum, directLinksNum);
			emit message(tr("Site links found: %1 | Direct links found: %2\n").arg(siteLinksNum).arg(directLinksNum), Wadseeker::Notice);
			checkNextSite();
		}
		else
		{
			emit fileDone(data, filename);
		}
	}

	emit message(" ", Wadseeker::Notice);

	if (!success)
	{
		checkNextSite();
	}
}

void WWWSeeker::protocolNameAndTypeOfReceivedFile(const QString& name, int type)
{
	if (type == Protocol::Other)
	{
		if (!isWantedFileOrZip(name))
		{
			emit message(ignoringMessage.arg(name), Wadseeker::Notice);
			if (currentProtocol != NULL)
			{
				currentProtocol->abort();
			}
		}
	}
}

void WWWSeeker::searchFiles(const QStringList& list, const QString& primaryFilename)
{
	aborting = false;
	customSiteUsed = false;
	currentPrimarySite = 0;
	checkedLinks.clear();
	filesToFind = list;
	primaryFile = primaryFilename;
	processedUrl = QUrl();

	checkNextSite();
}

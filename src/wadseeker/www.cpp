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
#include "protocols/idgames.h"
#include "www.h"
#include <QFileInfo>

QString WWW::ignoringMessage = tr("%1 is not a HTML file, nor a wanted file, nor a file with \'.zip\' or \'.7z\' extension. Ignoring.\n");

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

WWW::~WWW()
{
	delete ftp;
	delete http;
}

void WWW::abort()
{
	abortExec(true);
}

void WWW::abortExec(bool abortCompletely)
{
	aborting = abortCompletely;
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
	setUserAgentEx(agent);
}
///////////////////////////////////////////////////////////////////////////////
WWWSeeker::WWWSeeker()
{
	idgames = new Idgames(Idgames::defaultIdgamesUrl());

	idgamesHasHighPriority = false;
	useIdgames = true;

	connect(http, SIGNAL( nameAndTypeOfReceivedFile(const QString&, int) ), this, SLOT( protocolNameAndTypeOfReceivedFile(const QString&, int) ) );

	connect(idgames, SIGNAL( aborted() ), this, SLOT( protocolAborted() ) );
	connect(idgames, SIGNAL( dataReadProgress(int, int) ), this, SLOT( downloadProgressSlot(int, int) ) );
	connect(idgames, SIGNAL( done(bool, QByteArray&, int, const QString&) ), this, SLOT( protocolDone(bool, QByteArray&, int, const QString&) ) );
	connect(idgames, SIGNAL( message(const QString&, int) ), this, SLOT( messageSlot(const QString&, int) ) );
	connect(idgames, SIGNAL( nameAndTypeOfReceivedFile(const QString&, int) ), this, SLOT( protocolNameAndTypeOfReceivedFile(const QString&, int) ) );
}

WWWSeeker::~WWWSeeker()
{
	delete idgames;
}

void WWWSeeker::checkNextSite()
{
	// If link for custom site is invalid customSiteUsed will be set to true,
	// even before customSite is checked through nextSite() method.
	if (customSiteUsed 			// Did we check custom site already?
	&&	useIdgames 				// Do we use idgames at all?
	&&	directLinks.isEmpty() 	// Were there any direct links found on custom site?
	&&	siteLinks.isEmpty() 	// Were there any site links found on custom site?
	&&	idgamesHasHighPriority  // Does idgames have high priority?
	&&	!idgamesUsed)			// Was idgames already searched?
	{
		this->searchIdgames();
	}
	else
	{
		QUrl site = nextSite();
		if (site.isEmpty())
		{
			// A few words of explanation:
			// We don't need to check if idgamesHasHighPriority == false.
			// If it is true, the idgamesUsed will already be set to true by the
			// call to searchIdgames() above.
			if (useIdgames && !idgamesUsed)
			{
				this->searchIdgames();
			}
			else
			{
				processedUrl = QUrl();
				emit message(tr("No more sites.\n"), Wadseeker::Notice);
				emit fail();
			}
		}
		else
		{
			emit downloadProgress(0, 100);
			get(site);
		}
	}
}

const QString WWWSeeker::defaultIdgamesUrl()
{
	return Idgames::defaultIdgamesUrl();
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
	if (fi.suffix().compare("zip", Qt::CaseInsensitive) == 0 || fi.suffix().compare("7z", Qt::CaseInsensitive) == 0)
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

		url = url.toString().replace("%WADNAME%", primaryFile).replace("%ZIPNAME%", zipFile);

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

void WWWSeeker::searchFiles(const QStringList& list, const QString& primaryFilename, const QString& zipFilename)
{
	aborting = false;
	// This needs to be set here due to how Idgames archive is checked.
	customSiteUsed = !customSite.isValid();
	currentPrimarySite = 0;
	checkedLinks.clear();
	filesToFind = list;
	idgamesUsed = false;
	primaryFile = primaryFilename;
	zipFile = zipFilename;

	processedUrl = QUrl();

	checkNextSite();
}

void WWWSeeker::searchIdgames()
{
	idgamesUsed = true;
	currentProtocol = idgames;
	idgames->findFile(zipFile);
}

void WWWSeeker::setUseIdgames(bool use, bool highPriority, QString archiveURL)
{
	useIdgames = use;
	idgamesHasHighPriority = highPriority;
	idgames->setPage(archiveURL);
}

void WWWSeeker::setUserAgentEx(const QString& agent)
{
	idgames->setUserAgent(agent);
}

void WWWSeeker::skipSite()
{
	emit message(tr("Skipping site..."), Wadseeker::Notice);
	abortExec(false);
}

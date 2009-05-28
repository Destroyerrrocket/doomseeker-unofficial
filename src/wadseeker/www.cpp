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
#include "www.h"
#include <QFileInfo>

WWW::WWW()
{
	currentProtocol = NULL;

	connect(&ftp, SIGNAL( aborted() ), this, SLOT( protocolAborted() ) );
	connect(&ftp, SIGNAL( dataReadProgress(int, int) ), this, SLOT( downloadProgressSlot(int, int) ) );
	connect(&ftp, SIGNAL( done(bool, QByteArray&, int, const QString&) ), this, SLOT( protocolDone(bool, QByteArray&, int, const QString&) ) );
	connect(&ftp, SIGNAL( message(const QString&, Wadseeker::MessageType) ), this, SLOT( messageSlot(const QString&, Wadseeker::MessageType) ) );

	connect(&http, SIGNAL( aborted() ), this, SLOT( protocolAborted() ) );
	connect(&http, SIGNAL( dataReadProgress(int, int) ), this, SLOT( downloadProgressSlot(int, int) ) );
	connect(&http, SIGNAL( done(bool, QByteArray&, int, const QString&) ), this, SLOT( protocolDone(bool, QByteArray&, int, const QString&) ) );
	connect(&http, SIGNAL( message(const QString&, Wadseeker::MessageType) ), this, SLOT( messageSlot(const QString&, Wadseeker::MessageType) ) );
	connect(&http, SIGNAL( redirect(const QUrl&) ), this, SLOT( get(const QUrl&) ) );
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

void WWW::checkNextSite()
{
	QUrl site = nextSite();

	if (site.isEmpty())
	{
		processedUrl = QUrl();
		emit message(tr("No more sites.\n"), Wadseeker::Notice);
		emit noMoreSites();
	}
	else
	{
		emit downloadProgress(0, 100);
		get(site);
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
		currentProtocol = &http;
		http.get(urlValid);
	}
	else if (Ftp::isFTPLink(urlValid))
	{
		currentProtocol = &ftp;
		ftp.get(urlValid);
	}
	else
	{
		currentProtocol = NULL;
		message(tr("Protocol for this site is not supported\n"), Wadseeker::Error);
		checkNextSite();
	}

}

void WWW::messageSlot(const QString& msg, Wadseeker::MessageType type)
{
	emit message(msg, type);
}

QUrl WWW::nextSite()
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

void WWW::protocolAborted()
{
	if (!aborting)
	{
		checkNextSite();
	}
	else
	{
		emit aborted();
	}
}

void WWW::protocolDone(bool success, QByteArray& data, int fileType, const QString& filename)
{
	if (success)
	{
		emit message(tr("Got file %1.").arg(filename), Wadseeker::Notice);
		if (fileType == Protocol::Html)
		{
			emit message(tr("Parsing file as HTML looking for links.\n"), Wadseeker::Notice);
			CHtml html(data);
			html.capitalizeHTMLTags();
			html.linksFromHTMLByPattern(filesToFind, siteLinks, directLinks, processedUrl);
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

void WWW::searchFiles(const QStringList& list, const QString& primaryFilename)
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



//------------------------------------------------------------------------------
// www.cpp
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
#include "www.h"
#include <QFileInfo>

WWW::WWW()
{
	bAbort = false;
	currentGlobalSite = 0;

	connect(&http, SIGNAL( dataReceived(unsigned, unsigned, unsigned) ), this, SLOT( dataReceivedSlot(unsigned, unsigned, unsigned) ) );
	connect(&http, SIGNAL( error(const QString&) ), this, SLOT( httpError(const QString&) ) );
	connect(&http, SIGNAL( finishedReceiving(const QString&) ), this, SLOT( httpFinishedReceiving(const QString&) ) );
	connect(&http, SIGNAL( nameOfCurrentlyDownloadedResource(const QString&) ), this, SLOT( nameOfCurrentlyDownloadedResource(const QString&) ) );
	connect(&http, SIGNAL( notice(const QString&) ), this, SLOT( httpNotice(const QString&) ) );
	connect(&http, SIGNAL( size(unsigned int) ), this, SLOT( sizeSlot(unsigned int) ) );
}

void WWW::abort()
{
	bAbort = true;
	http.abort();
}

void WWW::dataReceivedSlot(unsigned howMuch, unsigned howMuchSum, unsigned percent)
{
	emit dataReceived(howMuch, howMuchSum, percent);
}

void WWW::get(const QString& strSeekedWad)
{
	reset();

	// Get the file extension and compare it against "zip"
	// If the file extension is "zip" already set only "zip"
	// as "binary" files for Http class.
	// If the file extension is not "zip" set "zip" and this extension
	// as "binary" files for Http class.

	QFileInfo fi(strSeekedWad);
	QString extension = fi.suffix();
	QStringList expectedFilenames;
	expectedFilenames << strSeekedWad;
	if (extension.compare("zip", Qt::CaseInsensitive) != 0)
	{
		expectedFilenames << QString(fi.completeBaseName() + ".zip");
	}

	seekedWad = strSeekedWad;
	http.setWantedFilenames(expectedFilenames);
	nextSite();
}

void WWW::getLinks()
{
	QFileInfo fi(seekedWad);
	QString extension = fi.suffix();

	QStringList wantedFileNames;
	wantedFileNames << fi.completeBaseName() + ".zip";
	if (extension.compare("zip", Qt::CaseInsensitive) != 0)
	{
		wantedFileNames << fi.fileName();
	}

	http.linksByPattern(wantedFileNames, directLinks, siteLinks);
}

void WWW::httpFinishedReceiving(const QString& err)
{
	if (bAbort)
	{
		return;
	}

	if (!err.isEmpty())
	{
		QString str = tr("HTTP Receive error: %1").arg(err);
		emit error(str);
		this->nextSite();
	}
	else
	{
		if( http.lastFileType() == Http::HTTP_FILE_TYPE_WANTED)
		{
			emit finishedReceiving(http.lastData());
		}
		else if ( http.lastFileType() == Http::HTTP_FILE_TYPE_HTML)
		{
			this->getLinks();
			this->nextSite();
		}
	}
}

void WWW::httpError(const QString& errorString)
{
	QString str = tr("HTTP error: %1").arg(errorString);
	emit error(str);
	this->nextSite();
}

void WWW::httpNotice(const QString& string)
{
	QString str = tr("HTTP notice: %1").arg(string);
	emit notice(str);
}

void WWW::nameOfCurrentlyDownloadedResource(const QString& res)
{
	lastFilename = res;
}

void WWW::nextSite()
{
	if (bAbort)
	{
		bAbort = false;
		return;
	}

	bool bGotUrl = false;

	if (!customSiteUsed && !customSite.isEmpty())
	{
		url = customSite;
		bGotUrl = true;
		customSiteUsed = true;
	}

	while (!directLinks.empty() && !bGotUrl)
	{
		url = directLinks.first();
		directLinks.removeFirst();
		if (checkedLinks.find(url.toString()) == checkedLinks.end())
		{
			bGotUrl = true;
		}
	}

	while (!siteLinks.empty() && !bGotUrl)
	{
		url = siteLinks.first();
		siteLinks.removeFirst();
		if (checkedLinks.find(url.toString()) == checkedLinks.end())
		{
			bGotUrl = true;
		}
	}

	while (!bGotUrl && currentGlobalSite < globalSiteLinks.size())
	{
		QString strUrl = globalSiteLinks[currentGlobalSite];
		url = strUrl.replace(QString("%WADNAME%"), seekedWad);
		++currentGlobalSite;
		if (!url.isEmpty())
		{
			bGotUrl = true;
		}
	}


	if (!bGotUrl)
	{
		emit noMoreSites();
		return;
	}

	checkedLinks.insert(url.toString());

	QString strNotice = tr("Next site: %1").arg(url.toString());
	emit notice(strNotice);

	if (Http::isHTTPLink(url))
	{
		http.get(url);
	}
	else if (Ftp::isFTPLink(url))
	{
		emit notice(tr("FTP sites not supported yet."));
		nextSite();
	}
}

void WWW::reset()
{
	bAbort = false;
	currentGlobalSite = 0;
	customSiteUsed = false;
	checkedLinks.clear();
	directLinks.clear();
	siteLinks.clear();
}

void WWW::sizeSlot(unsigned int s)
{
	emit size(s);
}

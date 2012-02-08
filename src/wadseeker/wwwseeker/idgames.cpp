//------------------------------------------------------------------------------
// idgames.cpp
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
#include "idgames.h"

#include "entities/waddownloadinfo.h"
#include "wwwseeker/htmlparser.h"
#include "wwwseeker/urlparser.h"

#include <QDebug>



Idgames::Idgames(const QString& idgamesPage)
{
	bIsAborting = false;
	currentPage = 1;
	idgamesBaseUrl = idgamesPage;
	pCurrentRequest = NULL;
	pNetworkAccessManager = new QNetworkAccessManager();
	seekedFile = new WadDownloadInfo("");
}

Idgames::~Idgames()
{
	delete seekedFile;
	pNetworkAccessManager->deleteLater();
}

void Idgames::abort()
{
	if (!bIsAborting)
	{
		if (pCurrentRequest != NULL)
		{
			bIsAborting = true;

			pCurrentRequest->abort();
		}
		else
		{
			emit finished(this);
		}
	}
}

void Idgames::afterProcess(PageProcessResults result, const QUrl& url)
{
	QByteArray nul;
	switch (result)
	{
		case NotIdgames:
			emit message(tr("%1 is not Idgames archive! Aborting.").arg(idgamesBaseUrl), WadseekerLib::Error);
			abort();
			break;

		case StringTooShort:
			emit message(tr("Idgames: String \"%1\" is too short. Aborting.").arg(zipName()), WadseekerLib::Error);
			abort();
			break;

		case NoPositions:
			emit message(tr("Idgames: File \"%1\" not found.").arg(zipName()), WadseekerLib::Notice);
			emit finished(this);
			break;

		case Ok:
			// If url is empty we move to next page
			// if it's not, we get it and return the retrieved site
			// to WWWSeeker.
			if (url.isEmpty())
			{
				getNextPage();
			}
			else
			{
				filePageFound = true;
				startNetworkQuery(url);
			}
			break;
	}
}

QString Idgames::defaultIdgamesUrl()
{
	return "http://www.doomworld.com/idgames/index.php?search=1&page=%PAGENUM%&field=filename&word=%ZIPNAME%&sort=time&order=asc";
}


void Idgames::extractAndEmitLinks(QByteArray& pageData, const QUrl& pageUrl)
{
	// Get all <A HREFs> from HTML.
	HtmlParser html(pageData);
	QList<Link> links = html.linksFromHtml();

	// Extrat URLs of interest from <A HREFs>
	UrlParser urlParser(links);

	QStringList possibleFilenames;
	possibleFilenames << zipName();
	QList<Link> directLinks = urlParser.directLinks(possibleFilenames, pageUrl);
	
	
	if (!directLinks.isEmpty())
	{
		QList<QUrl> directUrls;
		foreach (const Link& link, directLinks)
		{
			directUrls << link.url;
		}
		
		emit fileLinksFound(this->seekedFile->name(), directUrls);
	}

	emit finished(this);
}

void Idgames::setFile(const WadDownloadInfo& wad)
{
	*seekedFile = wad;
}

void Idgames::startSearch()
{
	currentPage = 1;
	filePageFound = false;

	if (!idgamesBaseUrl.contains("%ZIPNAME%"))
	{
		emit message(tr("Idgames error: no %ZIPNAME% present in idgames url:\n%1").arg(idgamesBaseUrl), WadseekerLib::Error);
		emit finished(this);
		return;
	}

	if (seekedFile->name().isEmpty())
	{
		emit message(tr("Idgames error: Specified search filename is empty or invalid."), WadseekerLib::Error);
		emit finished(this);
		return;
	}

	emit message(tr("Searching Idgames archive for file: %1").arg(zipName()), WadseekerLib::NoticeImportant);

	getNextPage();
}

void Idgames::getNextPage()
{
	emit message(tr("Page %1...").arg(currentPage), WadseekerLib::Notice);
	QString tmpUrl = idgamesBaseUrl;
	QUrl url = tmpUrl.replace("%PAGENUM%", QString::number(currentPage)).replace("%ZIPNAME%", zipName());
	++currentPage;
	startNetworkQuery(url);
}

void Idgames::networkRequestFinished()
{
	if (pCurrentRequest == NULL)
	{
		return;
	}

	QByteArray pageData = pCurrentRequest->readAll();
	QUrl pageUrl = pCurrentRequest->url();

	emit siteFinished(pageUrl);

	// Clean up to accept new requests.
	pCurrentRequest->deleteLater();
	pCurrentRequest = NULL;

	if (bIsAborting)
	{
		emit finished(this);
	}
	else
	{
		if (filePageFound)
		{
			// WAD page was found. Let's extract links.
			extractAndEmitLinks(pageData, pageUrl);
		}
		else
		{
			// We're still looking for the WAD page.
			QUrl filePageUrl;

			PageProcessResults result = processPage(pageData, filePageUrl);
			afterProcess(result, filePageUrl);
		}
	}
}

void Idgames::networkRequestProgress(qint64 done, qint64 total)
{
	if (pCurrentRequest != NULL)
	{
		emit siteProgress(pCurrentRequest->url(), done, total);
	}
}

Idgames::PageProcessResults Idgames::processPage(QByteArray& pageData, QUrl& url)
{
	/*
		The code we are looking for here looks like this:
		<table class=wadlisting>
			<tr>
				<td class=wadlisting_name><a href=?id=1054>Castle to Hell</a>&nbsp;&nbsp;</td>
				<td class=wadlisting_label>Filename:</td>
				<td class=wadlisting_field>hellcast.zip</td>
			</tr>

		This repeats for each position displayed on site.
		However we also need to take into account other cases:
		a) there are no positions on the site
		b) string is too short
		c) this is not idgames site (multiple checks)
	*/

	url.clear();

	// Check if this is idgames (check <head> data)
	int indexOfHead = pageData.indexOf("<head>");
	int indexOfHeadEnd = pageData.indexOf("</head>");

	if (indexOfHead < 0 || indexOfHeadEnd < indexOfHead)
	{
		return NotIdgames;
	}

	QByteArray head = pageData.mid(indexOfHead, indexOfHeadEnd - indexOfHead);

	if (!head.contains("<title>Doomworld /idgames database</title>")
	||	!head.contains("<META http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">")
	||	!head.contains("<link rel=\"stylesheet\" type=\"text/css\" href=\"idgames.css\">"))
	{
		return NotIdgames;
	}

	// Now see if page contains any positions and whether or not it has "Too short string error":
	// Get data inbetween <input type="submit" value="mong"> and <div class="timer">
	int indexOfMong = pageData.indexOf("<input type=\"submit\" value=\"mong\">");
	int indexOfTimer = pageData.indexOf("<div class=\"timer\">");
	if (indexOfMong < 0 || indexOfTimer < indexOfMong)
	{
		// This is not idgames if either of above are not found
		return NotIdgames;
	}

	QByteArray filesData = pageData.mid(indexOfMong, indexOfTimer - indexOfMong);
	if (!filesData.contains("<table class=wadlisting>"))
	{
		if (filesData.contains("Need longer search string."))
		{
			return StringTooShort;
		}
		else
		{
			return NoPositions;
		}
	}

	// Now we can proceed to parsing filenames and retrieving URL's.
	const QString wadListing = "<table class=wadlisting>";
	const QString filenameField = "<td class=wadlisting_field>";
	const QString aHref = "<a href=";

	int indexOfWadlisting = filesData.indexOf(wadListing);
	while(indexOfWadlisting >= 0)
	{
		// The first wadlisting_field after this is the filename.

		int indexOfFilename = filesData.indexOf(filenameField, indexOfWadlisting) + filenameField.length();
		if (indexOfFilename < 0)
		{
			// Yet again, this is not Idgames archive
			return NotIdgames;
		}

		int indexOfFilenameEnd = filesData.indexOf("</td>", indexOfFilename);
		QString filename = filesData.mid(indexOfFilename, indexOfFilenameEnd - indexOfFilename);

		if (filename.compare(zipName(), Qt::CaseInsensitive) == 0)
		{
			// The file is found, save it's URL and return 'Ok'.
			int indexOfWadname = filesData.indexOf("<td class=wadlisting_name>", indexOfWadlisting);
			if (indexOfWadname < 0)
			{
				// Yet again, this is not Idgames archive
				return NotIdgames;
			}

			QByteArray positionData = filesData.mid(indexOfWadname, indexOfFilename - indexOfWadname);
			int indexOfHref = positionData.indexOf(aHref);
			if (indexOfHref < 0)
			{
				emit message(tr("Idgames error: File \"%1\" was found, but cannot locate link to it's page").arg(zipName()), WadseekerLib::Error);
				return NoPositions;
			}

			indexOfHref += aHref.length();

			int indexOfHrefEnd = positionData.indexOf(">", indexOfHref);
			QUrl href = QString(positionData.mid(indexOfHref, indexOfHrefEnd - indexOfHref));
			QUrl base = idgamesBaseUrl;

			if (href.scheme().isEmpty())
				href.setScheme(base.scheme());
			if (href.authority().isEmpty())
				href.setAuthority(base.authority());
			if (href.path().isEmpty())
				href.setPath(base.path());

			url = href.toEncoded();
			return Ok;
		}

		indexOfWadlisting = filesData.indexOf(wadListing, indexOfFilenameEnd);
	}

	return NoPositions;
}

void Idgames::startNetworkQuery(const QUrl& url)
{
	QNetworkRequest request;
	request.setUrl(url);
	request.setRawHeader("User-Agent", userAgent.toAscii());

	QNetworkReply* pReply = pNetworkAccessManager->get(request);
	pCurrentRequest = pReply;
	this->connect(pReply, SIGNAL( downloadProgress(qint64, qint64)),
		SLOT( networkRequestProgress(qint64, qint64) ) );
	this->connect(pReply, SIGNAL( finished() ),
		SLOT( networkRequestFinished() ) );

	emit siteStarted(url);
}

QString Idgames::zipName() const
{
	return seekedFile->archiveName("zip");
}

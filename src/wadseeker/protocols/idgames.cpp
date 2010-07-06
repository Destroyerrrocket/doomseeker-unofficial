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
#include <QDebug>

Idgames::Idgames(const QString& idgamesPage)
{
	idgamesBaseUrl = idgamesPage;
}

void Idgames::afterProcess(PageProcessResults result, const QString& url)
{
	QByteArray nul;
	switch (result)
	{
		case NotIdgames:
			emit message(tr("%1 is not Idgames archive! Aborting.").arg(idgamesBaseUrl), Wadseeker::Error);
			abort();
			break;

		case StringTooShort:
			emit message(tr("Idgames: String %1 is too short. Aborting.").arg(seekedFile), Wadseeker::Error);
			abort();
			break;

		case NoPositions:
			emit message(tr("Idgames: File not found."), Wadseeker::Notice);
			emit done(false, nul, 0, processedFileName);
			break;

		case Ok:
			// If url is empty we move to next page
			// if it's not, we get it and return the retrieved site
			// to WWWSeeker.
			if (url.isEmpty())
			{
				getPage();
			}
			else
			{
				filePageFound = true;
				get(url);
			}
			break;
	}
}

QString Idgames::defaultIdgamesUrl()
{
    return "http://www.doomworld.com/idgames/index.php?search=1&page=%PAGENUM%&field=filename&word=%ZIPNAME%&sort=time&order=asc";
}

void Idgames::doneEx(bool error)
{
	if (error)
	{
		emit message(tr("Idgames HTTP error: %1").arg(qHttp->errorString()), Wadseeker::Error);
		noData = true;
	}

	if (redirected)
	{
		emit redirect(redirectUrl);
		return;
	}

	if (aborting)
	{
		return;
	}

	if (noData)
	{
	    // Error
		QByteArray data = QByteArray();
		disconnectQHttp();
		emit done(false, data, 0, processedFileName);
	}
	else
	{
		QByteArray data = qHttp->readAll();
		disconnectQHttp();

		if (filePageFound)
		{
		    // We found the page and are returning it to be processed
		    // by WWWSeeker.
			emit message(tr("File %1 was found in Idgames archive!s").arg(seekedFile), Wadseeker::Notice);
            emit done(true, data, fileType, processedFileName);
		}
		else
		{
		    // Process the page.
		    QString newUrl;
			PageProcessResults result = processPage(data, newUrl);
			afterProcess(result, newUrl);
		}
	}
}

void Idgames::findFile(const QString& zipName)
{
    currentPage = 1;
    filePageFound = false;

    if (!idgamesBaseUrl.contains("%ZIPNAME%"))
    {
        QByteArray nul;
        emit message(tr("Idgames error: no %ZIPNAME% present in idgames url:\n%1").arg(idgamesBaseUrl), Wadseeker::Error);
        emit done(false, nul, 0, "");
        return;
    }

	seekedFile = zipName;
	emit message(tr("Searching Idgames archive for file: %1").arg(zipName), Wadseeker::NoticeImportant);

	getPage();
}

void Idgames::getPage()
{
	emit message(tr("Page %1...").arg(currentPage), Wadseeker::Notice);
	QString tmpUrl = idgamesBaseUrl;
	QUrl url = tmpUrl.replace("%PAGENUM%", QString::number(currentPage)).replace("%ZIPNAME%", seekedFile);
	++currentPage;
	get(url);
}

Idgames::PageProcessResults Idgames::processPage(QByteArray& pageData, QString& url)
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

		if (filename.compare(seekedFile, Qt::CaseInsensitive) == 0)
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
				emit message(tr("Idgames error: File %1 was found, but cannot locate link to it's page").arg(seekedFile), Wadseeker::Error);
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

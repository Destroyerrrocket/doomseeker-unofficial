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
	connect(&http, SIGNAL( aborted() ), this, SLOT( protocolAborted() ) );
	connect(&http, SIGNAL( dataReadProgress(int, int) ), this, SLOT( downloadProgressSlot(int, int) ) );
	connect(&http, SIGNAL( done(bool, QByteArray&, int, const QString&) ), this, SLOT( protocolDone(bool, QByteArray&, int, const QString&) ) );
	connect(&http, SIGNAL( message(const QString&, Wadseeker::MessageType) ), this, SLOT( messageSlot(const QString&, Wadseeker::MessageType) ) );
	connect(&http, SIGNAL( redirect(const QUrl&) ), this, SLOT( get(const QUrl&) ) );
}

void WWW::abort()
{
	aborting = true;
	http.abort();
}

void WWW::capitalizeHTMLTags(QByteArray& byte)
{
	int begin = 0;
	int end = 0;

	while (true)
	{
		char endingChar = ' ';
		bool bNext = false;
		bool bValueBegin = false;

		begin = findTag(byte, end, &end);
		if (begin < 0 || end < 0)
			break;

		for(int i = begin; i < end; ++i)
		{
			if (bNext)
			{
				if (byte[i] == endingChar)
				{
					bNext = false;
					bValueBegin = false;
				}
				continue;
			}
			else
			{
				if (bValueBegin)
				{
					if (byte[i] == '\"')
					{
						bNext = true;
						endingChar = '\"';
					}
					else if (byte[i] != ' ')
					{
						bNext = true;
						endingChar = ' ';
					}
				}
				else
				{
					if (byte[i] == '=')
					{
						bValueBegin = true;
					}
					else
					{
						byte[i] = toupper(byte[i]);
					}
				} // end of else
			} // end of else
		} // end of for
	} // end of while
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

int	 WWW::findTag(QByteArray& byte, int beginAt, int* end)
{
	if (end == NULL)
		return -1;

	int begin = -1;
	*end = -1;
	for (int i = beginAt; i < byte.length(); ++i)
	{
		if (byte[i] == '<')
		{
			begin = i;
			break;
		}
	}

	if (begin < 0)
		return -1;

	for (int i = begin; i < byte.length(); ++i)
	{
		if (byte[i] == '>')
		{
			*end = i;
			break;
		}
	}

	return begin;
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
		http.get(urlValid);
	}
	else
	{
		message(tr("Protocol for this site is not supported\n"), Wadseeker::Error);
		checkNextSite();
	}

}

bool WWW::hasFileReferenceSomewhere(const QStringList& wantedFileNames, const Link& link)
{
	QString strQuery = link.url.encodedQuery();

	for (int i = 0; i < wantedFileNames.count(); ++i)
	{
		if (strQuery.contains(wantedFileNames[i], Qt::CaseInsensitive) || link.text.contains(wantedFileNames[i], Qt::CaseInsensitive) )
		{
			return true;
		}
	}

	return false;
}

bool WWW::isDirectLinkToFile(const QStringList& wantedFileNames, const QUrl& link)
{
	QFileInfo fi(link.encodedPath());
	for (int i = 0; i < wantedFileNames.count(); ++i)
	{
		if (fi.fileName() == wantedFileNames[i])
		{
			return true;
		}
	}

	return false;
}

/**
 * Steps:
 * 1. Find an occurence of "<A " character sequence,
 *		if nothing found return.
 * 2. Find an occurence of ">" character starting
 *		from index returned by 1, if nothing found return.
 * 3. Find an occurence of " HREF" character sequence
 *		starting from index returned by 1.
 * 4. Check if HREF is between 1. and 2. and if there is ' ' or '='
 *		character after " HREF"	if not goto 1. starting
 *		from index returned by 2.
 * 5. Get HREF value.
 * 6. Find "</A>" character sequence starting from 2.
 *		if nothing found return.
 * 7. Get everything between 2. and 6.
 * 8. Append Link struct to list.
 * 9. Goto 1. starting from index returned by 6.
 */
QList<Link>	WWW::linksFromHTML(const QByteArray& data)
{
	QList<Link> list;

	int indexBeginTag = 0;
	while (true)
	{
		int indexCloseBracket = 0;
		int indexHref = 0;

		// 1
		indexBeginTag = data.indexOf("<A ", indexBeginTag);
		if (indexBeginTag < 0)
			break;

		// 2
		indexCloseBracket = data.indexOf(">", indexBeginTag);
		if (indexCloseBracket < 0)
			break;

		// 3
		indexHref = data.indexOf(" HREF", indexBeginTag) + 1;

		// 4
		int strLength = QString("HREF").length();
		if (indexHref > indexCloseBracket || (data[indexHref + strLength] != ' ' && data[indexHref + strLength] != '=') )
		{
			// next iteration
			indexBeginTag = indexCloseBracket;
		}
		else
		{
			// 5
			QString url = Html::htmlValue(data, indexHref, indexCloseBracket);

			// 6
			int indexEndA = 0;

			indexEndA = data.indexOf("</A>", indexCloseBracket);
			if (indexEndA < 0)
				break;

			// 7
			QString text = data.mid(indexCloseBracket + 1, indexEndA - (indexCloseBracket + 1) );

			// 8
			Link link = {url, text};
			list.append(link);

			// 9
			indexBeginTag = indexEndA;
		}
	}

	return list;
}

void WWW::linksFromHTMLByPattern(const QByteArray& data, const QStringList& wantedFiles)
{
	QList<Link> list = linksFromHTML(data);
	QList<Link>::iterator it;

	for (it = list.begin(); it != list.end(); ++it)
	{
		QUrl newUrl = it->url;
		if (it->url.authority().isEmpty())
		{
			newUrl.setAuthority("http");
		}

		if (it->url.host().isEmpty())
		{
			if (processedUrl.host().isEmpty())
				continue;

			newUrl.setHost(processedUrl.host());
		}

		if (it->url.host().isEmpty() && it->url.authority().isEmpty())
		{
			QString path = processedUrl.path();
			path = path.left(path.lastIndexOf('/') + 1);
			path += it->url.path();
			newUrl.setPath(path);
		}

		if (isDirectLinkToFile(wantedFiles, it->url))
		{
			directLinks.append(newUrl);
		}
		else if (hasFileReferenceSomewhere(wantedFiles, *it))
		{
			// here we append all links that contain this filename somewhere else than in path
			siteLinks.append(newUrl);
		}
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
}

void WWW::protocolDone(bool success, QByteArray& data, int fileType, const QString& filename)
{
	if (success)
	{
		emit message(tr("Got file %1.").arg(filename), Wadseeker::Notice);
		if (fileType == Protocol::Html)
		{
			emit message(tr("Parsing file as HTML looking for links.\n"), Wadseeker::Notice);
			capitalizeHTMLTags(data);
			linksFromHTMLByPattern(data, filesToFind);
			checkNextSite();
		}
		else
		{
			emit fileDone(data, filename);
		}
	}

	emit message(" ", Wadseeker::Notice);

}

void WWW::searchFiles(const QStringList& list, const QString& primaryFilename)
{
	aborting = false;
	customSiteUsed = false;
	currentPrimarySite = 0;
	checkedLinks.clear();

	filesToFind = list;
	primaryFile = primaryFilename;
	checkNextSite();
}



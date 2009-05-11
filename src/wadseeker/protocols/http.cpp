//------------------------------------------------------------------------------
// http.cpp
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
#include "http.h"
#include <QByteArray>
#include <cctype>


Http::Http()
{
	construct();
}

Http::~Http()
{

}

void Http::construct()
{
	Qt::ConnectionType ct = Qt::DirectConnection;

	dontSendFinishedReceiving = false;

	connect(&qHttp, SIGNAL( done(bool) ), this, SLOT( done(bool)), ct);
	connect(&qHttp, SIGNAL( responseHeaderReceived(const QHttpResponseHeader&) ), this, SLOT( headerReceived(const QHttpResponseHeader&)), ct);
	connect(&qHttp, SIGNAL( readyRead ( const QHttpResponseHeader& ) ), this, SLOT( read(const QHttpResponseHeader&)), ct);
}


void Http::abort()
{
	qHttp.abort();
	qHttp.close();
}

QString Http::attachmentInformation(const QHttpHeader& header, QString& filename)
{
	QString val = header.value("Content-Disposition");
	QStringList values = val.split(";");
	QStringList::iterator it;
	for (it = values.begin(); it != values.end(); ++it)
	{
		if (it->trimmed().compare("attachment") == 0)
		{
			++it;
			if (it == values.end())
				break;

			QString ret = it->trimmed();
			QByteArray asciiStr = ret.toAscii();
			filename = htmlValue(asciiStr, "filename");
			return ret;
		}
	}

	return QString();
}

void Http::capitalizeTags(QByteArray& byte)
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

void Http::done(bool error)
{
	if (fileType == HTTP_FILE_TYPE_HTML)
	{
		capitalizeTags(data);
	}

	if (!dontSendFinishedReceiving)
	{
		if (error)
		{
			emit finishedReceiving(qHttp.errorString());
		}
		else
		{
			emit finishedReceiving(QString());
		}
	}
	else
	{
		dontSendFinishedReceiving = false;
	}
}

int	 Http::findTag(QByteArray& byte, int beginAt, int* end)
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

bool Http::hasFileReferenceSomewhere(const QStringList& wantedFileNames, const Link& link)
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

void Http::headerReceived(const QHttpResponseHeader& resp)
{
	QFileInfo fi(resource);
	QString attachmentInfo;
	QString attachmentFilename;
	QUrl url;

	dontSendFinishedReceiving = false;
	switch (resp.statusCode())
	{
		case STATUS_REDIRECT:
			// first we determine whether this is attachment redirect
			// (like the one from doom.dogsoft.net/getwad.php
			attachmentInfo = attachmentInformation(resp, attachmentFilename);

			if (!attachmentInfo.isEmpty() && isWantedFile(attachmentFilename))
			{
				fileType = HTTP_FILE_TYPE_WANTED;
				emit notice(tr("Downloading attached file: %1").arg(attachmentFilename));
				emit nameOfCurrentlyDownloadedResource(attachmentFilename);
			}
			else
			{
				fileType = HTTP_FILE_TYPE_HTML;
				dontSendFinishedReceiving = true;
				url = resp.value("Location");
				emit notice( tr("Redirecting to: %1").arg(url.toString()) );
				get(url);
			}
			break;

		case STATUS_OK:
			if (isWantedFile(fi))
			{
				fileType = HTTP_FILE_TYPE_WANTED;
			}
			else if (isHTMLFile(resp))
			{
				fileType = HTTP_FILE_TYPE_HTML;
			}
			else
			{
				fileType = HTTP_FILE_TYPE_UNKNOWN;
				url = site.toString() + resource;
				emit (finishedReceiving(tr("%1 is neither an expected file nor a HTML file. Ignoring.").arg(url.toString() )) );
				return;
			}

			break;

		default:
			emit error(QString::number(resp.statusCode()) + " - " + resp.reasonPhrase());
			break;
	}

	if (resp.hasContentLength())
	{
		sizeMax = resp.contentLength();
		emit size(sizeMax);
	}
	sizeCur = 0;
}

QString Http::htmlValue(const QByteArray& byte, int beginIndex, int endIndex)
{
	int indexStartValue = -1;
	int indexEndValue = -1;

	for (; beginIndex < endIndex; ++beginIndex)
	{
		if (byte[beginIndex] == '=')
		{
			++beginIndex;
			break;
		}
	}

	char endingChar = ' ';
	for (int i = beginIndex; i < endIndex; ++i)
	{
		if (indexStartValue < 0)
		{
			if (byte[i] == '\"')
			{
				indexStartValue = i + 1;
				endingChar = '\"';
			}
			else if (byte[i] == ' ')
			{
				continue;
			}
			else
			{
				indexStartValue = i;
			}
		}
		else
		{
			if (byte[i] == endingChar )
			{
				indexEndValue = i - 1;
				break;
			}
		}
	}

	if (indexStartValue < 0)
		return QString();

	if (indexEndValue < 0)
	{
		if (byte[endIndex] == '>')
			indexEndValue = endIndex - 1;
		else
			return QString();
	}

	QString ret = byte.mid(indexStartValue, indexEndValue - indexStartValue + 1);
	return ret;
}

QString Http::htmlValue(const QByteArray& byte, const QString& key)
{
	QByteArray upperByte = byte.toUpper();
	QString upperKey = key.toUpper();

	int beginIndex = upperByte.indexOf(upperKey);

	if (beginIndex < 0)
		return QString();

	for (; beginIndex < byte.size(); ++beginIndex)
	{
		if (byte[beginIndex] == '=')
		{
			++beginIndex;
			break;
		}
	}

	int indexStartValue = -1;
	int indexEndValue = byte.size() - 1;

	char endingChar = ' ';
	for (int i = beginIndex; i < byte.size(); ++i)
	{
		if (indexStartValue < 0)
		{
			if (byte[i] == '\"')
			{
				indexStartValue = i + 1;
				endingChar = '\"';
			}
			else if (byte[i] == ' ')
			{
				continue;
			}
			else
			{
				indexStartValue = i;
			}
		}
		else
		{
			if (byte[i] == endingChar )
			{
				indexEndValue = i - 1;
				break;
			}
		}
	}

	if (indexStartValue < 0)
		return QString();

	QString ret = byte.mid(indexStartValue, indexEndValue - indexStartValue + 1);
	return ret;
}

bool Http::isDirectLinkToFile(const QStringList& wantedFileNames, const QUrl& link)
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

bool Http::isHTMLFile(const QHttpHeader& http)
{
	if (!http.hasKey("Content-type"))
	{
		return false;
	}

	QString contentType = http.value("Content-type");

	if (contentType.contains("text/html", Qt::CaseInsensitive))
	{
		return true;
	}

	return false;
}

bool Http::isHTTPLink(const QUrl& url)
{
	const QString& scheme = url.scheme();
	if(scheme.isEmpty() || scheme.compare("http", Qt::CaseInsensitive) == 0)
		return true;

	return false;
}

bool Http::isWantedFile(const QFileInfo& fi)
{
	QStringList::iterator it;
	QString file = fi.fileName();
	for (it = wantedFilenames.begin(); it != wantedFilenames.end(); ++it)
	{
		if (file.compare(*it, Qt::CaseInsensitive) == 0)
			return true;
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

QList<Link>	Http::links()
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
			QString url = htmlValue(data, indexHref, indexCloseBracket);

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

void Http::linksByPattern(const QStringList& wantedFileNames, QList<QUrl>& directLinks, QList<QUrl>& siteLinks)
{
	QList<Link> list = links();
	QList<Link>::iterator it;

	for (it = list.begin(); it != list.end(); ++it)
	{
		if (isHTTPLink(it->url))
		{
			QString strUrl;
			if (it->url.authority().isEmpty())
			{
				QStringList path = resource.split('/');

				strUrl = site.toString();
				if (strUrl[strUrl.length() - 1] != '/')
					strUrl += '/';

				for (int i = 0; i < path.size() - 1; ++i)
				{
					if (!path[i].isEmpty())
					{
						strUrl += path[i] + '/';
					}
				}
			}
			else
			{
				strUrl = "";
			}

			QUrl newUrl(strUrl + it->url.toString());

			if (isDirectLinkToFile(wantedFileNames, it->url))
			{
				directLinks.append(newUrl);
			}
			else if (hasFileReferenceSomewhere(wantedFileNames, *it))
			{
				// here we append all links that contain this filename somewhere else than in path
				siteLinks.append(newUrl);
			}
		}
	}
}

void Http::sendGet()
{
	qHttp.setHost(this->site.encodedHost(), this->site.port(80));
	qHttp.get(this->resource);
}

void Http::setWantedFilenames(const QStringList& list)
{
	wantedFilenames = list;
}

void Http::read(const QHttpResponseHeader& httpResp)
{
	responseCode = httpResp.statusCode();
	responsePhrase = httpResp.reasonPhrase();

	QByteArray newData = qHttp.readAll();
	sizeCur += newData.length();
	data.append(newData);

	int percent = 0;
	if (sizeMax != 0)
	{
		percent = sizeCur * 100 / sizeMax;
		if ((sizeMax % sizeCur) != 0)
			++percent;
	}

	emit dataReceived(newData.length(), sizeCur, percent);
}

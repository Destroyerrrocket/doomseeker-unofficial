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

QStringList Http::htmlFileExtensions;

Http::Http()
{
	construct();
}

Http::Http(QString s)
{
	construct();

	setSite(s);
}

Http::~Http()
{
	construct();
}

void Http::construct()
{
	Qt::ConnectionType ct = Qt::DirectConnection;

	connect(&qHttp, SIGNAL( done(bool) ), this, SLOT( done(bool)), ct);
	connect(&qHttp, SIGNAL( responseHeaderReceived(const QHttpResponseHeader&) ), this, SLOT( headerReceived(const QHttpResponseHeader&)), ct);
	connect(&qHttp, SIGNAL( readyRead ( const QHttpResponseHeader& ) ), this, SLOT( read(const QHttpResponseHeader&)), ct);

	if (htmlFileExtensions.isEmpty())
	{
		htmlFileExtensions << "html" << "htm" << "php" << "php4" << "php5";
	}
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
			emit finishedReceiving(qHttp.errorString());
		else
			emit finishedReceiving(QString());
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

void Http::headerReceived(const QHttpResponseHeader& resp)
{
	QUrl url;
	switch (resp.statusCode())
	{
		case STATUS_REDIRECT:
			dontSendFinishedReceiving = true;
			url = resp.value("Location");
			setSite(url.authority());
			sendRequestGet(url.encodedPath() + url.encodedQuery());
			break;

		case STATUS_OK:
			dontSendFinishedReceiving = false;
			sizeMax = resp.contentLength();
			sizeCur = 0;
			emit size(sizeMax);
			break;

		default:
			dontSendFinishedReceiving = false;
			emit error(QString::number(resp.statusCode()) + " - " + resp.reasonPhrase());
			break;
	}
}

bool Http::isBinaryFile(const QFileInfo& fi)
{
	QStringList::iterator it;
	QString extension = fi.suffix();
	for (it = binaryFileExtensions.begin(); it != binaryFileExtensions.end(); ++it)
	{
		if (extension.compare(*it, Qt::CaseInsensitive) == 0)
			return true;
	}

	return false;
}

bool Http::isHTMLFile(const QFileInfo& fi)
{
	if (fi.fileName().isEmpty())
		return true;

	QStringList::iterator it;
	QString extension = fi.suffix();
	for (it = htmlFileExtensions.begin(); it != htmlFileExtensions.end(); ++it)
	{
		if (extension.compare(*it, Qt::CaseInsensitive) == 0)
			return true;
	}

	return false;
}

QUrl Http::lastLink() const
{
	return QUrl(site + resource);
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

QString Http::htmlValue(QByteArray& byte, int beginIndex, int endIndex)
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

void Http::sendRequestGet(QString resource)
{
	QFileInfo fi(resource);

	if (isBinaryFile(fi))
	{
		fileType = HTTP_FILE_TYPE_BINARY;
		qDebug() << "Receiving binary file!";
	}
	else if (isHTMLFile(fi))
	{
		fileType = HTTP_FILE_TYPE_HTML;
		qDebug() << "Receiving HTML file!";
	}
	else
	{
		fileType = HTTP_FILE_TYPE_BINARY;
		emit (finishedReceiving(site + resource + " will not be processed"));
		return;
	}

	data.clear();
	this->resource = resource;
	qHttp.get(resource);
}

void Http::setBinaryFilesExtensions(const QStringList& list)
{
	binaryFileExtensions = list;
}

void Http::setSite(const QString& s)
{
	site = s;
	qHttp.setHost(s);
}

void Http::read(const QHttpResponseHeader& httpResp)
{
	/*
	#ifdef QT_DEBUG
	qDebug() << "========================================================";
	qDebug() << "= Responded Responded Responded Responded Responded Re =";
	qDebug() << "= Status code:" << httpResp.statusCode() << httpResp.reasonPhrase() << "=";
	qDebug() << "========================================================";
	#endif
	*/

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

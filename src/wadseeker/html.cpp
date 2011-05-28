//------------------------------------------------------------------------------
// html.cpp
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

#include <QFileInfo>

CHtml::CHtml(QByteArray& data)
{
	this->data = data;
}

void CHtml::capitalizeHTMLTags()
{
	QByteArray& byte = data;
	int begin = 0;
	int end = 0;

	while (true)
	{
		char endingChar = ' ';
		bool bNext = false;
		bool bValueBegin = false;

		begin = findTag(end, &end);
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

int	 CHtml::findTag(int beginAt, int* end)
{
	if (end == NULL)
		return -1;

	QByteArray& byte = data;
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

bool CHtml::hasFileReferenceSomewhere(const QStringList& wantedFileNames, const Link& link)
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

QString CHtml::htmlValue(int beginIndex, int endIndex)
{
	const QByteArray& byte = this->data;
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

QString CHtml::htmlValue(const QString& key)
{
	const QByteArray& byte = this->data;
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

bool CHtml::isDirectLinkToFile(const QStringList& wantedFileNames, const QUrl& link)
{
	QFileInfo fi(link.encodedPath());
	for (int i = 0; i < wantedFileNames.count(); ++i)
	{
		if (fi.fileName().compare(wantedFileNames[i], Qt::CaseInsensitive) == 0)
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
 * 8. Append Link class to list.
 * 9. Goto 1. starting from index returned by 6.
 */
QList<Link>	CHtml::linksFromHTML()
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
			QString url = htmlValue(indexHref, indexCloseBracket);

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

void CHtml::linksFromHTMLByPattern(const QStringList& wantedFiles, QList<QUrl>& siteLinks, QList<QUrl>& directLinks, const QUrl& baseUrl, int& siteLinksOut, int& directLinksOut)
{
	siteLinksOut = 0;
	directLinksOut = 0;
	QList<Link> list = linksFromHTML();
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
			if (baseUrl.host().isEmpty())
				continue;

			newUrl.setHost(baseUrl.host());
		}

		if (it->url.port() < 0 && baseUrl.port() >= 0 && baseUrl.port() != 80)
		{
			newUrl.setPort(baseUrl.port());
		}

		// If the path in the processed url starts with '/' do not make any
		// changes to the path in the currently created URL.
		if (it->url.host().isEmpty() && it->url.authority().isEmpty() && !it->url.path().startsWith('/'))
		{
			QString path = baseUrl.path();
			path = path.left(path.lastIndexOf('/') + 1);
			path += it->url.path();
			newUrl.setPath(path);
		}


		if (isDirectLinkToFile(wantedFiles, it->url))
		{
			directLinks.append(newUrl);
			++directLinksOut;
		}
		else if (hasFileReferenceSomewhere(wantedFiles, *it))
		{
			// here we append all links that contain this filename somewhere else than in path
			siteLinks.append(newUrl);
			++siteLinksOut;
		}
	}
}

//------------------------------------------------------------------------------
// html.h
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
#ifndef __HTML_H_
#define __HTML_H_

#include "link.h"
#include "wadseeker.h"
#include <QByteArray>
#include <QString>

class CHtml
{
	public:
		CHtml(QByteArray&);

		/**
		 * Capitalizes all HTML keywords
		 */
		void		capitalizeHTMLTags();

		/**
		 * Finds a HTML tag starting from index in the byte array.
		 * @param beginAt 	- index from which searching starts
		 * @param[out] end 	- end index of a tag
		 * @return 			- begin index of a tag
		 */
		int			findTag(int beginAt, int* end);

		/**
		 * You put something like HREF="http://127.0.0.1" and it retrieves the thing after '=' without the "".
		 * @param beginIndex 	- index from which parsing starts
		 * @param endIndex		- index at which parsing ends
		 * @return 				- trimmed value, without white-spaces and quotes.
		 */
		QString		htmlValue(int beginIndex, int endIndex);

		/**
		 * You put a string of values, for example <A HREF="http://127.0.0.1/" TARGET="_blank"> and it
		 * retrieves the value after specified key.
		 * @param key			- key that will be searched for (for example: HREF)
		 */
		QString		htmlValue(const QString& key);

		/**
		 * Extracts links from HTML file.
		 */
		QList<Link> linksFromHTML();

		/**
		 * Extracts links from HTML file but only those that match "pattern".
		 * @param wantedFiles 	- 	names of the files we want to get the links to.
		 * @param siteLinks		- 	indirect links to wanted files will be appended here
		 * @param directLinks	- 	direct links to wanted files will be appended here
		 * @param baseUrl		- 	relative URLs will be converted to absolute
		 *							URLs using this URL as a base
		 */
		void		linksFromHTMLByPattern(const QStringList& wantedFiles, QList<QUrl>& siteLinks, QList<QUrl>& directLinks, const QUrl& baseUrl);

	protected:
		static bool			hasFileReferenceSomewhere(const QStringList& wantedFileNames, const Link& link);
		static bool			isDirectLinkToFile(const QStringList& wantedFileNames, const QUrl& link);

		QByteArray	data;
};

#endif

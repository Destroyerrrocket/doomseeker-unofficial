//------------------------------------------------------------------------------
// html.h
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
#ifndef __HTML_H_
#define __HTML_H_

#include "wadseeker.h"
#include <QByteArray>
#include <QString>

class WADSEEKER_API Html
{
	public:
		/**
		 * You put something like HREF="http://127.0.0.1" and it retrieves the thing after '=' without the "".
		 * @param byte			- array that will be searched
		 * @param beginIndex 	- index from which parsing starts
		 * @param endIndex		- index at which parsing ends
		 * @return 				- trimmed value, without white-spaces and quotes.
		 */
		static QString	htmlValue(const QByteArray& byte, int beginIndex, int endIndex);

		/**
		 * You put a string of values, for example <A HREF="http://127.0.0.1/" TARGET="_blank"> and it
		 * retrieves the value after specified key.
		 * @param byte			- array that will be searched
		 * @param key			- key that will be searched for (for example: HREF)
		 */
		static QString	htmlValue(const QByteArray& byte, const QString& key);
};

#endif

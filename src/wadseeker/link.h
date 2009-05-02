//------------------------------------------------------------------------------
// link.h
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
#ifndef __LINK_H_
#define __LINK_H_

#include <Qt>
#include <QUrl>

struct Link
{
	QUrl 		url;
	QString 	text;

	bool 		pathEndsWith(const QStringList& ends);
	bool		isHttpLink();
	/**
	 *	@param comparePage		- if not empty checks if URL refers to the same host as this param
	 *	@return true if URL points to another server
	 */
	bool		isRemote(const QUrl& comparePage);

	/**
	 *	@return true if the URL refers to the same page (for example URLs with '#')
	 */
	bool		isTheSamePage(const QUrl& comparePage);

	/**
	 * @return true if URL begins from javascript: phrase
	 */
	bool		isJavascriptURL();
};

#endif

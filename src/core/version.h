//------------------------------------------------------------------------------
// version.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __VERSION_H__
#define __VERSION_H__

#include <QString>
#include "global.h"

/**
 *	@brief Provides interface for plugins and for Doomseeker itself
 *	to obtain version information on the program.
 */
class MAIN_EXPORT Version
{
	public:
		/**
		 *	Returns Mercurial changeset or an empty string if not available.
		 */
		static QString			changeset();

		/**
		 *	@brief Combines program's name and versionRevision().
		 */
		static QString			fullVersionInfo() { return name() + " " + versionRevision(); }
		static QString			name();
		static QString			revision();
		static int				revisionNumber();

		/**
		 * @brief WWW User Agent used for HTTP communications.
		 */
		static QString			userAgent();

		static QString			version();

		/**
		 *	@brief Combines version and revision strings.
		 *	If revision is not available only version is returned.
		 */
		static QString			versionRevision();


};

#endif

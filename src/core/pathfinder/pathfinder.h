//------------------------------------------------------------------------------
// pathfinder.h
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

#ifndef __PATHFINDER_H_
#define __PATHFINDER_H_

#include "pathfinder/filesearchpath.h"
#include "global.h"
#include <QStringList>

class IniSection;
class IniVariable;

/**
 * @brief Result of multiple file search operation done by PathFinder.
 *
 * This object is copyable.
 */
class MAIN_EXPORT PathFinderResult
{
	public:
		PathFinderResult();
		COPYABLE_D_POINTERED_DECLARE(PathFinderResult);
		virtual ~PathFinderResult();

		/**
		 * @brief Paths to found files.
		 */
		QStringList& foundFiles();
		const QStringList& foundFiles() const;

		/**
		 * @brief Names of not found files.
		 */
		QStringList& missingFiles();
		const QStringList& missingFiles() const;

	private:
		class PrivData;
		PrivData *d;
};

/**
 * @brief Performs a case-insensitive (OS independent) file searches.
 *
 * This object is copyable.
 *
 * On case-insensitive file systems (like NTFS) the search is simple
 * and doesn't differ from what one would expect of the system tools to do.
 *
 * On case-sensitive file systems (Linux FS-es) the search goes against
 * the principles of the OS and treats files with alternating letter case
 * the same. So, on Linux doom2.wad, DOOM2.WAD and doom2.WAD are all treated
 * as equal. If more than one file is present in a searched directory, only
 * one path is returned and it's undefined which one will it be.
 *
 * Search is performed in a set of paths retrieved from configuration
 * or specified manually through constructor and addPrioritySearchDir().
 */
class MAIN_EXPORT PathFinder
{
	public:
		/**
		 * @brief Constructs PathFinder where paths are read from program
		 *        configuration.
		 */
		PathFinder();
		/**
		 * @brief Constructs PathFinder with custom paths.
		 *
		 * Program configuration is skipped here.
		 */
		PathFinder(const QStringList& paths);
		// [Zalewa] This may seem strange, but I don't see any reason why
		// PathFinder can't be copyable. All in all, it's just a set of paths.
		COPYABLE_D_POINTERED_DECLARE(PathFinder);
		virtual ~PathFinder();

		/**
		 * Provides a directory where we should search first before going to
		 * user specified locations.  This function can take either a directory
		 * or a file as its input.  If a file is given the directory part will
		 * be extracted.
		 */
		void addPrioritySearchDir(const QString& dir);
		/**
		 * @brief Performs a search for a single file.
		 */
		QString findFile(const QString& fileName) const;
		/**
		 * @brief Performs a search for multiple files, marking them as found
		 *        or missing.
		 */
		PathFinderResult findFiles(const QStringList& files) const;

	private:
		class PrivData;
		PrivData *d;
};

#endif

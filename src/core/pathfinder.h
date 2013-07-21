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

#include "global.h"
#include <QStringList>

class IniSection;
class IniVariable;

class MAIN_EXPORT PathFinderResult
{
	public:
		QStringList		foundFiles;
		QStringList		missingFiles;
};

class MAIN_EXPORT PathFinder
{
	public:
		PathFinder();
		PathFinder(const QString&);

		/**
		 * Provides a directory where we should search first before going to
		 * user specified locations.  This function can take either a directory
		 * or a file as its input.  If a file is given the directory part will
		 * be extracted.
		 */
		void				addPrioritySearchDir(const QString& dir);
		QString 			findFile(const QString& fileName) const;
		PathFinderResult	findFiles(const QStringList& files) const;

	protected:
		QStringList			pathList;
};

#endif

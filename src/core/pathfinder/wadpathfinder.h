//------------------------------------------------------------------------------
// wadpathfinder.h
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef idCA954A5A_EF00_4F51_9C9782634F191143
#define idCA954A5A_EF00_4F51_9C9782634F191143

#include "dpointer.h"
#include <QString>
#include <QStringList>

class PathFinder;

class WadFindResult
{
	public:
		WadFindResult();
		COPYABLE_D_POINTERED_DECLARE(WadFindResult);
		virtual ~WadFindResult();

		const QString &alias() const;
		void setAlias(const QString &val);

		bool isAlias() const;
		bool isValid() const;

		const QString &path() const;
		void setPath(const QString &val);

	private:
		class PrivData;
		PrivData *d;
};

/**
 * @brief Wrapper for PathFinder that specializes in findings WADs.
 */
class WadPathFinder
{
	public:
		WadPathFinder(const PathFinder &pathFinder);
		~WadPathFinder();

		WadFindResult find(const QString &name);

	private:
		class PrivData;
		PrivData *d;

		QStringList aliases(const QString &name) const;
};

#endif

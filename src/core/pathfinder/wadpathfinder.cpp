//------------------------------------------------------------------------------
// wadpathfinder.cpp
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
#include "wadpathfinder.h"

#include "configuration/doomseekerconfig.h"
#include "pathfinder/filealias.h"
#include "pathfinder/pathfinder.h"

DClass<WadFindResult>
{
	public:
		QString alias;
		QString path;
};

DPointered(WadFindResult)

WadFindResult::WadFindResult()
{
}

WadFindResult::~WadFindResult()
{
}

const QString &WadFindResult::alias() const
{
	return d->alias;
}

void WadFindResult::setAlias(const QString &val)
{
	d->alias = val;
}

bool WadFindResult::isAlias() const
{
	return isValid() && !d->alias.isEmpty();
}

bool WadFindResult::isValid() const
{
	return !d->path.isEmpty();
}

const QString &WadFindResult::path() const
{
	return d->path;
}

void WadFindResult::setPath(const QString &val)
{
	d->path = val;
}
///////////////////////////////////////////////////////////////////////////////
DClass<WadPathFinder>
{
	public:
		QList<FileAlias> aliases;
		PathFinder pathFinder;
};

DPointered(WadPathFinder)

WadPathFinder::WadPathFinder(const PathFinder &pathFinder)
{
	d->aliases = gConfig.doomseeker.wadAliases();
	d->pathFinder = pathFinder;
}

WadPathFinder::~WadPathFinder()
{
}

QStringList WadPathFinder::aliases(const QString &name) const
{
	foreach (const FileAlias &candidate, d->aliases)
	{
		if (candidate.name().compare(name, Qt::CaseInsensitive) == 0)
		{
			return candidate.aliases();
		}
	}
	return QStringList();
}

WadFindResult WadPathFinder::find(const QString &name)
{
	{
		QString path = d->pathFinder.findFile(name);
		if (!path.isEmpty())
		{
			WadFindResult result;
			result.setPath(path);
			return result;
		}
	}
	foreach (const QString &alias, aliases(name))
	{
		QString path = d->pathFinder.findFile(alias);
		if (!path.isEmpty())
		{
			WadFindResult result;
			result.setPath(path);
			result.setAlias(alias);
			return result;
		}
	}
	return WadFindResult();
}

//------------------------------------------------------------------------------
// basefileseeker.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "basefileseeker.h"

#include "pathfinder/filesearchpath.h"
#include <QDir>

QString BaseFileSeeker::findFile(const QString& fileName, const QList<FileSearchPath>& paths)
{
	foreach (const FileSearchPath& candidate, paths)
	{
		QString result = findFileInPath(fileName, candidate);
		if (!result.isNull())
		{
			return result;
		}
	}
	return QString();
}

QString BaseFileSeeker::findFileInPath(const QString& fileName, const FileSearchPath& path)
{
	QString result = findFileInSpecifiedDirectory(fileName, path.path());
	if (!result.isNull())
	{
		return result;
	}

	if (path.isRecursive())
	{
		QDir dir(path.path());
		QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		foreach (const QString& subDir, subDirs)
		{
			FileSearchPath subSearchPath(dir.filePath(subDir));
			subSearchPath.setRecursive(true);
			QString result = findFileInPath(fileName, subSearchPath);
			if (!result.isNull())
			{
				return result;
			}
		}
	}
	return QString();
}

//------------------------------------------------------------------------------
// pathfinder.cpp
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
#include "pathfinder.h"

#include "configuration/doomseekerconfig.h"
#include "pathfinder/caseinsensitivefsfileseeker.h"
#include "pathfinder/casesensitivefsfileseeker.h"
#include "pathfinder/filesearchpath.h"
#include "datapaths.h"
#include "log.h"
#include "strings.hpp"
#include <QDir>
#include <QFileInfo>
#include <cstdlib>

DClass<PathFinderResult>
{
	public:
		QStringList foundFiles;
		QStringList missingFiles;
};


DPointered(PathFinderResult)


PathFinderResult::PathFinderResult()
{
}

PathFinderResult::~PathFinderResult()
{
}

QStringList& PathFinderResult::foundFiles()
{
	return d->foundFiles;
}

const QStringList& PathFinderResult::foundFiles() const
{
	return d->foundFiles;
}

QStringList& PathFinderResult::missingFiles()
{
	return d->missingFiles;
}

const QStringList& PathFinderResult::missingFiles() const
{
	return d->missingFiles;
}

////////////////////////////////////////////////////////////////////////////////

DClass<PathFinder>
{
	public:
		QList<FileSearchPath> searchPaths;

		QString resolveDir(const QString &dir)
		{
			QFileInfo fileInfo(dir);
			if(fileInfo.isSymLink())
				fileInfo = QFileInfo(fileInfo.symLinkTarget());

			if(fileInfo.isBundle())
				return fileInfo.absoluteFilePath() + "/Contents/MacOS";
			else
			{
				if(fileInfo.isFile())
					return fileInfo.absoluteDir().absolutePath();
				else
					return fileInfo.absoluteFilePath();
			}
		}
};


DPointered(PathFinder)


PathFinder::PathFinder()
{
	d->searchPaths = gConfig.combinedWadseekPaths();
}

PathFinder::PathFinder(const QStringList& paths)
{
	foreach (const QString& path, paths)
	{
		d->searchPaths << path;
	}
}

PathFinder::~PathFinder()
{
}

PathFinder PathFinder::genericPathFinder(const QStringList &suffixes)
{
	QStringList paths;
#if defined(Q_OS_WIN32)
	paths << "." << ".."
		<< gDefaultDataPaths->workingDirectory()
		<< gDefaultDataPaths->workingDirectory() + "/.."
		<< DataPaths::programFilesDirectory(DataPaths::x64)
		<< DataPaths::programFilesDirectory(DataPaths::x86);
#else
	paths << "/usr/bin" << "/usr/local/bin" << "/usr/share/bin"
		<< "/usr/games/" << "/usr/local/games/"
		<< "/usr/share/games/" << gDefaultDataPaths->workingDirectory() << ".";
#endif
	QStringList pathsCopy(paths);
	foreach (const QString &path, pathsCopy)
	{
		foreach (const QString &suffix, suffixes)
		{
			paths << Strings::combinePaths(path, suffix);
		}
	}
	return PathFinder(paths);
}

void PathFinder::addPrioritySearchDir(const QString& dir)
{
	d->searchPaths.prepend(d->resolveDir(dir));
}

void PathFinder::addSearchDir(const QString &dir)
{
	d->searchPaths << d->resolveDir(dir);
}

QString PathFinder::findFile(const QString& fileName) const
{
	if (d->searchPaths.count() == 0)
	{
		return QString();
	}

	BaseFileSeeker* seeker = NULL;
	#ifdef Q_OS_WIN32
	seeker = new CaseInsensitiveFSFileSeeker();
	#else
	seeker = new CaseSensitiveFSFileSeeker();
	#endif
	QString result = seeker->findFile(fileName, d->searchPaths);
	delete seeker;
	return result;
}

PathFinderResult PathFinder::findFiles(const QStringList& files) const
{
	PathFinderResult result;
	foreach(const QString file, files)
	{
		QString filePath = findFile(file);
		if (filePath.isNull())
		{
			result.missingFiles() << file;
		}
		else
		{
			result.foundFiles() << filePath;
		}
	}

	return result;
}

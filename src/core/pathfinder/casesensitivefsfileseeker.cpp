//------------------------------------------------------------------------------
// casesensitivefsfileseeker.cpp
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
#include "casesensitivefsfileseeker.h"

#include "pathfinder/filesearchpath.h"
#include <QDir>
#include <QFileInfo>
#include <QStringList>

QString CaseSensitiveFSFileSeeker::findFileInSpecifiedDirectory(const QString& fileName,
	const QString& dirPath)
{
	QStringList filterList;
	filterList << fileName;
	QDir dir(dirPath);
	QFileInfoList fiList = dir.entryInfoList(filterList, QDir::Files);
	foreach (const QFileInfo& fileInfo, fiList)
	{
		QString candidate = fileInfo.fileName();
		if (candidate.compare(fileName, Qt::CaseInsensitive) == 0)
		{
			return fileInfo.absoluteFilePath();
		}
	}
	return QString();
}

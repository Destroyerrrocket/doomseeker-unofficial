//------------------------------------------------------------------------------
// filefind.cpp
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
// Copyright (C) 2017 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "filefind.h"

#include <QFileInfo>
#include <QSet>
#include <QStringList>

DClass<FileFind>
{
public:
	QStringList files;
};
DPointered(FileFind)

FileFind::FileFind(const QStringList &fileTreeListing)
{
	d->files = fileTreeListing;
}

QList<int> FileFind::findExactPath(const QString &path)
{
	QList<int> indices;
	for (int index = 0; index < d->files.size(); ++index)
	{
		if (path.compare(d->files[index], Qt::CaseInsensitive) == 0)
		{
			indices.append(index);
		}
	}
	return indices;
}

QList<int> FileFind::findFilenameAll(const QString &filename)
{
	QList<int> indices;
	indices << findExactPath(filename);
	for (int index = 0; index < d->files.size(); ++index)
	{
		if (indices.contains(index))
			continue;
		QFileInfo fullPathCandidate(d->files[index]);
		if (filename.compare(fullPathCandidate.fileName(), Qt::CaseInsensitive) == 0)
		{
			indices << index;
		}
	}
	return indices;
}

int FileFind::findFilename(const QString &filename)
{
	QList<int> indices = findFilenameAll(filename);
	if (!indices.isEmpty())
		return indices.first();
	else
		return -1;
}

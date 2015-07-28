//------------------------------------------------------------------------------
// fileutils.cpp
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "fileutils.h"

#include <QCryptographicHash>
#include <QDirIterator>
#include <QFileInfo>
#include "log.h"

QByteArray FileUtils::md5(const QString &path)
{
	QFile f(path);
	if (f.open(QIODevice::ReadOnly))
	{
		QCryptographicHash hash(QCryptographicHash::Md5);
		QByteArray chunk = f.read(1024 * 1024);
		for (; !chunk.isEmpty(); chunk = f.read(1024 * 1024))
		{
			hash.addData(chunk);
		}
		f.close();
		return hash.result();
	}
	return QByteArray();
}

bool FileUtils::containsPath(const QStringList &candidates, const QString &path)
{
	foreach (const QString &candidate, candidates)
	{
		if (QFileInfo(candidate) == QFileInfo(path))
		{
			return true;
		}
	}
	return false;
}

bool FileUtils::rmAllFiles(const QString& dirPath,
	const QStringList & nameFilters)
{
	QDirIterator it(dirPath, nameFilters, QDir::Files);
	bool bAllSuccess = true;
	while (it.hasNext())
	{
		QString path = it.next();
		QFile f(path);
		if (!f.remove())
		{
			bAllSuccess = false;
			gLog << Log::tr("Failed to remove: %1").arg(path);
		}
	}
	return bAllSuccess;
}

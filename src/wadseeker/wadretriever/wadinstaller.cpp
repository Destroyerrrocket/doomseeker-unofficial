//------------------------------------------------------------------------------
// wadinstaller.cpp
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
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "wadinstaller.h"

#include "entities/waddownloadinfo.h"
#include "zip/unarchive.h"

#include <QDebug>
#include <QDir>
#include <QFile>

WadInstaller::WadInstaller(const QString& installPath)
{
	d.installPath = installPath;
}

WadInstaller::WadInstallerResult WadInstaller::installArchive(UnArchive& archive, const QList< WadDownloadInfo* >& requestedWads)
{
	QDir installDir(d.installPath);
	WadInstallerResult dirResult = makeSureDirPathExists(installDir);
	if (dirResult.isError())
	{
		return dirResult;
	}

	WadInstallerResult result;

	// We will try to find all requested WADs in the single archive.
	foreach (const WadDownloadInfo* pWadInfo, requestedWads)
	{
		const QString& name = pWadInfo->name();

		int entryIndex = archive.findFileEntry(name);
		if (entryIndex >= 0)
		{
			// File was found in the archive. Attempt extraction.
			QString filePath = installDir.absoluteFilePath(name);
			if (archive.extract(entryIndex, filePath))
			{
				result.installedWads << name;
			}
		}
	}

	return result;
}

WadInstaller::WadInstallerResult WadInstaller::installFile(const QString& fileName, const QByteArray& fileData)
{
	// Check for problems.
	if (fileData.isEmpty())
	{
		return WadInstallerResult::makeError(tr("Attempt to save an empty file."));
	}

	QDir installDir(d.installPath);
	WadInstallerResult dirResult = makeSureDirPathExists(installDir);
	if (dirResult.isError())
	{
		return dirResult;
	}

	// Save the file from memory buffer to the media storage device.
	QString filePath = installDir.absoluteFilePath(fileName);
	QFile file(filePath);

	if (!file.open(QFile::WriteOnly))
	{
		return WadInstallerResult::makeCriticalError(
				tr("Failed to open file \"%1\" for write.")
					.arg(filePath) );
	}

	file.write(fileData);
	file.close();

	return WadInstallerResult::makeSuccess(fileName);
}

WadInstaller::WadInstallerResult WadInstaller::makeSureDirPathExists(QDir& dir)
{
	if (!dir.exists())
	{
		printf("Dir %s does not exist!\n", dir.canonicalPath().toAscii().constData());
		if (!dir.mkpath("."))
		{
			return WadInstallerResult::makeCriticalError(
				tr("Directory \"%1\", where files are supposed to be saved, doesn't exist and cannot be created.")
					.arg(dir.absolutePath()) );
		}
	}

	return WadInstallerResult();
}


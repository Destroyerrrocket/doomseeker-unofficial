//------------------------------------------------------------------------------
// waddownloadinfo.cpp
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
#include "waddownloadinfo.h"
#include "wadseekerversioninfo.h"

#include <QFileInfo>
#include <QStringList>

WadDownloadInfo::WadDownloadInfo()
{
	d.size = -1;
}

WadDownloadInfo::WadDownloadInfo(const QString& name)
{
	d.name = name;
	d.size = -1;
}

QString WadDownloadInfo::archiveName(const QString& suffix) const
{
	if (isArchive())
	{
		return name();
	}
	else
	{
		QFileInfo fi(d.name);

		QString baseName = fi.completeBaseName();
		return baseName + "." + suffix;
	}
}

QString WadDownloadInfo::basename() const
{
	QFileInfo fi(d.name);
	return fi.completeBaseName();
}

bool WadDownloadInfo::isArchive() const
{
	QStringList supportedArchives = WadseekerVersionInfo::supportedArchiveExtensions();
	QFileInfo fi(d.name);

	foreach (const QString& supportedSuffix, supportedArchives)
	{
		if (fi.suffix().compare(supportedSuffix, Qt::CaseInsensitive) == 0)
		{
			return true;
		}
	}

	return false;
}

bool WadDownloadInfo::isValid() const
{
	return !d.name.trimmed().isEmpty();
}

bool WadDownloadInfo::isFilenameIndicatingSameWad(const QString& filename) const
{
	QFileInfo fi(filename);
	return fi.completeBaseName().compare(basename(), Qt::CaseInsensitive) == 0;
}

bool WadDownloadInfo::operator==(const WadDownloadInfo& other) const
{
	const QString& name = this->name();
	const QString& otherName = other.name();

	return name.compare(otherName, Qt::CaseInsensitive) == 0;
}

bool WadDownloadInfo::operator!=(const WadDownloadInfo& other) const
{
	return !(*this == other);
}

QStringList WadDownloadInfo::possibleArchiveNames() const
{
	QStringList names;

	if (isArchive())
	{
		names << name();
	}
	else
	{
		QString basename = this->basename();

		foreach (const QString& suffix, WadseekerVersionInfo::supportedArchiveExtensions())
		{
			names << basename + "." + suffix;
		}
	}

	return names;
}

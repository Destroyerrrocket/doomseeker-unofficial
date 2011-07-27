//------------------------------------------------------------------------------
// waddownloadinfo.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "waddownloadinfo.h"
#include "wadseekerversioninfo.h"

#include <QFileInfo>
#include <QStringList>

WadDownloadInfo::WadDownloadInfo(const QString& name)
{
	d.name = name;
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

bool WadDownloadInfo::isFilenameIndicatingSameWad(const QString& filename) const
{
	QFileInfo fi(filename);
	return fi.completeBaseName().compare(basename(), Qt::CaseInsensitive) == 0;
}

QStringList WadDownloadInfo::possibleArchiveNames()
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

QStringList WadDownloadInfo::possibleWadNames()
{
	QStringList names;

	QFileInfo fi(name());
	if (!fi.suffix().isEmpty())
	{
		names << name();
	}
	else
	{
		QString basename = this->basename();

		foreach (const QString& suffix, WadseekerVersionInfo::knownWadExtensions())
		{
			names << basename + "." + suffix;
		}
	}

	return names;
}


//------------------------------------------------------------------------------
// waddownloadinfo.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "waddownloadinfo.h"

#include <QFileInfo>

WadDownloadInfo::WadDownloadInfo(const QString& name)
{
	d.name = name;
}

bool WadDownloadInfo::isArchive() const
{
	QFileInfo fi(d.name);

	return fi.suffix().compare("zip", Qt::CaseInsensitive) == 0
		|| fi.suffix().compare("7z", Qt::CaseInsensitive) == 0;
}


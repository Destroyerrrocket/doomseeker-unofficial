//------------------------------------------------------------------------------
// fileseekinfo.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "fileseekinfo.h"

FileSeekInfo::FileSeekInfo()
{

}

FileSeekInfo::FileSeekInfo(const QString& file, const QStringList& possibleFilenames)
{
	d.file = file;
	foreach (QString filename, possibleFilenames)
	{
		if (!filename.trimmed().isEmpty() && !isFilenameListed(filename))
		{
			d.possibleFilenames << filename.trimmed();
		}
	}
}

void FileSeekInfo::addPossibleFilename(const QString& filename)
{
	if (!isValid())
	{
		return;
	}

	if (!filename.trimmed().isEmpty() && !isFilenameListed(filename))
	{
		d.possibleFilenames << filename;
	}
}

bool FileSeekInfo::isFilenameListed(const QString& filename) const
{
	foreach (const QString& listFilename, d.possibleFilenames)
	{
		if (filename.compare(listFilename, Qt::CaseInsensitive) == 0)
		{
			return true;
		}
	}

	return false;
}

bool FileSeekInfo::operator==(const QString& file) const
{
	return this->file().compare(file, Qt::CaseInsensitive) == 0;
}

bool FileSeekInfo::operator==(const FileSeekInfo& other) const
{
	return *this == other.file();
}

bool FileSeekInfo::operator!=(const QString& file) const
{
	return !(*this == file);
}

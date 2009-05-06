//------------------------------------------------------------------------------
// unzip.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "unzip.h"

#include <QDebug>
#include <QFileInfo>


UnZip::UnZip(const QString& file)
{
	QFileInfo fi(file);
	if (fi.exists() && !fi.isDir())
	{
		zipFile.setFileName(file);
	}
}

QList<ZipLocalFileHeader> UnZip::allDataHeaders()
{
	qint64 pos = 0;
	QList<ZipLocalFileHeader> list;

	if (zipFile.fileName().isEmpty())
		return list;

	zipFile.open(QFile::ReadOnly);
	while(true)
	{
		ZipLocalFileHeader zip;

		if(!this->readHeader(zipFile, pos, zip))
		{
			if (zip.localFileHeaderSignature == LOCAL_FILE_HEADER_SIGNATURE)
			{
				emit error(tr("ZIP file might be corrupted!"));
			}
			break;
		}
		else
		{
			list << zip;
			pos += zip.fileEntrySize();
		}
	}
	zipFile.close();

	return list;
}

unsigned long UnZip::findFileEntry(const QString& entryName)
{
	if (zipFile.fileName().isEmpty())
		return UNSIGNED_MINUS_ONE;

	// Look for LOCAL_FILE_HEADER_SIGNATURE, if found skip bytes
	// till file name length and file name (string), if not, try to skip to next header.
	// Fail when
	zipFile.open(QFile::ReadOnly);

	qint64 pos = 0;
	bool bEnd = false;
	while(!bEnd)
	{
		ZipLocalFileHeader zip;

		if (!this->readHeader(zipFile, pos, zip))
		{
			zipFile.close();
			return UNSIGNED_MINUS_ONE;
		}


	}

	zipFile.close();
}

bool UnZip::readHeader(QFile& zipFile, qint64 pos, ZipLocalFileHeader& zip)
{
	zipFile.seek(pos);

	QByteArray array = zipFile.read(ZIP_LOCAL_FILE_HEADER_SIZE);

	if (!zip.fromByteArray(array.constData(), array.size()))
		return false;

	array = zipFile.read(zip.fileNameLength);
	if (array.size() < zip.fileNameLength)
		return false;

	if (zip.fileName != NULL)
		delete [] zip.fileName;

	zip.fileName = new char[zip.fileNameLength + 1];
	memcpy(zip.fileName, array.constData(), zip.fileNameLength);
	zip.fileName[zip.fileNameLength] = 0;

	array = zipFile.read(zip.extraFieldLength);
	if (array.size() < zip.extraFieldLength)
		return false;

	if (zip.extraField != NULL)
	{
		delete [] zip.fileName;
		zip.extraField = NULL;
	}

	if (array.size() != 0)
	{
		zip.extraField = new char[zip.extraFieldLength];
		memcpy(zip.extraField, array.constData(), zip.extraFieldLength);
	}

	return true;
}

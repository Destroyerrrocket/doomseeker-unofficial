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
#include <zlib.h>

#include <QDebug>
#include <QFileInfo>


UnZip::UnZip(const QString& file)
{
	zipFile.setFileName(file);
}

QList<ZipLocalFileHeader> UnZip::allDataHeaders()
{
	qint64 pos = 0;
	QList<ZipLocalFileHeader> list;

	if (!isValid())
	{
		emit error(tr("\"%1\" doesn't exist or is a directory.").arg(zipFile.fileName()));
		return list;
	}

	zipFile.open(QFile::ReadOnly);
	while(true)
	{
		ZipLocalFileHeader zip;

		int readError = this->readHeader(zipFile, pos, zip);

		if(readError == ZipLocalFileHeader::HE_CORRUPTED)
		{
			emit error(tr("ZIP file \"%1\" is corrupted!").arg(zipFile.fileName()));
			break;
		}
		else if (readError == ZipLocalFileHeader::HE_NO_ERROR)
		{
			list << zip;
			pos += zip.fileEntrySize();
		}
		else
		{
			break;
		}
	}
	zipFile.close();

	return list;
}

bool UnZip::extract(const ZipLocalFileHeader& header, const QString& where)
{
	if (!isValid())
		return false;

	zipFile.open(QFile::ReadOnly);

	qint64 pos = header.headerPosition + header.howManyBytesTillData();
	zipFile.seek(pos);

	QByteArray array = zipFile.read(header.compressedSize);
	if (array.isNull())
	{
		zipFile.close();
		return false;
	}
	zipFile.close();

	char* uncompressed = new char[header.uncompressedSize];
	if (uncompressed == NULL)
	{
		return false;
	}

	int err = uncompress(uncompressed, header.uncompressedSize, array);

	bool bRet = false;
	QString entry = zipFile.fileName() + "#" + header.fileName;
	QFile outputFile(where);
	switch (err)
	{
		case Z_OK:
			outputFile.open(QFile::WriteOnly);
			outputFile.write(uncompressed, header.uncompressedSize);
			outputFile.close();
			emit notice(tr("%1 uncompressed successfuly.").arg(entry));
			bRet = true;
			break;

		case Z_DATA_ERROR:
			emit error(tr("%1 is corrupted.").arg(entry));
			break;
	}

	delete [] uncompressed;
	return bRet;
}

ZipLocalFileHeader* UnZip::findFileEntry(const QString& entryName)
{
	if (zipFile.fileName().isEmpty())
		return NULL;

	zipFile.open(QFile::ReadOnly);

	qint64 pos = 0;
	ZipLocalFileHeader* ret = NULL;

	while(true)
	{
		ZipLocalFileHeader* zip = new ZipLocalFileHeader();

		if (this->readHeader(zipFile, pos, *zip) != ZipLocalFileHeader::HE_NO_ERROR)
		{
			delete zip;
			break;
		}

		QFileInfo fi(zip->fileName);
		QString strFile = fi.fileName();
		// If file was found return it's header.
		if (strFile.compare(entryName, Qt::CaseInsensitive) == 0)
		{
			ret = zip;
			break;
		}

		pos += zip->fileEntrySize();
		delete zip;
	}

	zipFile.close();
	return ret;
}

int UnZip::readHeader(QFile& zipFile, qint64 pos, ZipLocalFileHeader& zip)
{
	int readErr;
	zipFile.seek(pos);

	QByteArray array = zipFile.read(ZIP_LOCAL_FILE_HEADER_SIZE);

	readErr = zip.fromByteArray(array);
	if (readErr != ZipLocalFileHeader::HE_NO_ERROR)
	{
		if (pos == 0 && readErr == ZipLocalFileHeader::HE_NOT_LOCAL_FILE_HEADER)
		{
			emit error(tr("\"%1\" is not a ZIP file.").arg(zipFile.fileName()));
		}
		return readErr;
	}

	array = zipFile.read(zip.fileNameLength);
	if (array.size() < zip.fileNameLength)
		return ZipLocalFileHeader::HE_CORRUPTED;

	zip.fileName = array.constData();

	array = zipFile.read(zip.extraFieldLength);
	if (array.size() < zip.extraFieldLength)
		return ZipLocalFileHeader::HE_CORRUPTED;

	zip.extraField = array;

	zip.headerPosition = pos;

	return ZipLocalFileHeader::HE_NO_ERROR;
}

int	UnZip::uncompress(char* out, unsigned long uncompressedSize, const QByteArray& inArray)
{
	// Code here was blatantly ripped from SDE.
	unsigned char* in = new unsigned char[inArray.size()];
	memcpy(in, inArray.constData(), inArray.size());

	z_stream zstream;
	zstream.next_in = in;
	zstream.avail_in = inArray.size();
	zstream.next_out = (unsigned char*)out;
	zstream.avail_out = uncompressedSize;
	zstream.zalloc = Z_NULL;
	zstream.zfree = Z_NULL;
	unsigned int err = inflateInit2(&zstream, -15);
	while(err == Z_OK && zstream.avail_out != 0)
	{
		err = inflate(&zstream, Z_SYNC_FLUSH);
	}
	if(err != Z_STREAM_END)
	{
		delete[] in;
		return Z_DATA_ERROR;
	}
	err = inflateEnd(&zstream);

	delete[] in;
	return err;
}

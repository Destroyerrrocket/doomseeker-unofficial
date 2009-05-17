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

UnZip::UnZip(const QByteArray& data)
{
	dataType = ByteArray;
	zipData = data;
}

UnZip::UnZip(const QString& file)
{
	dataType = File;
	zipFile.setFileName(file);
}

QList<ZipLocalFileHeader> UnZip::allDataHeaders()
{
	qint64 pos = 0;
	QList<ZipLocalFileHeader> list;

	if (!isValid())
	{
		if (dataType == File)
			emit message(tr("\"%1\" doesn't exist or is a directory.").arg(zipFile.fileName()), Wadseeker::Error);
		else
			emit message(tr("No data is present."), Wadseeker::Error);
		return list;
	}

	if (dataType == File)
	{
		zipFile.open(QFile::ReadOnly);
	}
	while(true)
	{
		ZipLocalFileHeader zip;

		int readError;
		if (dataType == File)
		{
			readError = this->readHeader(zipFile, pos, zip);
		}
		else
		{
			readError = this->readHeader(zipData, pos, zip);
		}

		if(readError == ZipLocalFileHeader::Corrupted)
		{
			emit message(tr("ZIP file \"%1\" is corrupted!").arg(zipFile.fileName()), Wadseeker::Error);
			break;
		}
		else if (readError == ZipLocalFileHeader::NoError)
		{
			list << zip;
			pos += zip.fileEntrySize();
		}
		else
		{
			break;
		}
	}
	if (dataType == File)
	{
		zipFile.close();
	}

	return list;
}

bool UnZip::extract(const ZipLocalFileHeader& header, const QString& where)
{
	if (!isValid())
		return false;

	QByteArray array;
	if (dataType == File)
	{
		zipFile.open(QFile::ReadOnly);

		qint64 pos = header.headerPosition + header.howManyBytesTillData();
		zipFile.seek(pos);

		array = zipFile.read(header.compressedSize);
		if (array.isNull())
		{
			zipFile.close();
			return false;
		}
		zipFile.close();
	}
	else
	{
		array = zipData.mid(header.headerPosition + header.howManyBytesTillData(), header.compressedSize);
	}

	char* uncompressed = new char[header.uncompressedSize];
	if (uncompressed == NULL)
	{
		return false;
	}

	int err = uncompress(uncompressed, header.uncompressedSize, array);

	bool bRet = false;
	QFile outputFile(where);
	switch (err)
	{
		case Z_OK:
			outputFile.open(QFile::WriteOnly);
			outputFile.write(uncompressed, header.uncompressedSize);
			outputFile.close();
			bRet = true;
			break;

		case Z_DATA_ERROR:
			break;
	}

	delete [] uncompressed;
	return bRet;
}

ZipLocalFileHeader* UnZip::findFileEntry(const QString& entryName)
{
	if (!isValid())
		return NULL;

	qint64 pos = 0;
	ZipLocalFileHeader* ret = NULL;

	if (dataType == File)
	{
		zipFile.open(QFile::ReadOnly);
	}
	while(true)
	{
		ZipLocalFileHeader* zip = new ZipLocalFileHeader();

		int err;
		if (dataType == File)
		{
			err = this->readHeader(zipFile, pos, *zip);
		}
		else
		{
			err = this->readHeader(zipData, pos, *zip);
		}
		if (err != ZipLocalFileHeader::NoError)
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
	if (dataType == File)
	{
		zipFile.close();
	}
	return ret;
}

bool UnZip::isZip()
{
	ZipLocalFileHeader zip;
	int err;
	if (dataType == File)
	{
		zipFile.open(QIODevice::ReadOnly);
		err = readHeader(zipFile, 0, zip);
		zipFile.close();
	}
	else
	{
		err = readHeader(zipData, 0, zip);
	}

	if (err != ZipLocalFileHeader::NoError)
	{
		return false;
	}

	return true;
}

int UnZip::readHeader(QByteArray& zipData, qint64 pos, ZipLocalFileHeader& zip)
{
	int readErr;

	QByteArray array = zipData.mid(pos, ZIP_LOCAL_FILE_HEADER_SIZE);

	zip.headerPosition = pos;
	readErr = zip.fromByteArray(array);
	if (readErr != ZipLocalFileHeader::NoError)
	{
		return readErr;
	}

	pos += ZIP_LOCAL_FILE_HEADER_SIZE;
	array = zipData.mid(pos, zip.fileNameLength);
	if (array.size() < zip.fileNameLength)
		return ZipLocalFileHeader::Corrupted;

	zip.fileName = array;

	pos += zip.fileNameLength;
	array = zipData.mid(pos, zip.extraFieldLength);
	if (array.size() < zip.extraFieldLength)
		return ZipLocalFileHeader::Corrupted;

	zip.extraField = array;

	return ZipLocalFileHeader::NoError;
}

int UnZip::readHeader(QFile& zipFile, qint64 pos, ZipLocalFileHeader& zip)
{
	int readErr;
	zipFile.seek(pos);

	QByteArray array = zipFile.read(ZIP_LOCAL_FILE_HEADER_SIZE);

	readErr = zip.fromByteArray(array);
	if (readErr != ZipLocalFileHeader::NoError)
	{
		return readErr;
	}

	array = zipFile.read(zip.fileNameLength);
	if (array.size() < zip.fileNameLength)
		return ZipLocalFileHeader::Corrupted;

	zip.fileName = array.constData();

	array = zipFile.read(zip.extraFieldLength);
	if (array.size() < zip.extraFieldLength)
		return ZipLocalFileHeader::Corrupted;

	zip.extraField = array;

	zip.headerPosition = pos;

	return ZipLocalFileHeader::NoError;
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

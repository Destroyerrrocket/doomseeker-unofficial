//------------------------------------------------------------------------------
// unzip.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "unzip.h"
#include <zlib.h>

#include <QBuffer>
#include <QDebug>
#include <QFileInfo>

UnZip::UnZip(QIODevice *device) 
: UnArchive(device)
{
}

UnZip::~UnZip()
{
}

QList<ZipLocalFileHeader> UnZip::allDataHeaders()
{
	qint64 pos = 0;
	QList<ZipLocalFileHeader> list;

	if (!isValid())
	{
		emit message(tr("No valid zip data is present."), WadseekerLib::Error);
		return list;
	}

	stream->open(QFile::ReadOnly);
	while(true)
	{
		ZipLocalFileHeader zip;

		int readError = this->readHeader(pos, zip);

		if(readError == ZipLocalFileHeader::Corrupted)
		{
			emit message(tr("ZIP file is corrupted!"), WadseekerLib::Error);
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
	stream->close();

	return list;
}

bool UnZip::extract(int file, const QString& where)
{
	if (!isValid())
		return false;

	QByteArray array;
	ZipLocalFileHeader header;
	stream->open(QFile::ReadOnly);
	readHeader(file, header);
	qint64 pos = header.headerPosition + header.howManyBytesTillData();
	stream->seek(pos);

	array = stream->read(header.compressedSize);
	if (array.isNull())
	{
		stream->close();
		return false;
	}
	stream->close();

	QFile outputFile(where);
	if (header.compressionMethod != 0)
	{
		char* uncompressed = new char[header.uncompressedSize];
		if (uncompressed == NULL)
		{
			return false;
		}


		int err = uncompress(uncompressed, header.uncompressedSize, array);

		bool bRet = false;
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
	else
	{
		// No compression, simply copy.
		outputFile.open(QFile::WriteOnly);
		outputFile.write(array, header.uncompressedSize);
		outputFile.close();
		return true;
	}
}



int UnZip::findFileEntry(const QString& entryName)
{
	if (!isValid())
		return -1;

	qint64 pos = 0;
	ZipLocalFileHeader* ret = NULL;

	stream->open(QFile::ReadOnly);
	while(true)
	{
		ZipLocalFileHeader* zip = new ZipLocalFileHeader();

		int err;
		err = this->readHeader(pos, *zip);
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
	stream->close();

	if(ret == NULL)
		return -1;
	delete ret;
	return pos;
}

QString UnZip::fileNameFromIndex(int file)
{
	ZipLocalFileHeader header;

	stream->open(QFile::ReadOnly);
	readHeader(file, header);
	stream->close();

	return header.fileName;
}

bool UnZip::isZip()
{
	ZipLocalFileHeader zip;
	int err;
	stream->open(QIODevice::ReadOnly);
	err = readHeader(0, zip);
	stream->close();

	if (err != ZipLocalFileHeader::NoError)
	{
		return false;
	}

	return true;
}

int UnZip::readHeader(qint64 pos, ZipLocalFileHeader& zip)
{
	// This expects the file to be already open and doesn't close it.
	int readErr;
	stream->seek(pos);

	QByteArray array = stream->read(ZIP_LOCAL_FILE_HEADER_SIZE);

	readErr = zip.fromByteArray(array);
	if (readErr != ZipLocalFileHeader::NoError)
	{
		return readErr;
	}

	array = stream->read(zip.fileNameLength);
	if (array.size() < zip.fileNameLength)
		return ZipLocalFileHeader::Corrupted;

	zip.fileName = array.constData();

	array = stream->read(zip.extraFieldLength);
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

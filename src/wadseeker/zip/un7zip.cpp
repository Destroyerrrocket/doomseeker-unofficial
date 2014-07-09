//------------------------------------------------------------------------------
// un7zip.cpp
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include <QFile>

#include "un7zip.h"
#include <cstdlib>
#include <cstdio>

#include <QBuffer>
#include <QFile>

SZByteStream::SZByteStream(QIODevice *buffer) : buffer(buffer)
{
	buffer->open(QIODevice::ReadOnly);

	stream.Read = Read;
	stream.Seek = Seek;
}

SZByteStream::~SZByteStream()
{
	buffer->close();
}

SRes SZByteStream::Read(void *p, void *buf, size_t *size)
{
	SZByteStream *s = reinterpret_cast<SZByteStream *> (p);
	qint64 numRead = s->buffer->read(reinterpret_cast<char *> (buf), *size);
	if(numRead < 0)
	{
		*size = 0;
		return SZ_ERROR_READ;
	}
	*size = numRead;
	return SZ_OK;
}

SRes SZByteStream::Seek(void *p, Int64 *pos, ESzSeek origin)
{
	SZByteStream *s = reinterpret_cast<SZByteStream *> (p);
	bool ret = false;
	switch(origin)
	{
		default:
			ret = s->buffer->seek(*pos);
			break;
		case SZ_SEEK_CUR:
			ret = s->buffer->seek(s->buffer->pos() + *pos);
			break;
		case SZ_SEEK_END:
			ret = s->buffer->seek(s->buffer->size() + *pos);
			break;
	}
	*pos = s->buffer->pos();
	return ret ? SZ_OK : SZ_ERROR_DATA;
}

////////////////////////////////////////////////////////////////////////////////

void *Un7Zip::SzAlloc(void *p, size_t size) { return malloc(size); }
void Un7Zip::SzFree(void *p, void *address) { free(address); }
ISzAlloc Un7Zip::alloc = { SzAlloc, SzFree };

Un7Zip::Un7Zip(QIODevice *device)
: UnArchive(device), out(NULL), outSize(0), valid(true)
{
	Init();
}

Un7Zip::~Un7Zip()
{
	if(out != NULL)
		IAlloc_Free(&alloc, out);
	SzArEx_Free(&db, &alloc);

	delete byteStream;
}

bool Un7Zip::extract(int file, const QString &where)
{
	UInt32 blockIndex = 0xFFFFFFFF; // Not sure what this is, but this is what ZDoom uses.
	size_t offset, outSizeProcessed;

	SRes res = SzAr_Extract(&db, &lookStream.s, file, &blockIndex, &out, &outSize, &offset, &outSizeProcessed, &alloc, &alloc);
	if(res == SZ_OK)
	{
		QFile outputFile(where);
		outputFile.open(QFile::WriteOnly);
		outputFile.write(reinterpret_cast<char*> (out + offset), outSizeProcessed);
		outputFile.close();
		return true;
	}
	return false;
}

QString Un7Zip::fileNameFromIndex(int file)
{
	if(file < 0 || file >= static_cast<int> (db.db.NumFiles))
		return QString();
	return QString(db.db.Files[file].Name);
}

int Un7Zip::findFileEntry(const QString &entryName)
{
	for(unsigned int i = 0;i < db.db.NumFiles;i++)
	{
		const CSzFileItem &file = db.db.Files[i];
		if(entryName.compare(file.Name, Qt::CaseInsensitive) == 0)
			return i;
	}
	return -1;
}

void Un7Zip::Init()
{
	if (g_CrcTable[1] == 0)
		CrcGenerateTable();

	byteStream = new SZByteStream(stream);
	LookToRead_CreateVTable(&lookStream, false);
	lookStream.realStream = &byteStream->stream;
	LookToRead_Init(&lookStream);

	SzArEx_Init(&db);
	SRes tmp;
	if((tmp = SzArEx_Open(&db, &lookStream.s, &alloc, &alloc)) != SZ_OK)
		valid = false;
}

//------------------------------------------------------------------------------
// un7zip.h
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

#ifndef __UN7ZIP_H__
#define __UN7ZIP_H__

#include <QBuffer>
#include <QByteArray>
#include <QObject>

extern "C"
{
#include "Archive/7z/7zHeader.h"
#include "Archive/7z/7zExtract.h"
#include "Archive/7z/7zIn.h"
#include "7zCrc.h"
}

struct SZByteStream
{
	// [BL] Ugh, I really don't see a way around copying this ZDoom hack.
	//      This must be the first variable.
	ISeekInStream	stream;

	QBuffer			buffer;

	SZByteStream(QByteArray *array);
	~SZByteStream();

	static SRes Read(void *p, void *buf, size_t *size);
	static SRes Seek(void *p, Int64 *pos, ESzSeek origin);
};

class Un7Zip : public QObject
{
	Q_OBJECT

	public:
		Un7Zip(const QByteArray &data);
		~Un7Zip();

		bool	extract(int file, const QString &where);
		QString	fileNameFromIndex(int file) const;
		int		findFileEntry(const QString &entryName) const;
		bool	isValid() const { return valid; }

	protected:
		static void	*SzAlloc(void *p, size_t size);
		static void	SzFree(void *p, void *address);

		SZByteStream	*byteStream;
		CLookToRead		lookStream;

		CSzArEx			db;
		static ISzAlloc	alloc;
		Byte*			out;
		size_t			outSize;

		QByteArray		data;
		bool			valid;
};

#endif

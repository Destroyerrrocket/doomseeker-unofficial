//------------------------------------------------------------------------------
// unzip.h
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
#ifndef __UNZIP_H_
#define __UNZIP_H_

#include "../wadseeker.h"
#include "localfileheader.h"
#include "unarchive.h"
#include <QFile>
#include <QFileInfo>
#include <QObject>

class UnZip : public UnArchive
{
	Q_OBJECT

	public:
		UnZip(QIODevice *device);
		~UnZip();


		/**
		 * @return all data headers found in the zip file
		 */
		QList<ZipLocalFileHeader> 	allDataHeaders();

		bool						extract(int file, const QString& where);

		/**
		 * @return the position of the header in the archive.
		 */
		int							findFileEntry(const QString& entryName);

		QString						fileNameFromIndex(int file);

		/**
		 * @return true if the zipFile is a valid file or zipData is not empty.
		 *         and is a valid zip file.
		 */
		bool	isValid()
		{
			return isZip();
		}

		/**
		 * @return true if data passed to constructor is a zip data.
		 */
		bool	isZip();

	private:
		/**
		 *	This method expects zipFile to be already open and it won't close it.
		 */
		int			readHeader(qint64 pos, ZipLocalFileHeader& zip);
		int         uncompress(QIODevice& streamIn, QIODevice& streamOut, unsigned long compressedSize);
};

#endif

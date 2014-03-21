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
		 * @brief Extracts all data headers found in the zip file
		 */
		QList<ZipLocalFileHeader> allDataHeaders();

		/**
		 * @brief Extracts file to specified path.
		 *
		 * @param file
		 *      File index. It can be extracted in following ways:
		 *      - By counting indices of list returned by allDataHeaders().
		 *        Each index corresponds directly to this param.
		 *      - Through findFileEntry()
		 * @param where
		 *      Path in the file system to which the file should be extracted.
		 */
		bool extract(int file, const QString& where);

		/**
		 * @brief File index for given entry name.
		 *
		 * This is an opposite of fileNameFromIndex()
		 *
		 * @return Value that can be passed to extract() or negative value
		 *         if entry not found or error occurred.
		 */
		int findFileEntry(const QString& entryName);

		/**
		 * @brief File name fron given index.
		 *
		 * This is an opposite of fileNameFromIndex()
		 *
		 * @see allDataHeaders()
		 * @see extract()
		 */
		QString fileNameFromIndex(int file);

		/**
		 * @brief true if the QIODevice specified in the constructor
		 *        is a valid ZIP file.
		 */
		bool isValid()
		{
			return isZip();
		}

		/**
		 * @brief true if the QIODevice specified in the constructor
		 *        is a valid ZIP file.
		 */
		bool isZip();

	private:
		/**
		 * @brief This method expects the iodevice to be already open 
		 *        and it won't close it.
		 *
		 * @return ZipLocalFileHeader::HeaderError value
		 */
		int readHeader(qint64 pos, ZipLocalFileHeader& zip);
		
		/**
		 * @brief This method expects the iodevice to be already open 
		 *        and it won't close it.
		 *
		 * @return ZipLocalFileHeader::HeaderError value
		 */
		int readHeaderFromFileIndex(int file, ZipLocalFileHeader& zip);
		
		int uncompress(QIODevice& streamIn, QIODevice& streamOut, unsigned long compressedSize);
};

#endif

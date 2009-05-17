//------------------------------------------------------------------------------
// unzip.h
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
#ifndef __UNZIP_H_
#define __UNZIP_H_

#include "../wadseekerglobals.h"
#include "localfileheader.h"
#include <QFile>
#include <QFileInfo>
#include <QObject>

class UnZip : public QObject
{
	Q_OBJECT

	public:
		enum DataType
		{
			File		= 0,
			ByteArray	= 1
		};

		UnZip(const QByteArray& data);
		UnZip(const QString& file);


		/**
		 * @return all data headers found in the zip file
		 */
		QList<ZipLocalFileHeader> 	allDataHeaders();

		bool						extract(const ZipLocalFileHeader& header, const QString& where);

		/**
		 * @return position of file entry in the zip file,
		 *	beginning from local file header signature (see: localfileheader.h)
		 */
		ZipLocalFileHeader*			findFileEntry(const QString& entryName);

		/**
		 * @return true if the zipFile is a valid file or zipData is not empty.
		 */
		bool	isValid()
		{
			if (dataType == File)
			{
				QFileInfo fi(zipFile);
				return (fi.exists() && !fi.isDir());
			}
			else
			{
				return !zipData.isEmpty();
			}
		}

		/**
		 * @return true if data passed to constructor is a zip data.
		 */
		bool	isZip();

	signals:
		void message(const QString&, WadseekerMessageType type);

	protected:
		DataType	dataType;
		QFile		zipFile;
		QByteArray	zipData;

		int			readHeader(QByteArray& zipData, qint64 pos, ZipLocalFileHeader& zip);
		int			readHeader(QFile& zipFile, qint64 pos, ZipLocalFileHeader& zip);
		int			uncompress(char* out, unsigned long uncompressedSize, const QByteArray& inArray);
};

#endif

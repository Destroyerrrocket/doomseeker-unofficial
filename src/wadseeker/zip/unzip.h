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

#include "localfileheader.h"
#include <QFile>
#include <QObject>

#define UNSIGNED_MINUS_ONE static_cast<unsigned long>(-1)


class UnZip : public QObject
{
	Q_OBJECT

	public:
		UnZip(const QString& file);

		/**
		 * @return all data headers found in the zip file
		 */
		QList<ZipLocalFileHeader> 	allDataHeaders();
		/**
		 * @return position of file entry in the zip file,
		 *	beginning from local file header signature (see: localfileheader.h)
		 *	if entry is not found the value is UNSIGNED_MINUS_ONE
		 */
		unsigned long				findFileEntry(const QString& entryName);

	signals:
		void error(const QString&);

	protected:
		QFile		zipFile;

		bool		readHeader(QFile& zipFile, qint64 pos, ZipLocalFileHeader& zip);
};

#endif

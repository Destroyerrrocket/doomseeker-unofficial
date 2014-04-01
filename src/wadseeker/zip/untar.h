//------------------------------------------------------------------------------
// untar.h
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
// Copyright (C) 2011 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __UNTAR_H__
#define __UNTAR_H__

#include <QByteArray>
#include <QIODevice>
#include <QList>
#include <QObject>

#include "unarchive.h"

class UnTar : public UnArchive
{
	Q_OBJECT

	public:
		UnTar(QIODevice *device);
		~UnTar();

		bool extract(int file, const QString &where);
		QString fileNameFromIndex(int file);
		int findFileEntry(const QString &entryName);
		bool isValid() { return valid; }

	protected:
		class TarFile
		{
			public:
				QString filename;
				unsigned int size;
				unsigned int offset;
		};

		void scanTarFile();

		bool valid;
		QList<TarFile> directory;
};

#endif

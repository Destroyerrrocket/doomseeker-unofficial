//------------------------------------------------------------------------------
// unarchive.cpp
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

#include "unarchive.h"
#include "un7zip.h"
#include "unzip.h"

UnArchive *UnArchive::OpenArchive(const QFileInfo &fi, const QByteArray &data)
{
	if(fi.suffix().compare("zip", Qt::CaseInsensitive) == 0)
		return new UnZip(data);
	else if(fi.suffix().compare("7z", Qt::CaseInsensitive) == 0)
		return new Un7Zip(data);
	return NULL;
}

UnArchive *UnArchive::OpenArchive(const QString &filename)
{
	QFileInfo fi(filename);
	if(!fi.isReadable())
		return NULL;

	if(fi.suffix().compare("zip", Qt::CaseInsensitive) == 0)
		return new UnZip(filename);
	else if(fi.suffix().compare("7z", Qt::CaseInsensitive) == 0)
		return new Un7Zip(filename);

	return NULL;
}

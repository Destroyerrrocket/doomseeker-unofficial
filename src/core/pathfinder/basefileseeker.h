//------------------------------------------------------------------------------
// basefileseeker.h
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id05405E76_A2CE_42DA_B784CB577350685F
#define id05405E76_A2CE_42DA_B784CB577350685F

#include <QList>
#include <QString>

class FileSearchPath;

class BaseFileSeeker
{
	public:
		virtual ~BaseFileSeeker() {}

		QString findFile(const QString& fileName, const QList<FileSearchPath>& paths);
		QString findFileInPath(const QString& fileName, const FileSearchPath& path);
		virtual QString findFileInSpecifiedDirectory(const QString& fileName, const QString& dirPath) = 0;
};

#endif // header

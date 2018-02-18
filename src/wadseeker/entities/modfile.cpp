//------------------------------------------------------------------------------
// modfile.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "modfile.h"

DClass<ModFile>
{
public:
	QString fileName;
	QString name;
	QString description;
	QString md5;
	QUrl url;
	QString version;
};
DPointered(ModFile)

ModFile::ModFile()
{
}

ModFile::~ModFile()
{
}

const QString &ModFile::fileName() const
{
	return d->fileName;
}

void ModFile::setFileName(const QString &v) const
{
	d->fileName = v;
}

const QString &ModFile::name() const
{
	return d->name;
}

void ModFile::setName(const QString &v) const
{
	d->name = v;
}

const QString &ModFile::description() const
{
	return d->description;
}

void ModFile::setDescription(const QString &v) const
{
	d->description = v;
}

const QString &ModFile::md5() const
{
	return d->md5;
}

void ModFile::setMd5(const QString &v) const
{
	d->md5 = v;
}

const QUrl &ModFile::url() const
{
	return d->url;
}

void ModFile::setUrl(const QUrl &v) const
{
	d->url = v;
}

const QString &ModFile::version() const
{
	return d->version;
}

void ModFile::setVersion(const QString &v) const
{
	d->version = v;
}

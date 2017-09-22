//------------------------------------------------------------------------------
// modset.cpp
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
#include "modset.h"

#include <QMap>

DClass<ModSet>
{
public:
	QMap<QString, ModFile> files;
};
DPointered(ModSet)

ModSet::ModSet()
{
}

ModSet::~ModSet()
{
}

void ModSet::addModFile(const ModFile &file)
{
	if (!file.fileName().isEmpty())
	{
		d->files.insert(file.fileName(), file);
	}
}

void ModSet::clear()
{
	d->files.clear();
}

ModFile ModSet::findFileName(const QString &fileName) const
{
	return d->files[fileName];
}

ModFile ModSet::first() const
{
	if (isEmpty())
	{
		return ModFile();
	}
	return modFiles().first();
}

bool ModSet::isEmpty() const
{
	return d->files.isEmpty();
}

QList<ModFile> ModSet::modFiles() const
{
	return d->files.values();
}

void ModSet::removeModFile(const ModFile &file)
{
	d->files.remove(file.fileName());
}

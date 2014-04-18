//------------------------------------------------------------------------------
// filealias.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "filealias.h"

FileAlias::FileAlias()
{
}

FileAlias::FileAlias(const QString &name)
{
	d.name = name;
}

void FileAlias::addAlias(const QString &val)
{
	if (!d.aliases.contains(val, Qt::CaseInsensitive))
	{
		d.aliases << val;
	}
}

void FileAlias::addAliases(const QStringList &val)
{
	foreach (const QString &element, val)
	{
		addAlias(element);
	}
}

const QStringList &FileAlias::aliases() const
{
	return d.aliases;
}

void FileAlias::setAliases(const QStringList &val)
{
	d.aliases = val;
}

FileAlias FileAlias::deserializeQVariant(const QVariant &var)
{
	QVariantMap m = var.toMap();
	FileAlias result;
	result.setAliases(m["aliases"].toStringList());
	result.setName(m["name"].toString());
	return result;
}

QVariant FileAlias::serializeQVariant() const
{
	QVariantMap m;
	m["aliases"] = aliases();
	m["name"] = name();
	return m;
}

FileAlias FileAlias::freeDoom1Aliases()
{
	FileAlias result;
	result.setName("doom.wad");
	QStringList aliases;
	aliases << "freedomu.wad" << "freedoom1.wad";
	result.setAliases(aliases);
	return result;
}

QList<FileAlias> FileAlias::freeDoom2Aliases()
{
	QStringList bases;
	// Each of these are equivalent in terms of modern source-ports.
	// If custom levels require one of those WADs as a base, I'd assume
	// they will run just fine with any of them, and thus also with freedoom.
	bases << "doom2.wad" << "tnt.wad" << "plutonia.wad";
	QList<FileAlias> result;
	foreach (const QString &base, bases)
	{
		FileAlias alias;
		alias.setName(base);
		QStringList aliases;
		aliases << "freedoom.wad" << "freedoom2.wad";
		alias.setAliases(aliases);
		result << alias;
	}
	return result;
}

bool FileAlias::isSameName(const QString &otherName) const
{
	return d.name.compare(otherName, Qt::CaseInsensitive) == 0;
}

bool FileAlias::isValid() const
{
	return !name().isEmpty() && !aliases().isEmpty();
}

const QString &FileAlias::name() const
{
	return d.name;
}

void FileAlias::setName(const QString &val)
{
	d.name = val;
}

QList<FileAlias> FileAlias::standardWadAliases()
{
	QList<FileAlias> result;
	result << freeDoom1Aliases();
	foreach (const FileAlias &alias, freeDoom2Aliases())
	{
		result << alias;
	}
	return result;
}
///////////////////////////////////////////////////////////////////////////////
QList<FileAlias> FileAliasList::mergeDuplicates(const QList<FileAlias> &input)
{
	QList<FileAlias> result;
	foreach (const FileAlias &alias, input)
	{
		bool merged = false;
		for (int i = 0; i < result.size(); ++i)
		{
			FileAlias &aliasOnList = result[i];
			if (aliasOnList.isSameName(alias.name()))
			{
				aliasOnList.addAliases(alias.aliases());
				merged = true;
				break;
			}
		}
		if (!merged)
		{
			result << alias;
		}
	}
	return result;
}

//------------------------------------------------------------------------------
// ini.cpp
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
#include "ini.h"
#include "log.h"
#include "main.h"
#include "strings.h"

#include <cassert>

Ini::Ini(const QString& filename)
{
    pIni = new QSettings(filename, QSettings::IniFormat);
}

Ini::~Ini()
{
	delete pIni;
}

IniSection Ini::createSection(const QString& name)
{
	if (name.isEmpty())
	{
		return IniSection();
	}

    return IniSection(this, name);
}

IniVariable Ini::createSetting(const QString& sectionName, const QString& name, const QVariant& data)
{
	IniSection section = createSection(sectionName);
	if (section.isNull())
	{
		return IniVariable();
	}

	return section.createSetting(name, data);
}

void Ini::deleteSection(const QString& sectionName)
{
	removeKey(sectionName);
}

void Ini::deleteSetting(const QString& sectionName, const QString& settingName)
{
	removeKey(sectionName + "/" + settingName);
}

bool Ini::loadIniFile(const QString& filePath)
{
	gLog << tr("Ini file is: %1").arg(filePath);

	if (pIni != NULL)
	{
		delete pIni;
	}

	pIni = new QSettings(filePath, QSettings::IniFormat);
	return pIni != NULL;
}

void Ini::removeKey(const QString& key)
{
	pIni->remove(key);
}

IniSection Ini::retrieveSection(const QString& name)
{
	if (name.isEmpty())
	{
		return IniSection();
	}

	return IniSection(this, name);
}

IniVariable Ini::retrieveSetting(const QString& sectionName, const QString& variableName)
{
	IniSection section = this->section(sectionName);
	if (section.isNull())
	{
		return IniVariable();
	}

	return section.retrieveSetting(variableName);
}

bool Ini::save()
{
    if (!pIni->isWritable())
    {
        return false;
    }

	pIni->sync();
	return true;
}

IniSection Ini::section(const QString& name)
{
	return createSection(name);
}

QVector<IniSection> Ini::sectionsArray(const QString& regexPattern)
{
	QVector<IniSection> sectionsReferencesArray;

	QRegExp regExp(regexPattern, Qt::CaseInsensitive);

	QStringList groups = pIni->childGroups();

	foreach (const QString& key, groups)
	{
		if (key.contains(regExp))
		{
			sectionsReferencesArray << IniSection(this, key);
		}
	}

	return sectionsReferencesArray;
}

IniVariable Ini::setting(const QString& sectionName, const QString& variableName)
{
	if (sectionName.isEmpty() || variableName.isEmpty())
	{
		return IniVariable();
	}

	IniVariable var = retrieveSetting(sectionName, variableName);
	if (var.isNull())
	{
		return createSetting(sectionName, variableName, QVariant());
	}

	return var;
}

void Ini::setValue(const QString& key, const QVariant& value)
{
	assert(pIni != NULL);

	pIni->setValue(key, value);
}

QVariant Ini::value(const QString& key) const
{
	assert(pIni != NULL);

	return pIni->value(key);
}

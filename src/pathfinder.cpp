//------------------------------------------------------------------------------
// pathfinder.cpp
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

#include "pathfinder.h"
#include <QDir>
#include <QFileInfo>

PathFinder::PathFinder(Config* cfg)
{
	config = cfg;
}

QString PathFinder::findWad(const QString& fileName)
{
	if (config == NULL)
		return QString();

	SettingsData* setting;
	setting = config->setting("WadPaths");
	QStringList strList = setting->string().split(";", QString::SkipEmptyParts);

	for (int i = 0; i < strList.count(); ++i)
	{
		QFileInfo file(strList[i] + QDir::separator() + fileName);
		if (file.exists() && file.isFile())
			return file.absoluteFilePath();
	}

	return QString();
}

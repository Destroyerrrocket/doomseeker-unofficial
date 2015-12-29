//------------------------------------------------------------------------------
// gamefile.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "gamefile.h"

#include "fileutils.h"

DClass<GameFile>
{
public:
	QString configName;
	bool executable;
	QString fileName;
	QString niceName;
	QStringList searchSuffixes;
};

DPointered(GameFile)

GameFile::GameFile()
{
	d->executable = false;
}

GameFile::~GameFile()
{
}

GameFile GameFile::exe(const QString &configName, const QString &niceName,
	const QString &fileName)
{
	GameFile o;
	o.setConfigName(configName);
	o.setFileName(fileName);
	o.setNiceName(niceName);
	o.d->executable = true;
	return o;
}

const QString &GameFile::configName() const
{
	return d->configName;
}

void GameFile::setConfigName(const QString &name)
{
	d->configName = name;
}

const QString &GameFile::fileName() const
{
	return d->fileName;
}

void GameFile::setFileName(const QString &name)
{
	d->fileName = name;
}

bool GameFile::isExecutable() const
{
	return d->executable;
}

bool GameFile::isSameFile(const QString &otherFileName)
{
	QString thisFile = fileName();
#if defined(Q_OS_WIN32)
	if (isExecutable())
	{
		if (!thisFile.endsWith(".exe"))
		{
			thisFile += ".exe";
		}
	}
#endif
	return thisFile.compare(otherFileName, FileUtils::comparisonSensitivity()) == 0;
}

const QString &GameFile::niceName() const
{
	return d->niceName;
}

void GameFile::setNiceName(const QString &name)
{
	d->niceName = name;
}

QStringList &GameFile::searchSuffixes() const
{
	return d->searchSuffixes;
}

void GameFile::setSearchSuffixes(const QStringList &suffixes)
{
	d->searchSuffixes = suffixes;
}

///////////////////////////////////////////////////////////////////////////

DClass<GameFileList>
{
public:
	QList<GameFile> list;
};

DPointered(GameFileList)


GameFileList::GameFileList()
{
}

GameFileList::~GameFileList()
{
}

void GameFileList::add(const GameFile &gameFile)
{
	d->list << gameFile;
}

QList<GameFile> GameFileList::asQList() const
{
	return d->list;
}

void GameFileList::clear()
{
	d->list.clear();
}

GameFileList& operator<<(GameFileList &list, const GameFile &gameFile)
{
	list.add(gameFile);
	return list;
}

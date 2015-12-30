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
	bool clientExecutable;
	bool serverExecutable;
	bool offlineExecutable;
	QString fileName;
	QString niceName;
	QStringList searchSuffixes;
};

DPointered(GameFile)

GameFile::GameFile()
{
	d->clientExecutable = false;
	d->serverExecutable = false;
	d->offlineExecutable = false;
}

GameFile::~GameFile()
{
}

const QString &GameFile::configName() const
{
	return d->configName;
}

GameFile &GameFile::setConfigName(const QString &name)
{
	d->configName = name;
	return *this;
}

const QString &GameFile::fileName() const
{
	return d->fileName;
}

GameFile &GameFile::setFileName(const QString &name)
{
	d->fileName = name;
	return *this;
}

bool GameFile::isExecutable() const
{
	return isClientExecutable() || isServerExecutable() || isOfflineExecutable();
}

bool GameFile::isClientExecutable() const
{
	return d->clientExecutable;
}

GameFile &GameFile::setClientExecutable(bool b)
{
	d->clientExecutable = b;
	return *this;
}

bool GameFile::isServerExecutable() const
{
	return d->serverExecutable;
}

GameFile &GameFile::setServerExecutable(bool b)
{
	d->serverExecutable = b;
	return *this;
}

bool GameFile::isOfflineExecutable() const
{
	return d->offlineExecutable;
}

GameFile &GameFile::setOfflineExecutable(bool b)
{
	d->offlineExecutable = b;
	return *this;
}

GameFile &GameFile::setCsoModesExecutable(bool b)
{
	return setClientExecutable(b).setServerExecutable(b).setOfflineExecutable(b);
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

bool GameFile::isValid() const
{
	return !configName().isEmpty();
}

const QString &GameFile::niceName() const
{
	return d->niceName;
}

GameFile &GameFile::setNiceName(const QString &name)
{
	d->niceName = name;
	return *this;
}

QStringList &GameFile::searchSuffixes() const
{
	return d->searchSuffixes;
}

GameFile &GameFile::setSearchSuffixes(const QStringList &suffixes)
{
	d->searchSuffixes = suffixes;
	return *this;
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

GameFileList &GameFileList::append(const GameFile &gameFile)
{
	d->list << gameFile;
	return *this;
}

GameFileList &GameFileList::append(const GameFileList &list)
{
	foreach (const GameFile &other, list.asQList())
	{
		append(other);
	}
	return *this;
}

QList<GameFile> GameFileList::asQList() const
{
	return d->list;
}

void GameFileList::clear()
{
	d->list.clear();
}

GameFile GameFileList::first() const
{
	foreach (const GameFile &file, d->list)
	{
		if (file.isValid())
		{
			return file;
		}
	}
	return GameFile();
}

GameFile GameFileList::findByConfigName(const QString &configName)
{
	foreach (const GameFile &file, d->list)
	{
		if (file.configName() == configName)
		{
			return file;
		}
	}
	return GameFile();
}

bool GameFileList::isEmpty() const
{
	return d->list.isEmpty();
}

GameFileList &GameFileList::prepend(const GameFile &gameFile)
{
	d->list.prepend(gameFile);
	return *this;
}

GameFileList& operator<<(GameFileList &list, const GameFile &gameFile)
{
	list.append(gameFile);
	return list;
}

GameFileList& operator<<(GameFileList &list, const GameFileList &other)
{
	list.append(other);
	return list;
}

///////////////////////////////////////////////////////////////////////////

GameFileList GameFiles::allCreateGameExecutables(const GameFileList &list)
{
	GameFileList result;
	foreach (const GameFile &file, list.asQList())
	{
		if ((file.isOfflineExecutable() || file.isServerExecutable()) && file.isValid())
		{
			result << file;
		}
	}
	return result;
}

GameFileList GameFiles::allClientExecutables(const GameFileList &list)
{
	GameFileList result;
	foreach (const GameFile &file, list.asQList())
	{
		if (file.isClientExecutable() && file.isValid())
		{
			result << file;
		}
	}
	return result;
}

GameFileList GameFiles::allServerExecutables(const GameFileList &list)
{
	GameFileList result;
	foreach (const GameFile &file, list.asQList())
	{
		if (file.isServerExecutable() && file.isValid())
		{
			result << file;
		}
	}
	return result;
}

GameFile GameFiles::defaultClientExecutable(const GameFileList &list)
{
	foreach (const GameFile &file, list.asQList())
	{
		if (file.isValid() && file.isClientExecutable() && file.configName() == "BinaryPath")
		{
			return file;
		}
	}
	return GameFile();
}

GameFile GameFiles::defaultServerExecutable(const GameFileList &list)
{
	foreach (const GameFile &file, list.asQList())
	{
		if (file.isValid() && file.isServerExecutable()
			&& (file.configName() == "ServerBinaryPath" || file.configName() == "BinaryPath"))
		{
			return file;
		}
	}
	return GameFile();
}

//------------------------------------------------------------------------------
// gamefile.h
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
#ifndef idc66bde0f_a415_4f5a_8dcb_34697293515c
#define idc66bde0f_a415_4f5a_8dcb_34697293515c

#include "dptr.h"
#include "global.h"
#include <QList>
#include <QString>

/**
 * @ingroup group_pluginapi
 * @brief Game file definition allows to browse this file in configuration box.
 *
 * There are some standard values for configName() property:
 *
 * - BinaryPath is the general game executable path. It's always used as the
 *   client executable when joining servers and as offline game executable.
 *   It can also be used to host servers if EnginePlugin::Data::clientOnly
 *   is true.
 * - ServerBinaryPath is path to server executable. It's used when
 *   EnginePlugin::Data::clientOnly is false.
 *
 * Apart from these, the configName can be anything you want and anything your
 * plugin needs. Remember however that choosing a distinct name is important to
 * avoid collisions with any future standard names.
 */
class MAIN_EXPORT GameFile
{
public:
	/**
	 * @brief Builds GameFile suitable for executable files.
	 *
	 * isExecutable() will return true.
	 */
	static GameFile exe(const QString &configName, const QString &niceName,
		const QString &fileName);

	GameFile();
	virtual ~GameFile();

	/**
	 * @brief Is this an executable file.
	 */
	bool isExecutable() const;
	/**
	 * @brief Guesses by file name if this is the same file.
	 */
	bool isSameFile(const QString &otherFileName);

	/**
	 * @brief Setting name where path will be stored in plugin's IniSection.
	 */
	const QString &configName() const;
	void setConfigName(const QString &name);

	/**
	 * @brief Name of the file on disk.
	 *
	 * Platform specific extensions (.exe) are unnecessary.
	 */
	const QString &fileName() const;
	void setFileName(const QString &name);

	/**
	 * @brief Descriptive name, ie. "client executable", "server executable",
	 * etc.
	 */
	const QString &niceName() const;
	void setNiceName(const QString &name);

	/**
	 * @brief Path suffixes that help in automatically finding this file.
	 *
	 * @see EnginePlugin::InitFeatures::EP_GameFileSearchSuffixes.
	 */
	QStringList &searchSuffixes() const;
	void setSearchSuffixes(const QStringList &suffixes);

private:
	DPtr<GameFile> d;
};


/**
 * @ingroup group_pluginapi
 * @brief GameFile collection.
 */
class MAIN_EXPORT GameFileList
{
	friend GameFileList& operator<<(GameFileList &list, const GameFile &gameFile);

public:
	GameFileList();
	virtual ~GameFileList();

	void add(const GameFile &gameFile);
	QList<GameFile> asQList() const;
	void clear();

private:
	DPtr<GameFileList> d;
};

#endif

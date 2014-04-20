//------------------------------------------------------------------------------
// doomseekerfilepaths.h
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __DOOMSEEKERFILEPATHS_H__
#define __DOOMSEEKERFILEPATHS_H__

#include "datapaths.h"
#include "global.h"
#include <QString>

/**
 * @ingroup group_pluginapi
 * @brief Static class responsible for retrieving paths to data files used
 *        by Doomseeker.
 *
 * This uses a static instance of DataPaths class through pDataPaths member,
 * which should be specified during init phase.
 */
class MAIN_EXPORT DoomseekerFilePaths
{
	public:
		static const QString IP2C_DATABASE_FILENAME;
		static const QString IP2C_QT_SEARCH_PATH;
		static const QString TEMP_SERVER_CONFIG_FILENAME;
		static const QString INI_FILENAME;
		static const QString IRC_INI_FILENAME;
		static const QString PASSWORD_INI_FILENAME;

		static DataPaths* pDataPaths;

		static QString ini();
		static QString ircIni();
		static QString ip2cDatabase();
		static QString passwordIni();
		static QString tempServerConfig();
		static QString updatePackagesStorageDir();

	private:
		static QString joinIfNeitherEmpty(const QString &left, const QString &right);
};

#endif

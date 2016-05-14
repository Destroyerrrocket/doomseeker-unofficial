//------------------------------------------------------------------------------
// doomseekerfilepaths.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "doomseekerfilepaths.h"

DataPaths* DoomseekerFilePaths::pDataPaths = NULL;

const QString DoomseekerFilePaths::IP2C_DATABASE_FILENAME = "IpToCountry.dat";
const QString DoomseekerFilePaths::IP2C_QT_SEARCH_PATH = "data:" + IP2C_DATABASE_FILENAME;
const QString DoomseekerFilePaths::TEMP_SERVER_CONFIG_FILENAME = "tmpserver.cfg";
const QString DoomseekerFilePaths::INI_FILENAME = "doomseeker.ini";
const QString DoomseekerFilePaths::IRC_INI_FILENAME = "doomseeker-irc.ini";
const QString DoomseekerFilePaths::PASSWORD_INI_FILENAME = "doomseeker-password.ini";


QString DoomseekerFilePaths::ini()
{
	return joinIfNeitherEmpty(pDataPaths->programsDataDirectoryPath(), INI_FILENAME);
}

QString DoomseekerFilePaths::ircIni()
{
	return joinIfNeitherEmpty(pDataPaths->programsDataDirectoryPath(), IRC_INI_FILENAME);
}

QString DoomseekerFilePaths::ip2cDatabase()
{
	return joinIfNeitherEmpty(pDataPaths->programsDataSupportDirectoryPath(), IP2C_DATABASE_FILENAME);
}

QString DoomseekerFilePaths::joinIfNeitherEmpty(const QString &left, const QString &right)
{
	if (left.trimmed().isEmpty() || right.trimmed().isEmpty())
	{
		return QString();
	}
	return left + "/" + right;
}

QString DoomseekerFilePaths::passwordIni()
{
	return joinIfNeitherEmpty(pDataPaths->programsDataDirectoryPath(), PASSWORD_INI_FILENAME);
}

QString DoomseekerFilePaths::tempServerConfig()
{
	return joinIfNeitherEmpty(pDataPaths->programsDataDirectoryPath(), TEMP_SERVER_CONFIG_FILENAME);
}

QString DoomseekerFilePaths::updatePackagesStorageDir()
{
	return pDataPaths->localDataLocationPath(DataPaths::UPDATE_PACKAGES_DIR_NAME);
}

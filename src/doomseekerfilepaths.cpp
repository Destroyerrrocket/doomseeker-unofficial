//------------------------------------------------------------------------------
// doomseekerfilepaths.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "doomseekerfilepaths.h"

DataPaths*		DoomseekerFilePaths::pDataPaths = NULL;

const QString	DoomseekerFilePaths::CONFIG_FILENAME				= "doomseeker.cfg";
const QString	DoomseekerFilePaths::IP2C_DATABASE_FILENAME			= "IpToCountry.csv";
const QString	DoomseekerFilePaths::TEMP_SERVER_CONFIG_FILENAME	= "tmpserver.cfg";
		
QString DoomseekerFilePaths::config()
{
	return pDataPaths->programsDataDirectoryPath() + "/" + CONFIG_FILENAME;
}

QString DoomseekerFilePaths::ip2cDatabase()
{
	return pDataPaths->programsDataDirectoryPath() + "/" + IP2C_DATABASE_FILENAME;
}

QString DoomseekerFilePaths::tempServerConfig()
{
	return pDataPaths->programsDataDirectoryPath() + "/" + TEMP_SERVER_CONFIG_FILENAME;
}

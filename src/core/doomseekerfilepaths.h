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
 *	@brief Static class responsible for retrieving paths to data files used
 *	by Doomseeker.
 *
 *	This uses a static instance of DataPaths class which should be specified
 *	by Main during init.
 */
class MAIN_EXPORT DoomseekerFilePaths
{
	public:
		static const QString	CONFIG_FILENAME;
		static const QString	IP2C_DATABASE_FILENAME;
		static const QString	IP2C_QT_SEARCH_PATH;
		static const QString	TEMP_SERVER_CONFIG_FILENAME;
	
		static DataPaths*		pDataPaths;
		
		static QString			config();
		static QString			ip2cDatabase();
		static QString			tempServerConfig();
};

#endif

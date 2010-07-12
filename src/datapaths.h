//------------------------------------------------------------------------------
// datapaths.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __DATAPATHS_H__
#define __DATAPATHS_H__

#include <QDir>
#include <QString>
#include <QStringList>

/**
 *	@brief Represents directories used by Doomseeker to store data.
 *
 *	Since Doomseeker doesn't need to store much all data is stored in a single
 *	directory. For portable model this is the same directory as in which 
 *	Doomseeker.exe resides. Otherwise .doomseeker/ directory is created
 *	in a path depending on the operating system.
 */
class DataPaths
{
	public:
		static const QString	PROGRAM_APPDATA_DIR_NAME;
	
		DataPaths(bool bPortableModeOn = false);

		/**
		 *	@brief Checks if all directories can be written to.
		 *
		 *	@return List of directories for which the test FAILED.
		 */		
		QStringList				canWrite() const;
		
		/**
		 *	@brief Creates necessary directories for application run.
		 *
		 *	If directories exist nothing happens.
		 *
		 *	@return If it was impossible to create at least one of the required
		 *	directories false will be returned. True is returned only on full
		 *	success.
		 *
		 *	@see directoriesExist()
		 */
		bool					createDirectories();

		QString					dataDirectoryPath() const;
		
		/**
		 *	@brief Checks if all necessary directories exist.
		 *
		 *	@return List of directories that DO NOT exist.
		 */
		QStringList				directoriesExist() const;
		
		bool					isPortableModeOn() const { return bIsPortableModeOn; }
		
		void					setPortableModeOn(bool b) { bIsPortableModeOn = b; }

		/**
		 *	@brief Checks if the root directory for Doomseeker data storage
		 *	is accessible.
		 */
		bool					validateAppDataDirectory();
				
	protected:
		/**
		 *	@return True if path is a directory that exists and can be written 
		 *	to.
		 */
		static bool				validateDir(const QString& path);	
	
		bool					bIsPortableModeOn;
		
		/**
		 *	@brief Gets path to the root directory for data storage.
		 *
		 *	If portable mode is ON this points to the appliation's directory.
		 *	Otherwise:
		 *
		 *	For Windows this is determined based on %APPDATA% environment 
		 *	variable. If this cannot be found then QDir::home() is used.
		 *
		 *	On other systems QDir::home() is used directly.
		 *
		 *	@param append - this string will be appended to the returned path.
		 *
		 *	@return Empty string if directory doesn't pass validateDir() check.
		 *	Otherwise the path returned is always absolute.
		 */
		QString					appDataDirectory(QString append = QString()) const;
		
		/**
		 *	@brief If directory already exists true is returned.
		 */
		bool					tryCreateDirectory(const QDir& rootDir, const QString& dirToCreate) const;
};

#endif

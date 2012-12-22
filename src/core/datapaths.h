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

#include "global.h"
#include <QDir>
#include <QString>
#include <QStringList>

/**
 *	@brief Represents directories used by Doomseeker to store data.
 *
 *	Since Doomseeker doesn't need to store much all data is stored in a single
 *	directory. For portable model this is the same directory as in which
 *	Doomseeker.exe resides. Otherwise .doomseeker/ directory can be created
 *	in a path depending on the operating system.
 */
class MAIN_EXPORT DataPaths
{
	public:
		enum MachineType
		{
			x86,
			x64,
			Preferred
		};

		static const QString	PROGRAMS_APPDATA_DIR_NAME;
		static const QString    PROGRAMS_APPDATASUPPORT_DIR_NAME;
		static const QString	DEMOS_DIR_NAME;
		static const QString	TRANSLATIONS_DIR_NAME;
		static const QString UPDATE_PACKAGES_DIR_NAME;

		/**
		 *	@b Retrieves correct path to "Program Files" directory.
		 *	Windows only.
		 *
		 *	This exploits environmental variables such as %PROGRAMFILES%,
		 *	%PROGRAMFILES(X86)% and ProgramW6432. This method is used to
		 *	determine the correct path to the "Program Files" directory on
		 *	Windows (XP and above). Since on *nix systems there is no equivalent
		 *	it will return an empty string.
		 */
		static QString			programFilesDirectory(MachineType machineType);

		/**
		 * @brief Paths to directories where program should search for its
		 *        static data.
		 *
		 * By static data we understand read only files which come preinstalled
		 * with the program. These files should reside in known locations.
		 *
		 * @param subdir
		 *     If this sub-path is specified then then it's appended to all
		 *     returned paths.
		 */
		static QStringList staticDataSearchDirs(const QString& subdir = QString());

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

		QString					demosDirectoryPath() const;

		/**
		 *	@brief Checks if all necessary directories exist.
		 *
		 *	@return List of directories that DO NOT exist.
		 */
		QStringList				directoriesExist() const;

		const QString&			directoryNameForProgram() const { return programsDirectoryName; }

		/**
		 * @brief Path to the directory where large data should be
		 *        saved.
		 *
		 * This path is supposed to hold data which should be stored only
		 * the local file system. If portable mode is on this returns path
		 * to program directory with "/.static" appended. If portable mode
		 * is off this returns path to QDesktopServices::DataLocation with
		 * "/.PROGRAMS_APPDATA_DIR_NAME" appended.
		 *
		 * @param subpath
		 *     If specified then this path is appended to the returned path.
		 */
		QString localDataLocationPath(const QString& subpath = QString()) const;

		/**
		 *	@brief Path to directory where this concrete application should
		 *	store it's data.
		 *
		 *	Depending on model (portable or not) and operating system this might
		 *	point to a number of different directories. However the root dir
		 *	is determined by calling the systemAppDataDirectory() method and
		 *	appending string contained in programsDirectoryName member.
		 */
		QString					programsDataDirectoryPath() const;

		/**
		 *  @brief Allows switching from Preferences to Application Support on OS X.
		 *
		 *  Same as programsDataDirectoryPath() on other systems or in portable mode.
		 */
		QString					programsDataSupportDirectoryPath() const;

		bool					isPortableModeOn() const { return bIsPortableModeOn; }

		void					setPortableModeOn(bool b) { bIsPortableModeOn = b; }
		void					setDirectoryNameForProgram(const QString& name) { programsDirectoryName = name; }

		/**
		 *	@brief Gets path to the root directory for data storage.
		 *
		 *	If portable mode is ON this points to the application's directory.
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
		QString					systemAppDataDirectory(QString append = QString()) const;

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
		 *	@brief Defaults to PROGRAMS_APPDATA_DIR_NAME.
		 */
		QString					programsDirectoryName;
		QString					programsSupportDirectoryName;
		QString					demosDirectoryName;

		/**
		 *	@brief If directory already exists true is returned.
		 */
		bool					tryCreateDirectory(const QDir& rootDir, const QString& dirToCreate) const;
};

#endif

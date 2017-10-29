//------------------------------------------------------------------------------
// datapaths.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __DATAPATHS_H__
#define __DATAPATHS_H__

#include "global.h"
#include "dptr.h"
#include <QDir>
#include <QString>
#include <QStringList>

#define gDefaultDataPaths (DataPaths::defaultInstance())

class EnginePlugin;

/**
 * @ingroup group_pluginapi
 * @brief Represents directories used by Doomseeker to store data.
 *
 * Doomseeker data is stored in three general directories: the 'configuration
 * storage' dir, the 'local data storage' dir (using Windows nomenclature:
 * Roaming & Local, respectively), and the 'My Documents' dir.
 * If portable mode is *disabled* (the default),
 * these directories are created in according to the current Operating System
 * standards or according to historical behavior of older Doomseeker
 * versions. Also if portable mode is disabled then, depending on the platform
 * and its configuration, some of those directories might be on a Network File
 * System. For portable model both directories are created where Doomseeker's
 * executable resides.
 *
 * What are the exact names of those directories and where they're physically
 * located should be inconsequential. The contract here is that it's ensured
 * that those directories are writable, however the paths may change between
 * Doomseeker versions. To preserve specific directory, it's preferable to store
 * its path in a configuration setting and allow user to modify it. The rule
 * of thumb for picking directory type is this:
 *
 * - programsDataDirectoryPath() is the "Roaming" directory. It should be used to
 *   store relatively small amount of data - like config files.
 * - localDataLocationPath() is the "Local" directory. It can be used to store
 *   data that's large or only valid to the particular machine that is running
 *   Doomseeker (like cache).
 * - documentsLocationPath() is the "My Documents" directory with
 *   "doomseeker" appended to it. Store here files that should also be normally
 *   accessible to the user through directory navigation. On Windows platform
 *   it has also a benefit of additional hard drive space as it is very easy
 *   to move "My Documents" dir to a different partition or drive.
 *
 * @note
 * You might've noticed that Doomseeker itself doesn't always follow these rules
 * of thumb. This unfortunate behavior is the result of insufficient knowledge
 * at the time when this was first developed on how to properly design system
 * like these and how to utilize Qt framework, which already has convenient
 * functions that help to solve this problem. This behavior might change in
 * future versions of Doomseeker.
 *
 * Plugins, to store their "Local" data, can use pluginLocalDataLocationPath()
 * method to obtain the path where this data can be stored. To store "Documents"
 * data, use pluginDocumentsLocationPath().
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

		static const QString PROGRAMS_APPDATA_DIR_NAME;
		static const QString PROGRAMS_APPDATASUPPORT_DIR_NAME;
		static const QString DEMOS_DIR_NAME;
		static const QString CHATLOGS_DIR_NAME;
		static const QString TRANSLATIONS_DIR_NAME;
		static const QString UPDATE_PACKAGES_DIR_NAME;
		static const QString UPDATE_PACKAGE_FILENAME_PREFIX;

		/**
		 * @b Retrieves correct path to "Program Files" directory.
		 * Windows only.
		 *
		 * This exploits environmental variables such as %PROGRAMFILES%,
		 * %PROGRAMFILES(X86)% and ProgramW6432. This method is used to
		 * determine the correct path to the "Program Files" directory on
		 * Windows (XP and above). Since on *nix systems there is no equivalent
		 * it will return an empty string.
		 */
		static QString programFilesDirectory(MachineType machineType);

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

		static void initDefault(bool bPortableModeOn);
		/**
		 * @brief Retrieves default instance that is used throughout
		 *        the program.
		 *
		 * This instance must first be init with initDefault().
		 */
		static DataPaths *defaultInstance();

		DataPaths(bool bPortableModeOn = false);
		virtual ~DataPaths();

		/**
		 * @brief Checks if all directories can be written to.
		 *
		 * @return List of directories for which the test FAILED.
		 */
		QStringList canWrite() const;

		/**
		 * @brief Creates necessary directories for application run.
		 *
		 * If directories exist nothing happens.
		 *
		 * @return If it was impossible to create at least one of the required
		 * directories false will be returned. True is returned only on full
		 * success.
		 *
		 * @see directoriesExist()
		 */
		bool createDirectories();

		QString demosDirectoryPath() const;

		/**
		 * @brief Checks if all necessary directories exist.
		 *
		 * @return List of directories that DO NOT exist.
		 */
		QStringList directoriesExist() const;

		/**
		 * @brief Path to the "My Documents" directory with Doomseeker's
		 * own subpath suffix.
		 *
		 * In portable mode is on this will be the program directory with
		 * "storage" appended to it.
		 *
		 * @param subpath
		 *     Additional subpath in the "My Documents/Doomseeker" subpath.
		 */
		QString documentsLocationPath(const QString &subpath = QString()) const;

		/**
		 * @brief Path to the directory where large data should be
		 *        saved.
		 *
		 * This path is supposed to hold data which should be stored only
		 * the local file system. If portable mode is on this returns path
		 * to program directory with "/.static" appended. If portable mode
		 * is off this returns path to QDesktopServices::DataLocation with
		 * "/QCoreApplication::applicationName()" appended.
		 *
		 * @param subpath
		 *     If specified then this path is appended to the returned path.
		 */
		QString localDataLocationPath(const QString& subpath = QString()) const;

		/**
		 * @brief Place where EnginePlugin can store its "My Documents" content.
		 *
		 * This path is a directory path created by documentsDataLocationPath()
		 * with suffix unique for each plugin. The suffix is derived in the same
		 * way as in pluginLocalDataLocationPath().
		 *
		 * This method is to be used by plugins.
		 *
		 * @param plugin
		 *     Plugin must pass reference to its implementation of EnginePlugin.
		 */
		QString pluginDocumentsLocationPath(const EnginePlugin &plugin) const;

		/**
		 * @brief Place where EnginePlugin can store its local files.
		 *
		 * This path is a directory path created by localDataLocationPath() with
		 * suffix unique for each plugin. The suffix is partially derived from
		 * EnginePlugin::nameCanonical() and ensured to remain constant as long
		 * as EnginePlugin::nameCanonical() doesn't change for given plugin. It
		 * also takes portable mode into consideration. However, it's not
		 * ensured that the directory will exist.
		 *
		 * This method is to be used from plugins.
		 *
		 * @param plugin
		 *     Plugin must pass reference to its implementation of EnginePlugin.
		 */
		QString pluginLocalDataLocationPath(const EnginePlugin &plugin) const;

		/**
		 * @brief Ordered locations were plugin libraries could be loaded from.
		 *
		 * Provides an ordered list of locations where PluginLoader will attempt
		 * to load EnginePlugins from.
		 */
		QStringList pluginSearchLocationPaths() const;

		/**
		 * @brief Path to directory where this concrete application should
		 * store its data.
		 *
		 * Depending on model (portable or not) and operating system this might
		 * point to a number of different directories. However the root dir
		 * is determined by calling the systemAppDataDirectory() method and
		 * appending string returned by programDirName() getter.
		 */
		QString programsDataDirectoryPath() const;

		/**
		 * @brief Defaults to PROGRAMS_APPDATA_DIR_NAME.
		 */
		const QString &programDirName() const;

		/**
		 * @brief Allows switching from Preferences to Application Support on OS X.
		 *
		 * Same as programsDataDirectoryPath() on other systems or in portable mode.
		 */
		QString programsDataSupportDirectoryPath() const;

		bool isPortableModeOn() const;

		void setPortableModeOn(bool b);
		void setProgramDirName(const QString& name);
		/**
		 * @brief Changes the location returned by workingDirectory.
		 *
		 * No longer used internally. Previously this was used to logically set
		 * the "working directory" to the location of the application binary.
		 * This makes the name of the functions something of a misnomer.
		 */
		void setWorkingDirectory(const QString &workingDirectory);

		/**
		 * @brief Gets path to the root directory for data storage.
		 *
		 * If portable mode is ON this points to the application's directory.
		 * Otherwise:
		 *
		 * For Windows this is determined based on %APPDATA% environment
		 * variable. If this cannot be found then QDir::home() is used.
		 *
		 * On other systems QDir::home() is used directly.
		 *
		 * @param append - this string will be appended to the returned path.
		 *
		 * @return Empty string if directory doesn't pass validateDir() check.
		 * Otherwise the path returned is always absolute.
		 */
		QString systemAppDataDirectory(QString append = QString()) const;

		/**
		 * @brief Checks if the root directory for Doomseeker data storage
		 * is accessible.
		 */
		bool validateAppDataDirectory();

		/**
		 * @brief Program working directory.
		 *
		 * Despite the name of the function, for historical reasons, this
		 * actually returns the binary's location as determined by
		 * QCoreApplication::applicationDataPath.
		 */
		const QString &workingDirectory() const;

	protected:
		/**
		 * @return True if path is a directory that exists and can be written
		 * to.
		 */
		static bool validateDir(const QString& path);

		/**
		 * @brief If directory already exists true is returned.
		 */
		bool tryCreateDirectory(const QDir& rootDir, const QString& dirToCreate) const;

	private:
		DPtr<DataPaths> d;

		static QString env(const QString &key);
		static DataPaths *staticDefaultInstance;
};

#endif

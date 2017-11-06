//------------------------------------------------------------------------------
// datapaths.cpp
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
#include "datapaths.h"

#include <QCoreApplication>
#include "plugins/engineplugin.h"
#include "strings.hpp"
#include <QDesktopServices>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <cassert>
#include <cstdlib>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

// Sanity check for INSTALL_PREFIX and INSTALL_LIBDIR
#if !defined(INSTALL_PREFIX) || !defined(INSTALL_LIBDIR)
#error Build system should provide definition for INSTALL_PREFIX and INSTALL_LIBDIR
#endif

static QStringList uniquePaths(const QStringList &paths)
{
	QList<QFileInfo> uniqueMarkers;
	QStringList result;
	foreach (const QString &path, paths)
	{
		if (!uniqueMarkers.contains(path))
		{
			uniqueMarkers << path;
			result << path;
		}
	}
	return result;
}

DClass<DataPaths>
{
	public:
		static const QString PLUGINS_DIR_NAME;

		bool bIsPortableModeOn;
		QString programsDirectoryName;
		QString programsSupportDirectoryName;
		QString demosDirectoryName;
		QString workingDirectory;
};

DPointered(DataPaths)

DataPaths *DataPaths::staticDefaultInstance = NULL;


#ifdef Q_OS_MAC
const QString DataPaths::PROGRAMS_APPDATA_DIR_NAME = "Library/Preferences/Doomseeker";
const QString DataPaths::PROGRAMS_APPDATASUPPORT_DIR_NAME = "Library/Application Support/Doomseeker";
#else
const QString DataPaths::PROGRAMS_APPDATA_DIR_NAME = ".doomseeker";
const QString DataPaths::PROGRAMS_APPDATASUPPORT_DIR_NAME = "";
#endif
const QString DataPaths::DEMOS_DIR_NAME = "demos";
const QString DataPaths::CHATLOGS_DIR_NAME = "chatlogs";
const QString PrivData<DataPaths>::PLUGINS_DIR_NAME = "plugins";
const QString DataPaths::TRANSLATIONS_DIR_NAME = "translations";
const QString DataPaths::UPDATE_PACKAGES_DIR_NAME = "updates";
const QString DataPaths::UPDATE_PACKAGE_FILENAME_PREFIX = "doomseeker-update-pkg-";

DataPaths::DataPaths(bool bPortableModeOn)
{
	d->bIsPortableModeOn = bPortableModeOn;

	d->programsDirectoryName = PROGRAMS_APPDATA_DIR_NAME;
	d->programsSupportDirectoryName = PROGRAMS_APPDATASUPPORT_DIR_NAME;
	d->demosDirectoryName = PROGRAMS_APPDATA_DIR_NAME + QDir::separator() + DEMOS_DIR_NAME;

	// Logically this would be "./" but our only use of this class as of
	// Doomseeker 1.1 would use setWorkingDirectory to applicationDirPath()
	d->workingDirectory = QCoreApplication::applicationDirPath();
}

DataPaths::~DataPaths()
{
}

QStringList DataPaths::canWrite() const
{
	QStringList failedList;

	QString dataDirectory = programsDataDirectoryPath();
	if (!validateDir(dataDirectory))
	{
		failedList.append(dataDirectory);
	}

	return failedList;
}

bool DataPaths::createDirectories()
{
	// This variable should only be changed to false and only if something
	// fails.
	bool bAllSuccessful = true;

	QDir appDataDir(systemAppDataDirectory());
	if (!tryCreateDirectory(appDataDir, programDirName()))
	{
		bAllSuccessful = false;
	}
	if (!programsDataSupportDirectoryPath().isEmpty()
		&& !tryCreateDirectory(appDataDir, programsDataSupportDirectoryPath()))
	{
		bAllSuccessful = false;
	}

	QDir programDirectory(programsDataDirectoryPath());
	if (!tryCreateDirectory(programDirectory, "demos"))
	{
		bAllSuccessful = false;
	}

	return bAllSuccessful;
}

DataPaths *DataPaths::defaultInstance()
{
	return staticDefaultInstance;
}


QStringList DataPaths::defaultWadPaths() const
{
	QStringList filePaths;
	filePaths << programsDataDirectoryPath();

	// The directory which contains the Doomseeker executable may be a good
	// default choice, but on unix systems the bin directory is not worth
	// searching.
	if(QDir(workingDirectory()).dirName() != "bin")
		filePaths << workingDirectory();

	return filePaths;
}

QString DataPaths::demosDirectoryPath() const
{
	QString demosDir = systemAppDataDirectory(d->demosDirectoryName);
	return demosDir;
}

QStringList DataPaths::directoriesExist() const
{
	QStringList failedList;
	QList<QDir> checkList;

	checkList << programsDataDirectoryPath();
	if (!d->programsSupportDirectoryName.isEmpty())
		checkList << programsDataSupportDirectoryPath();

	foreach(const QDir &dataDirectory, checkList)
	{
		if (!dataDirectory.exists())
		{
			failedList.append(dataDirectory.absolutePath());
		}
	}

	return failedList;
}

QString DataPaths::documentsLocationPath(const QString &subpath) const
{
	QString rootPath;
	if (!isPortableModeOn())
	{
#if QT_VERSION >= 0x050000
		rootPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
#else
		rootPath = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#endif
		rootPath = Strings::combinePaths(rootPath, "doomseeker");
	}
	else
	{
		rootPath = systemAppDataDirectory("storage");
	}
	return Strings::combinePaths(rootPath, subpath);
}

QString DataPaths::env(const QString &key)
{
	return QProcessEnvironment::systemEnvironment().value(key);
}

const QString& DataPaths::programDirName() const
{
	return d->programsDirectoryName;
}

void DataPaths::initDefault(bool bPortableModeOn)
{
	assert(staticDefaultInstance == NULL && "DataPaths can have only one default.");
	if (staticDefaultInstance == NULL)
	{
		staticDefaultInstance = new DataPaths(bPortableModeOn);
	}
}

bool DataPaths::isPortableModeOn() const
{
	return d->bIsPortableModeOn;
}

void DataPaths::setPortableModeOn(bool b)
{
	d->bIsPortableModeOn = b;
}

void DataPaths::setProgramDirName(const QString& name)
{
	d->programsDirectoryName = name;
}

QString DataPaths::localDataLocationPath(const QString& subpath) const
{
	QString rootPath;
	if (!isPortableModeOn())
	{
#if QT_VERSION >= 0x050000
		rootPath = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
#else
		rootPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
	}
	else
	{
		rootPath = systemAppDataDirectory(".static");
	}
	return Strings::combinePaths(rootPath, subpath);
}

QString DataPaths::pluginLocalDataLocationPath(const EnginePlugin &plugin) const
{
	return localDataLocationPath(QString("%1/%2").arg(
			PrivData<DataPaths>::PLUGINS_DIR_NAME, plugin.nameCanonical()));
}

QString DataPaths::pluginDocumentsLocationPath(const EnginePlugin &plugin) const
{
	return documentsLocationPath(QString("%1/%2").arg(
			PrivData<DataPaths>::PLUGINS_DIR_NAME, plugin.nameCanonical()));
}

QStringList DataPaths::pluginSearchLocationPaths() const
{
	QStringList paths;
	paths.append(programsDataSupportDirectoryPath());
	paths.append(workingDirectory());
	paths.append("./");
#ifndef Q_OS_WIN32
	paths.append(INSTALL_PREFIX "/" INSTALL_LIBDIR "/doomseeker/");
#endif
	paths = uniquePaths(paths);
	return Strings::combineManyPaths(paths, "engines/");
}

QString DataPaths::programFilesDirectory(MachineType machineType)
{
	#ifdef Q_OS_WIN32
		QString envVarName = "";

		switch (machineType)
		{
			case x86:
				envVarName = "ProgramFiles(x86)";
				break;

			case x64:
				envVarName = "ProgramW6432";
				break;

			case Preferred:
				envVarName = "ProgramFiles";
				break;

			default:
				return QString();
		}

		QString path = env(envVarName);
		if (path.isEmpty() && machineType != Preferred)
		{
			// Empty outcome may happen on 32-bit systems where variables
			// like "ProgramFiles(x86)" may not exist.
			//
			// If "ProgramFiles" variable is empty then something is seriously
			// wrong with the system.
			path = programFilesDirectory(Preferred);
		}

		return path;

	#else
		return QString();
	#endif
}

QString DataPaths::programsDataDirectoryPath() const
{
	QString appDataDir = systemAppDataDirectory(programDirName());
	return appDataDir;
}

QString DataPaths::programsDataSupportDirectoryPath() const
{
	if (isPortableModeOn() || d->programsSupportDirectoryName.isEmpty())
		return programsDataDirectoryPath();

	QString appSupportDataDir = systemAppDataDirectory(d->programsSupportDirectoryName);
	return appSupportDataDir;
}

void DataPaths::setWorkingDirectory(const QString &workingDirectory)
{
	d->workingDirectory = workingDirectory;
}

QStringList DataPaths::staticDataSearchDirs(const QString& subdir)
{
	QStringList paths;
	paths.append(QDir::currentPath()); // current working dir
	paths.append(QCoreApplication::applicationDirPath()); // where exe is located
#ifndef Q_OS_WIN32
	paths.append(INSTALL_PREFIX "/share/doomseeker"); // standard arch independent linux path
#endif
	paths = uniquePaths(paths);
	QString subdirFiltered = subdir.trimmed();
	if (!subdirFiltered.isEmpty())
	{
		for (int i = 0; i < paths.size(); ++i)
		{
			paths[i] = Strings::combinePaths(paths[i], subdirFiltered);
		}
	}
	return paths;
}

QString DataPaths::systemAppDataDirectory(QString append) const
{
	Strings::triml(append, "/\\");

	if (isPortableModeOn())
	{
		QString path = d->workingDirectory + "/" + append;
		return QDir(path).absolutePath();
	}

	// For non-portable model this continues here:
	QString dir;

	#ifdef Q_OS_WIN32
	// Let's open new block to prevent variable "bleeding".
	{
		QString envVar = env("APPDATA");
		if (validateDir(envVar))
		{
			dir = envVar;
		}
	}
	#endif

	if (dir.isEmpty())
	{
		dir = QDir::homePath();
		if (!validateDir(dir))
		{
			return QString();
		}
	}

	Strings::trimr(dir, "/\\");

	dir += QDir::separator() + append;

	return QDir(dir).absolutePath();
}

bool DataPaths::tryCreateDirectory(const QDir& rootDir, const QString& dirToCreate) const
{
	if (!rootDir.exists(dirToCreate))
	{
		return rootDir.mkdir(dirToCreate);
	}

	return true;
}

bool DataPaths::validateAppDataDirectory()
{
	return validateDir(systemAppDataDirectory());
}

bool DataPaths::validateDir(const QString& path)
{
	QFileInfo fileInfo(path);

	bool bCondition1 = !path.isEmpty();
	bool bCondition2 = fileInfo.exists();
	bool bCondition3 = fileInfo.isDir();

	return bCondition1 && bCondition2 && bCondition3;
}

const QString &DataPaths::workingDirectory() const
{
	return d->workingDirectory;
}

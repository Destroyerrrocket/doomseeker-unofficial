//------------------------------------------------------------------------------
// datapaths.cpp
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
#include "datapaths.h"

#include <QCoreApplication>
#include "strings.h"
#include <QDesktopServices>
#include <cstdlib>

class DataPaths::PrivData
{
	public:
		bool bIsPortableModeOn;
		QString programsDirectoryName;
		QString programsSupportDirectoryName;
		QString demosDirectoryName;
		QString workingDirectory;
};

#ifdef Q_OS_MAC
const QString DataPaths::PROGRAMS_APPDATA_DIR_NAME = "Library/Preferences/Doomseeker";
const QString DataPaths::PROGRAMS_APPDATASUPPORT_DIR_NAME = "Library/Application Support/Doomseeker";
#else
const QString DataPaths::PROGRAMS_APPDATA_DIR_NAME = ".doomseeker";
const QString DataPaths::PROGRAMS_APPDATASUPPORT_DIR_NAME = "";
#endif
const QString DataPaths::DEMOS_DIR_NAME = "demos";
const QString DataPaths::TRANSLATIONS_DIR_NAME = "translations";
const QString DataPaths::UPDATE_PACKAGES_DIR_NAME = "updates";
const QString DataPaths::UPDATE_PACKAGE_FILENAME_PREFIX = "doomseeker-update-pkg-";

DataPaths::DataPaths(bool bPortableModeOn)
{
	d = new PrivData();
	d->bIsPortableModeOn = bPortableModeOn;

	d->programsDirectoryName = PROGRAMS_APPDATA_DIR_NAME;
	d->programsSupportDirectoryName = PROGRAMS_APPDATASUPPORT_DIR_NAME;
	d->demosDirectoryName = PROGRAMS_APPDATA_DIR_NAME + QDir::separator() + DEMOS_DIR_NAME;
	d->workingDirectory = "./";
}

DataPaths::~DataPaths()
{
	delete d;
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

const QString& DataPaths::programDirName() const
{
	return d->programsDirectoryName;
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
	// TODO This won't work correctly on Mac because we didn't use
	// the QDesktopServices from the beginning. Using this class
	// would save a lot of trouble and simplify code in this file a lot.
	// Unfortunatelly right now using it would cause compatibility errors for
	// Linux users who already have Doomseeker installed as the locations
	// returned by QDesktopServices are different in certain cases. However,
	// with some work some compromise could be achieved.
	QString rootPath;
	if (!isPortableModeOn())
	{
		rootPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
		rootPath = Strings::combinePaths(rootPath, ".doomseeker");
	}
	else
	{
		rootPath = systemAppDataDirectory(".static");
	}
	return Strings::combinePaths(rootPath, subpath);
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

		QString path = getenv(envVarName.toAscii().constData());
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
	paths.append("/usr/share/doomseeker"); // standard linux path
	paths.append("/usr/local/share/doomseeker"); // standard linux path 2
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
		QString envVar = getenv("APPDATA");
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


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
#include "main.h"
#include "strings.h"
#include <cstdlib>

#ifdef Q_OS_MAC
const QString DataPaths::PROGRAMS_APPDATA_DIR_NAME = "Library/Preferences/Doomseeker";
const QString DataPaths::PROGRAMS_APPDATASUPPORT_DIR_NAME = "Library/Application Support/Doomseeker";
#else
const QString DataPaths::PROGRAMS_APPDATA_DIR_NAME = ".doomseeker";
const QString DataPaths::PROGRAMS_APPDATASUPPORT_DIR_NAME = "";
#endif
const QString DataPaths::DEMOS_DIR_NAME = "demos";

DataPaths::DataPaths(bool bPortableModeOn)
{
	bIsPortableModeOn = bPortableModeOn;

	programsDirectoryName = PROGRAMS_APPDATA_DIR_NAME;
	programsSupportDirectoryName = PROGRAMS_APPDATASUPPORT_DIR_NAME;
	demosDirectoryName = PROGRAMS_APPDATA_DIR_NAME + QDir::separator() + DEMOS_DIR_NAME;
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
	if (!tryCreateDirectory(appDataDir, programsDirectoryName))
	{
		bAllSuccessful = false;
	}
	if (!programsSupportDirectoryName.isEmpty() && !tryCreateDirectory(appDataDir, programsSupportDirectoryName))
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
	QString demosDir = systemAppDataDirectory(demosDirectoryName);
	return demosDir;
}

QStringList DataPaths::directoriesExist() const
{
	QStringList failedList;
	QList<QDir> checkList;

	checkList << programsDataDirectoryPath();
	if (!programsSupportDirectoryName.isEmpty())
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
	QString appDataDir = systemAppDataDirectory(programsDirectoryName);
	return appDataDir;
}

QString DataPaths::programsDataSupportDirectoryPath() const
{
	if (bIsPortableModeOn || programsSupportDirectoryName.isEmpty())
		return programsDataDirectoryPath();

	QString appSupportDataDir = systemAppDataDirectory(programsSupportDirectoryName);
	return appSupportDataDir;
}

QString DataPaths::systemAppDataDirectory(QString append) const
{
	Strings::triml(append, "/\\");

	if (bIsPortableModeOn)
	{
		// In portable model Main class already stores the "working dir", ie.
		// the directory where the executable resides.
		QString path = Main::workingDirectory + "/" + append;
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

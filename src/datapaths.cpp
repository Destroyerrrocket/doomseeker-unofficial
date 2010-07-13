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

const QString DataPaths::PROGRAM_APPDATA_DIR_NAME = ".doomseeker";

DataPaths::DataPaths(bool bPortableModeOn)
{
	bIsPortableModeOn = bPortableModeOn;
}

QString DataPaths::appDataDirectory(QString append) const
{
	if (bIsPortableModeOn)
	{
		return QDir(Main::workingDirectory).absolutePath();
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
	Strings::triml(append, "/\\");

	dir += QDir::separator() + append;
	
	return QDir(dir).absolutePath();
}

QStringList DataPaths::canWrite() const
{
	QStringList failedList;
	
	QString dataDirectory = dataDirectoryPath();
	if (!validateDir(dataDirectory))
	{
		failedList.append(dataDirectory);
	}

	return failedList;
}

bool DataPaths::tryCreateDirectory(const QDir& rootDir, const QString& dirToCreate) const
{
	if (!rootDir.exists(dirToCreate))
	{
		return rootDir.mkdir(dirToCreate);
	}
	
	return true;
}
		
bool DataPaths::createDirectories()
{
	// This variable should only be changed to false and only if something 
	// fails.
	bool bAllSuccessful = true;

	QDir appDataDir(appDataDirectory());
	
	if (tryCreateDirectory(appDataDir, PROGRAM_APPDATA_DIR_NAME))
	{
		bAllSuccessful = false;
	}

	return bAllSuccessful;
}

QString DataPaths::dataDirectoryPath() const
{
	QString appDataDir = appDataDirectory(PROGRAM_APPDATA_DIR_NAME);
	return appDataDir;
}

QStringList DataPaths::directoriesExist() const
{
	QStringList failedList;
	
	QDir dataDirectory = dataDirectoryPath();
	if (!dataDirectory.exists())
	{
		failedList.append(dataDirectory.absolutePath());
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

bool DataPaths::validateAppDataDirectory()
{
	return validateDir(appDataDirectory());
}

bool DataPaths::validateDir(const QString& path)
{
	QFileInfo fileInfo(path);
	return !path.isEmpty() && fileInfo.exists() 
	&&		fileInfo.isDir() && fileInfo.isWritable();
}

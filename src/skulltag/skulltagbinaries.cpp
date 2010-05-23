//------------------------------------------------------------------------------
// skulltagbinaries.cpp
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
#include "skulltagbinaries.h"
#include "skulltagmain.h"
#include "skulltagserver.h"
#include "main.h"

#include <QMessageBox>

#ifdef Q_OS_WIN32
#define ST_BINARY_NAME "skulltag.exe"
#define SCRIPT_FILE_EXTENSION ".bat"
#else
#define ST_BINARY_NAME "skulltag"
#define SCRIPT_FILE_EXTENSION ".sh"
#endif

SkulltagBinaries::SkulltagBinaries(const SkulltagServer* server)
: server(server)
{
}

QString SkulltagBinaries::clientBinary(QString& error) const
{
	SettingsData* setting;
	if (!server->isTestingServer() || !Main::config->setting("SkulltagEnableTesting")->boolean())
	{
		return Binaries::clientBinary(error);
	}
	else
	{
		// This is common code for both Unix and Windows:
		setting = Main::config->setting("SkulltagTestingPath");
		QString path = setting->string();
		if (path.isEmpty())
		{
			error = tr("No testing directory specified for Skulltag");
			return QString();
		}

		if (path[path.length() - 1] != '/' && path[path.length() - 1] != '\\' )
			path += '/';

		path += server->version();

		QFileInfo fi(path);
		if (!fi.exists())
		{
			error = tr("%1\ndoesn't exist.\nYou need to install new testing binaries.").arg(path);
			QString messageBoxContent = tr("%1\n\nDo you want Doomseeker to create %2 directory and copy all your .ini files from your base directory?\n\nNote: You will still have to manualy install the binaries.").arg(error, server->version());

			if (QMessageBox::question(Main::mainWindow, tr("Doomseeker - missing testing binaries"), messageBoxContent, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			{
				// setting->string() should still contain base dir
				// for testing binaries
				QDir dir(setting->string());
				if (!dir.mkdir(server->version()))
				{
					error = tr("Unable to create directory:\n%1").arg(path);
					return QString();
				}

				// Now copy all .ini's. On Linux .ini's are kept in ~/.skulltag so this will
				// do nothing, but on Windows this should work like magic.
				QDir baseBinaryDir(clientWorkingDirectory(error));
				QStringList nameFilters;
				nameFilters << "*.ini";
				QStringList iniFiles = baseBinaryDir.entryList(nameFilters, QDir::Files);
				foreach(QString str, iniFiles)
				{
					QString sourcePath = baseBinaryDir.canonicalPath() + '/' + str;
					QString targetPath = path + '/' + str;
					QFile file(sourcePath);
					file.copy(targetPath);
				}

				QMessageBox::information(Main::mainWindow, tr("Doomseeker"), tr("Please install now version \"%1\" into:\n%2").arg(server->version(), path));
				error = QString();
			}
			return QString();
		}

		if (!fi.isDir())
		{
			error = tr("%1\nexists but is NOT a directory.\nCannot proceed.").arg(path);
			return QString();
		}

		QString binPath = path + '/' + ST_BINARY_NAME;
		fi = QFileInfo(binPath);
		if (!fi.exists() || (fi.isDir() && !fi.isBundle()))
		{
			error = tr("%1\ndoesn't contain Skulltag executable").arg(path);
			return QString();
		}

		// Everything checked out, so proceed to create (if necessary) and return path to the script file.
		QString retPath;
		if (!spawnTestingBatchFile(path, retPath, error))
		{
			// error is already specified inside spawnTestingBatchFile()S
			return QString();
		}

		return retPath;
	}
}

QString SkulltagBinaries::clientWorkingDirectory(QString& error) const
{
	SettingsData* setting = Main::config->setting("SkulltagBinaryPath");
	QFileInfo fi(setting->string());
	return fi.canonicalPath();
}

QString SkulltagBinaries::configKeyServerBinary() const
{
	#ifdef Q_OS_WIN32
		return configKeyClientBinary();
	#else
		return "SkulltagServerBinaryPath";
	#endif
}

const PluginInfo* SkulltagBinaries::plugin() const
{
	return SkulltagMain::get();
}

bool SkulltagBinaries::spawnTestingBatchFile(const QString& versionDir, QString& fullPathToFile, QString& error) const
{
	QString binaryPath = versionDir + '/' + ST_BINARY_NAME;
	// This will create an actual path to file, because there is no '/' at the end
	// of scriptFilepath.
	fullPathToFile = versionDir + SCRIPT_FILE_EXTENSION;
	QFileInfo fi(fullPathToFile);
	QFile file(fullPathToFile);
	if (fi.isDir())
	{
		error = tr("%1\n should be a script file but is a directory!").arg(fullPathToFile);
		return false;
	}

	if (fi.exists())
	{
		printf("File Permissions: %X\n", (unsigned int)file.permissions());
		if ((file.permissions() & QFile::ExeUser) == 0)
		{
			error = tr("You don't have permissions to execute file: %1\n").arg(fullPathToFile);
			return false;
		}
		return true;
	}

	QString content;
	Binaries* binaries = server->binaries();
	#ifdef Q_OS_WIN32
	// Create Windows batch file
	// Extract drive letter:
	QString driveLetter;
	QString workDir = binaries->clientWorkingDirectory(error);
	for (int i = 0; i < workDir.length(); ++i)
	{
		if (workDir[i] == ':')
		{
			driveLetter = workDir.left(i);
		}
	}

	if (!driveLetter.isEmpty())
	{
		content += driveLetter + ":\r\n";
	}

	QString cdDir = binaries->clientWorkingDirectory(error).replace('/', '\\');
	QString exePath = binaryPath.replace('/', '\\');

	content += "cd \"" + cdDir + "\"\r\n";
	content += "\"" + exePath + "\" %*"; // %* deals with all the parameters
	#else
	QString cdDir = binaries->clientWorkingDirectory(error);

	// Create Unix script file
	content  = "#!/bin/bash\n";
	content += "cd \"" + cdDir + "\" \n";
	content += "export LANG=C\n"; // without this Skulltag won't run on my system (Zalewa)
	content += "\"" + binaryPath + "\" $*"; // $* deals with all the parameters
	#endif
	delete binaries;

	if (!error.isNull())
	{
		error.prepend(tr("Error while creating a shell script: "));
		return false;
	}

	if (!file.open(QIODevice::WriteOnly))
	{
		error = tr("Couldn't open batch file \"%1\" for writing").arg(fullPathToFile);
		return false;
	}

	if (file.write(content.toAscii()) < 0)
	{
		error = tr("Error while writing batch file \"%1\"").arg(fullPathToFile);
		file.close();
		return false;
	}

	file.close();

	if (!file.setPermissions(file.permissions() | QFile::ExeUser))
	{
		error = tr("Cannot set permissions for file:\n%1").arg(fullPathToFile);
		return false;
	}

	return true;
}

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
#include "serverapi/message.h"

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

QString SkulltagBinaries::clientBinary(Message& message) const
{
	IniSection& config = *SkulltagMain::get()->pInterface->pConfig;

	if (!server->isTestingServer() || !config["EnableTesting"])
	{
		return Binaries::clientBinary(message);
	}
	else
	{
		message = Message();
		QString error;

		// This is common code for both Unix and Windows:
		IniVariable &setting = config["TestingPath"];
		QString path = setting;
		if (path.isEmpty())
		{
			error = tr("No testing directory specified for Skulltag");
			message = Message::customError(error);
			return QString();
		}

		if (path[path.length() - 1] != '/' && path[path.length() - 1] != '\\' )
		{
			path += '/';
		}

		path += server->version();

		QFileInfo fi(path);
		if (!fi.exists())
		{
			error = tr("%1\ndoesn't exist.\nYou need to install new testing binaries.").arg(path);
			QString messageBoxContent = tr("%1\n\n\
Do you want Doomseeker to create %2 directory and copy all your .ini files from your base directory?\n\n\
Note: You will still have to manualy install the binaries."
										   ).arg(error, server->version());

			if (QMessageBox::question(Main::mainWindow, tr("Doomseeker - missing testing binaries"), messageBoxContent, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			{
				// setting->string() should still contain base dir
				// for testing binaries
				QDir dir(setting);
				if (!dir.mkdir(server->version()))
				{
					error = tr("Unable to create directory:\n%1").arg(path);
					message = Message::customError(error);
					return QString();
				}

				// Now copy all .ini's. On Linux .ini's are kept in ~/.skulltag so this will
				// do nothing, but on Windows this should work like magic.
				QDir baseBinaryDir(clientWorkingDirectory(message));
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

				// Show user the prompt to install the binaries.
				QMessageBox::information(Main::mainWindow, tr("Doomseeker"), tr("Please install now version \"%1\" into:\n%2").arg(server->version(), path));

				// Try this method again.
				return clientBinary(message);
			}
			return QString();
		}

		if (!fi.isDir())
		{
			error = tr("%1\nexists but is NOT a directory.\nCannot proceed.").arg(path);
			message = Message::customError(error);
			return QString();
		}

		QString binPath = path + '/' + ST_BINARY_NAME;
		fi = QFileInfo(binPath);
		if (!fi.exists() || (fi.isDir() && !fi.isBundle()))
		{
			error = tr("%1\ndoesn't contain Skulltag executable").arg(path);
			message = Message::customError(error);
			return QString();
		}

		// Everything checked out, so proceed to create (if necessary) and return path to the script file.
		QString retPath;
		if (!spawnTestingBatchFile(path, retPath, message))
		{
			// message is already specified inside spawnTestingBatchFile()S
			return QString();
		}

		return retPath;
	}
}

QString SkulltagBinaries::clientWorkingDirectory(Message& message) const
{
	IniSection& config = *SkulltagMain::get()->pInterface->pConfig;

	QFileInfo fi(config["BinaryPath"]);
	return fi.canonicalPath();
}

QString SkulltagBinaries::configKeyServerBinary() const
{
	#ifdef Q_OS_WIN32
		return configKeyClientBinary();
	#else
		return "ServerBinaryPath";
	#endif
}

const PluginInfo* SkulltagBinaries::plugin() const
{
	return SkulltagMain::get();
}

bool SkulltagBinaries::spawnTestingBatchFile(const QString& versionDir, QString& fullPathToFile, Message& message) const
{
	QString binaryPath = versionDir + '/' + ST_BINARY_NAME;
	// This will create an actual path to file, because there is no '/' at the end
	// of scriptFilepath.
	fullPathToFile = versionDir + SCRIPT_FILE_EXTENSION;
	QFileInfo fi(fullPathToFile);
	QFile file(fullPathToFile);
	if (fi.isDir())
	{
		QString error = tr("%1\n should be a script file but is a directory!").arg(fullPathToFile);
		message = Message::customError(error);
		return false;
	}

	if (fi.exists())
	{
		printf("File Permissions: %X\n", (unsigned int)file.permissions());
		if ((file.permissions() & QFile::ExeUser) == 0)
		{
			QString error = tr("You don't have permissions to execute file: %1\n").arg(fullPathToFile);
			message = Message::customError(error);
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
	QString workDir = binaries->clientWorkingDirectory(message);
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

	QString cdDir = binaries->clientWorkingDirectory(message).replace('/', '\\');
	QString exePath = binaryPath.replace('/', '\\');

	content += "cd \"" + cdDir + "\"\r\n";
	content += "\"" + exePath + "\" %*"; // %* deals with all the parameters
	#else
	QString cdDir = binaries->clientWorkingDirectory(message);

	// Create Unix script file
	content  = "#!/bin/bash\n";
	content += "cd \"" + cdDir + "\" \n";
	content += "export LANG=C\n"; // without this Skulltag won't run on my system (Zalewa)
	content += "\"" + binaryPath + "\" $*"; // $* deals with all the parameters
	#endif
	delete binaries;

	if (!file.open(QIODevice::WriteOnly))
	{
		QString error = tr("Couldn't open batch file \"%1\" for writing").arg(fullPathToFile);
		message = Message::customError(error);
		return false;
	}

	if (file.write(content.toAscii()) < 0)
	{
		QString error = tr("Error while writing batch file \"%1\"").arg(fullPathToFile);
		message = Message::customError(error);
		file.close();
		return false;
	}

	file.close();

	if (!file.setPermissions(file.permissions() | QFile::ExeUser))
	{
		QString error = tr("Cannot set permissions for file:\n%1").arg(fullPathToFile);
		message = Message::customError(error);
		return false;
	}

	return true;
}

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
SkulltagBinaries::SkulltagBinaries(SkulltagServer* server)
: Binaries(server)
{
}

QString SkulltagServer::configKeyServerBinary() const
{ 
	#ifdef Q_OS_WIN32
		return configKeyClientBinary();
	#else
		return "SkulltagServerBinaryPath"; 
	#endif
}

QString SkulltagServer::clientBinary(QString& error) const
{
	SettingsData* setting;
	if (!this->testingServer || !Main::config->setting("SkulltagEnableTesting")->boolean())
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

		path += version();

		QFileInfo fi(path);
		if (!fi.exists())
		{
			error = tr("%1\ndoesn't exist.\nYou need to install new testing binaries.").arg(path);
			QString messageBoxContent = tr("%1\n\nDo you want Doomseeker to create %2 directory and copy all your .ini files from your base directory?\n\nNote: You will still have to manualy install the binaries.").arg(error, version());

			if (QMessageBox::question(Main::mainWindow, tr("Doomseeker - missing testing binaries"), messageBoxContent, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			{
				// setting->string() should still contain base dir
				// for testing binaries
				QDir dir(setting->string());
				if (!dir.mkdir(version()))
				{
					error = tr("Unable to create directory:\n%1").arg(path);
					return QString();
				}

				// Now copy all .ini's. On Linux .ini's are kept in ~/.skulltag so this will
				// do nothing, but on Windows this should work like magic.
				QDir baseBinaryDir(clientWorkingDirectory());
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

				QMessageBox::information(Main::mainWindow, tr("Doomseeker"), tr("Please install now version \"%1\" into:\n%2").arg(version(), path));
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

QString SkulltagServer::clientWorkingDirectory() const
{
	SettingsData* setting = Main::config->setting("SkulltagBinaryPath");
	QFileInfo fi(setting->string());
	return fi.canonicalPath();
}

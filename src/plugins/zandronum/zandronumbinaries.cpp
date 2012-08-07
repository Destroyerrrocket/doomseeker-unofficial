//------------------------------------------------------------------------------
// zandronumbinaries.cpp
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "log.h"
#include "zandronumbinaries.h"
#include "zandronumengineplugin.h"
#include "zandronumserver.h"
#include "main.h"
#include "serverapi/message.h"
#include "../wadseeker/zip/unarchive.h"
#include "plugins/engineplugin.h"
#include "version.h"

#include <QBuffer>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressDialog>

#define TESTING_BINARY_URL_BASE "http://zandronum.com/downloads/testing/%1/ZandroDev%1-%2"

#ifdef Q_OS_WIN32
#define TESTING_BINARY_URL TESTING_BINARY_URL_BASE"windows.zip"
#define ZANDRONUM_BINARY_NAME "zandronum.exe"
#define SCRIPT_FILE_EXTENSION ".bat"
#else

#ifndef __x86_64__
#define TESTING_BINARY_URL TESTING_BINARY_URL_BASE"linux-x86.tar.bz2"
#else
#define TESTING_BINARY_URL TESTING_BINARY_URL_BASE"linux-x86_64.tar.bz2"
#endif

#define ZANDRONUM_BINARY_NAME "zandronum"
#define SCRIPT_FILE_EXTENSION ".sh"
#endif

ZandronumBinaries::ZandronumBinaries(const ZandronumServer* server)
: Binaries(ZandronumEnginePlugin::staticInstance()), server(server)
{
}

QString ZandronumBinaries::clientBinary(Message& message) const
{
	IniSection& config = *ZandronumEnginePlugin::staticInstance()->data()->pConfig;

	if (!server->isTestingServer() || !config["EnableTesting"])
	{
		return Binaries::clientBinary(message);
	}
	else
	{
		message = Message();
		QString error;

		// This is common code for both Unix and Windows:
		IniVariable setting = config["TestingPath"];
		QString path = setting;
		if (path.isEmpty())
		{
			error = tr("No testing directory specified for Zandronum");
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
Do you want Doomseeker to create %2 directory and copy all your .ini files from your base directory?"
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

				// Now copy all .ini's. On Linux .ini's are kept in ~/.zandronum so this will
				// do nothing, but on Windows this should work like magic.
				QDir baseBinaryDir(clientWorkingDirectory(message));
				QStringList nameFilters;
				nameFilters << "*.ini";
				QStringList iniFiles = baseBinaryDir.entryList(nameFilters, QDir::Files);
				foreach(QString str, iniFiles)
				{
					QString sourcePath = baseBinaryDir.absolutePath() + '/' + str;
					QString targetPath = path + '/' + str;
					QFile file(sourcePath);
					file.copy(targetPath);
				}

				if(!downloadTestingBinaries(path))
				{
					// Show user the prompt to install the binaries.
					QMessageBox::information(Main::mainWindow, tr("Doomseeker"), tr("Please install now version \"%1\" into:\n%2").arg(server->version(), path));
				}

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

		QString binPath = path + '/' + ZANDRONUM_BINARY_NAME;
		fi = QFileInfo(binPath);
		if (!fi.exists() || (fi.isDir() && !fi.isBundle()))
		{
			error = tr("%1\ndoesn't contain Zandronum executable").arg(path);
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

QString ZandronumBinaries::clientWorkingDirectory(Message& message) const
{
	IniSection& config = *ZandronumEnginePlugin::staticInstance()->data()->pConfig;

	QFileInfo fi((QString)config["BinaryPath"]);
	return fi.absolutePath();
}

QString ZandronumBinaries::configKeyServerBinary() const
{
	#ifdef Q_OS_WIN32
		return configKeyClientBinary();
	#else
		return "ServerBinaryPath";
	#endif
}

bool ZandronumBinaries::downloadTestingBinaries(const QDir &destination) const
{
#ifdef Q_OS_MAC
	// Can't do anything for Mac OS X at this time. :/
	return false;
#else
	// Download testing binaries
	ZandronumVersion version(server->version());

	// Find the hg revision string
	QString hgVersion;
	if(version.hgVersionDate() < 4000) // Arbitrary number here to determine between old and new format
		hgVersion = QString("%1").arg(version.hgVersionDate());
	else
		hgVersion = QString("%1-%2").arg(version.hgVersionDate()).arg(version.hgVersionTime(), 4, 10, QChar('0'));

	// Get URL
	QString versionPrefix;
	if (version.revisionLetter() != 0)
	{
		versionPrefix = QString("%1.%2%3").arg(version.majorVersion())
			.arg(version.minorVersion()).arg(QChar(version.revisionLetter()));
	}
	else
	{
		versionPrefix = QString("%1.%2").arg(version.majorVersion())
			.arg(version.minorVersion());
	}

	QUrl url(QString(TESTING_BINARY_URL).arg(versionPrefix).arg(hgVersion));
	TestingProgressDialog dialog(url);
	if(dialog.exec() == QDialog::Accepted)
	{
		// Extract the needed files.
		QString filename;
		QFileInfo fi(dialog.filename());
		QByteArray data = dialog.data();
		QBuffer dataBuffer(&data);
		UnArchive *archive = UnArchive::openArchive(fi, &dataBuffer);

		if(archive != NULL)
		{
			for(int i = 0;;++i)
			{
				filename = archive->fileNameFromIndex(i);

				if (filename.isNull())
				{
					break;
				}

				archive->extract(i, destination.path() + QDir::separator() + filename);
				// Make sure we can execute the binary.
				if(filename == ZANDRONUM_BINARY_NAME)
				{
					QFile binaryFile(destination.path() + QDir::separator() + filename);
					binaryFile.setPermissions(binaryFile.permissions() | QFile::ExeUser);
				}
			}
			delete archive;
			return true;
		}
	}

	return false;
#endif
}

const EnginePlugin* ZandronumBinaries::plugin() const
{
	return ZandronumEnginePlugin::staticInstance();
}

bool ZandronumBinaries::spawnTestingBatchFile(const QString& versionDir, QString& fullPathToFile, Message& message) const
{
	QString binaryPath = versionDir + '/' + ZANDRONUM_BINARY_NAME;
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
		gLog << tr("File Permissions: %1").arg((unsigned int)file.permissions(), 0, 16);
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
	content += "export LANG=C\n"; 
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

////////////////////////////////////////////////////////////////////////////////

TestingProgressDialog::TestingProgressDialog(const QUrl& url)
: QProgressDialog(tr("Downloading testing binaries..."), tr("Cancel"), 0, 0, Main::mainWindow)
{
	connect(this, SIGNAL(canceled()), this, SLOT(abort()));

	getUrl(url);

	setAutoClose(false);
	setAutoReset(false);
	setMinimumDuration(0);
}

void TestingProgressDialog::abort()
{
	pNetworkReply->abort();
}

void TestingProgressDialog::downloadProgress(qint64 value, qint64 max)
{
	setValue(value);
	setMaximum(max);
}

void TestingProgressDialog::downloadFinished()
{
	QUrl possibleRedirectUrl = pNetworkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	QUrl url = pNetworkReply->request().url();
	if (!possibleRedirectUrl.isEmpty()
		&& possibleRedirectUrl != url)
	{
		// Redirect.
		if (possibleRedirectUrl.isRelative())
		{
			possibleRedirectUrl = url.resolved(possibleRedirectUrl);
		}

		pNetworkReply->deleteLater();
		getUrl(possibleRedirectUrl);
	}
	else
	{
		QUrl url = pNetworkReply->request().url();
		QFileInfo fi(url.path());
		QString filename = fi.fileName();

		downloadedFilename = filename;
		downloadedFileData = pNetworkReply->readAll();

		accept();
	}
}

void TestingProgressDialog::getUrl(const QUrl& url)
{
	QNetworkRequest request;
	request.setUrl(url);
	request.setRawHeader("User-Agent", Version::userAgent().toAscii());

	this->pNetworkReply = networkAccessManager.get(request);

	connect(pNetworkReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
	connect(pNetworkReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
}


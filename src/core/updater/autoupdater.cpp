//------------------------------------------------------------------------------
// autoupdater.cpp
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
#include "autoupdater.h"

#include "configuration/doomseekerconfig.h"
#include "updater/updatechannel.h"
#include "updater/updatepackagefilter.h"
#include "updater/updaterinfoparser.h"
#include "datapaths.h"
#include "log.h"
#include "main.h"
#include "strings.h"
#include "version.h"
#include <wadseeker/protocols/fixednetworkaccessmanager.h>
#include <QByteArray>
#include <QDebug>
#include <QNetworkRequest>
#include <QTemporaryFile>
#include <cassert>

class AutoUpdater::PrivData
{
	public:
		bool bDownloadAndInstallRequireConfirmation;
		bool bIsRunning;
		bool bStarted;
		UpdateChannel channel;
		UpdatePackage currentlyDownloadedPackage;
		QStringList downloadedPackagesFilenames;
		ErrorCode errorCode;
		QMap<QString, QList<unsigned long long> > ignoredPackagesRevisions;
		QList<UpdatePackage> newUpdatePackages;
		QList<UpdatePackage> packagesInDownloadQueue;
		QTemporaryFile* pCurrentPackageFile;
		FixedNetworkAccessManager* pNam;
		QNetworkReply* pNetworkReply;
};
//////////////////////////////////////////////////////////////////////////////

const QString AutoUpdater::PLUGIN_PREFIX = "p-";
const QString AutoUpdater::MAIN_PROGRAM_PACKAGE_NAME = "doomseeker";
// This can be set to different values depending on target platform.
const QString AutoUpdater::UPDATER_INFO_URL = "http://doomseeker.drdteam.org/updates/update-info_win32.js";

AutoUpdater::AutoUpdater(QObject* pParent)
: QObject(pParent)
{
	d = new PrivData();
	d->bDownloadAndInstallRequireConfirmation = false;
	d->bIsRunning = false;
	d->bStarted = false;
	d->errorCode = EC_Ok;
	d->pCurrentPackageFile = NULL;
	d->pNam = new FixedNetworkAccessManager();
	d->pNetworkReply = NULL;
}

AutoUpdater::~AutoUpdater()
{
	if (d->pCurrentPackageFile != NULL)
	{
		delete d->pCurrentPackageFile;
	}
	if (d->pNetworkReply != NULL)
	{
		d->pNetworkReply->disconnect();
		d->pNetworkReply->abort();
		d->pNetworkReply->deleteLater();
	}
	d->pNam->disconnect();
	d->pNam->deleteLater();
	delete d;
}

void AutoUpdater::abort()
{
	if (d->pNetworkReply != NULL)
	{
		d->pNetworkReply->disconnect();
		d->pNetworkReply->abort();
		d->pNetworkReply->deleteLater();
		d->pNetworkReply = NULL;
	}
	emit finishWithError(EC_Aborted);
}

const UpdateChannel& AutoUpdater::channel() const
{
	return d->channel;
}

void AutoUpdater::confirmDownloadAndInstall()
{
	d->packagesInDownloadQueue = d->newUpdatePackages;
	startNextPackageDownload();
}

const QStringList& AutoUpdater::downloadedPackagesFilenames() const
{
	return d->downloadedPackagesFilenames;
}

void AutoUpdater::dumpUpdatePackagesToLog(const QList<UpdatePackage>& packages)
{
	foreach (const UpdatePackage& pkg, packages)
	{
		gLog << tr("Detected update for package \"%1\" from version \"%2\" to version \"%3\".")
			.arg(pkg.displayName, pkg.currentlyInstalledDisplayVersion, pkg.displayVersion);
	}
}

AutoUpdater::ErrorCode AutoUpdater::errorCode() const
{
	return d->errorCode;
}

QString AutoUpdater::errorCodeToString(ErrorCode code)
{
	switch (code)
	{
		case EC_Ok:
			return tr("Ok");
		case EC_Aborted:
			return tr("Update was aborted.");
		case EC_NullUpdateChannel:
			return tr("No valid update channel was specified. Please check your configuration.");
		case EC_UpdaterInfoDownloadProblem:
			return tr("Failed to download updater info file.");
		case EC_UpdaterInfoCannotParse:
			return tr("Cannot parse updater info file.");
		case EC_UpdaterInfoMissingMainProgramNode:
			return tr("Main program node is missing from updater info file.");
		case EC_MissingRevisionInfo:
			return tr("Revision info on one of the packages is missing from the "
				"updater info file. Check the log for details.");
		case EC_MissingDownloadUrl:
			return tr("Download URL for one of the packages is missing from the "
				"updater info file. Check the log for details.");
		case EC_InvalidDownloadUrl:
			return tr("Download URL for one of the packages is invalid. "
				"Check the log for details.");
		case EC_PackageDownloadProblem:
			return tr("Update package download failed. Check the log for details.");
		case EC_StorageDirCreateFailure:
			return tr("Failed to create directory for updates packages storage.");
		case EC_PackageCantBeSaved:
			return tr("Failed to create package file for saving.");
		default:
			return tr("Unknown error.");
	}
}

QString AutoUpdater::errorString() const
{
	return errorCodeToString(errorCode());
}

void AutoUpdater::finishWithError(ErrorCode code)
{
	d->bIsRunning = false;
	d->errorCode = code;
	emit finished();
}

bool AutoUpdater::isRunning() const
{
	return d->bIsRunning;
}

QNetworkReply::NetworkError AutoUpdater::lastNetworkError() const
{
	if (errorCode() != EC_Ok && d->pNetworkReply != NULL)
	{
		return d->pNetworkReply->error();
	}
	return QNetworkReply::NoError;
}

const QList<UpdatePackage>& AutoUpdater::newUpdatePackages() const
{
	return d->newUpdatePackages;
}

void AutoUpdater::onPackageDownloadFinish()
{
	if (d->pNetworkReply->error() == QNetworkReply::NoError)
	{
		gLog << tr("Finished downloading package \"%1\".")
			.arg(d->currentlyDownloadedPackage.displayName);
		startPackageScriptDownload(d->currentlyDownloadedPackage);
	}
	else
	{
		gLog << tr("Network error when downloading package \"%1\": [%2] %3")
			.arg(d->currentlyDownloadedPackage.displayName)
			.arg(d->pNetworkReply->error())
			.arg(d->pNetworkReply->errorString());
		finishWithError(EC_PackageDownloadProblem);
	}
}

void AutoUpdater::onPackageDownloadReadyRead()
{
	const int MAX_CHUNK_SIZE = 2 * 1024 * 1024; // 2MB
	QByteArray data = d->pNetworkReply->read(MAX_CHUNK_SIZE);
	while (!data.isEmpty())
	{
		d->pCurrentPackageFile->write(data);
		data = d->pNetworkReply->read(MAX_CHUNK_SIZE);
	}
}

void AutoUpdater::onPackageScriptDownloadFinish()
{
	if (d->pNetworkReply->error() == QNetworkReply::NoError)
	{
		gLog << tr("Finished downloading package script \"%1\".")
			.arg(d->currentlyDownloadedPackage.displayName);
		QString savePath = d->pCurrentPackageFile->fileName() + ".xml";
		QFile file(savePath);
		if (!file.open(QIODevice::WriteOnly))
		{
			gLog << tr("Failed to save package script to file: %1").arg(savePath);
			finishWithError(EC_PackageCantBeSaved);
			return;
		}
		file.write(d->pNetworkReply->readAll());
		file.close();

		if (!d->packagesInDownloadQueue.isEmpty())
		{
			startNextPackageDownload();
		}
		else
		{
			gLog << tr("All packages downloaded.");
			finishWithError(EC_Ok);
		}
	}
	else
	{
		gLog << tr("Network error when downloading package script \"%1\": [%2] %3")
			.arg(d->currentlyDownloadedPackage.displayName)
			.arg(d->pNetworkReply->error())
			.arg(d->pNetworkReply->errorString());
		finishWithError(EC_PackageDownloadProblem);
	}
}

void AutoUpdater::onUpdaterInfoDownloadFinish()
{
	if (d->pNetworkReply->error() != QNetworkReply::NoError)
	{
		finishWithError(EC_UpdaterInfoDownloadProblem);
		return;
	}
	QByteArray json = d->pNetworkReply->readAll();
	UpdaterInfoParser parser;
	ErrorCode parseResult = (ErrorCode) parser.parse(json);
	if (parseResult == EC_Ok)
	{
		UpdatePackageFilter filter;
		filter.setChannel(d->channel);
		filter.setIgnoreRevisions(d->ignoredPackagesRevisions);
		QList<UpdatePackage> packagesList = filter.filter(parser.packages());
		if (!packagesList.isEmpty())
		{
			dumpUpdatePackagesToLog(packagesList);
			d->newUpdatePackages = packagesList;
			if (d->bDownloadAndInstallRequireConfirmation)
			{
				emit downloadAndInstallConfirmationRequested();
			}
			else
			{
				confirmDownloadAndInstall();
			}
		}
		else
		{
			// Nothing to update.
			finishWithError(EC_Ok);
		}
	}
	else
	{
		finishWithError(parseResult);
	}
}

bool AutoUpdater::preparePackagesTempDirectory()
{
	return true;
}

void AutoUpdater::setChannel(const UpdateChannel& updateChannel)
{
	d->channel = updateChannel;
}

void AutoUpdater::setIgnoreRevisions(const QMap<QString, QList<unsigned long long> >& packagesRevisions)
{
	d->ignoredPackagesRevisions = packagesRevisions;
}

void AutoUpdater::setRequireDownloadAndInstallConfirmation(bool b)
{
	d->bDownloadAndInstallRequireConfirmation = b;
}

void AutoUpdater::start()
{
	if (d->bStarted)
	{
		qDebug() << "Cannot start AutoUpdater more than once.";
		// Always cause assertion failure. Program shouldn't
		// go into this state.
		assert(false && "Cannot start AutoUpdater more than once.");
		return;
	}
	d->bStarted = true;
	if (d->channel.isNull())
	{
		finishWithError(EC_NullUpdateChannel);
		return;
	}
	QDir storageDir(updateStorageDirPath());
	if (!storageDir.mkpath("."))
	{
		gLog << tr("Failed to create directory for updates storage: %1")
			.arg(storageDir.path());
		finishWithError(EC_StorageDirCreateFailure);
	}
	d->bIsRunning = true;
	QNetworkRequest request;
	request.setRawHeader("User-Agent", Version::userAgent().toAscii());
	request.setUrl(UPDATER_INFO_URL);
	QNetworkReply* pReply = d->pNam->get(request);
	// The updater info file should always be very small and
	// we can safely store it all in memory.
	this->connect(pReply,
		SIGNAL(finished()),
		SLOT(onUpdaterInfoDownloadFinish()));
	d->pNetworkReply = pReply;
}

void AutoUpdater::startNextPackageDownload()
{
	assert(!d->packagesInDownloadQueue.isEmpty() && "AutoUpdater::startNextPackageDownload()");
	UpdatePackage pkg = d->packagesInDownloadQueue.takeFirst();
	startPackageDownload(pkg);
}

void AutoUpdater::startPackageDownload(const UpdatePackage& pkg)
{
	QUrl url = pkg.downloadUrl;
	if (!url.isValid() || url.isRelative())
	{
		// Parser already performs a check for this but let's do this
		// again to make sure nothing got lost on the way.
		gLog << tr("Invalid download URL for package \"%1\": %2")
			.arg(pkg.displayName, pkg.downloadUrl.toString());
		finishWithError(EC_InvalidDownloadUrl);
		return;
	}
	gLog << tr("Downloading package \"%1\" from URL: %2.").arg(pkg.displayName,
		pkg.downloadUrl.toString());

	QString fileNameTemplate = QString("%1-XXXXXX.zip").arg(pkg.name);
	QString filePathTemplate = Strings::combinePaths(updateStorageDirPath(), fileNameTemplate);
	qDebug() << "filePathTemplate: " << filePathTemplate;
	if (d->pCurrentPackageFile != NULL)
	{
		delete d->pCurrentPackageFile;
	}
	d->pCurrentPackageFile = new QTemporaryFile(filePathTemplate);
	d->pCurrentPackageFile->setAutoRemove(false);
	if (!d->pCurrentPackageFile->open())
	{
		gLog << tr("Couldn't save file in path: %1").arg(updateStorageDirPath());
		delete d->pCurrentPackageFile;
		d->pCurrentPackageFile = NULL;
		finishWithError(EC_PackageCantBeSaved);
		return;
	}
	QFileInfo fileInfo(d->pCurrentPackageFile->fileName());
	d->downloadedPackagesFilenames << fileInfo.fileName();

	QNetworkRequest request;
	request.setRawHeader("User-Agent", Version::userAgent().toAscii());
	request.setUrl(url);
	QNetworkReply* pReply = d->pNam->get(request);
	d->currentlyDownloadedPackage = pkg;
	d->pNetworkReply = pReply;
	this->connect(pReply, SIGNAL(readyRead()),
		SLOT(onPackageDownloadReadyRead()));
	this->connect(pReply, SIGNAL(finished()),
		SLOT(onPackageDownloadFinish()));
}

void AutoUpdater::startPackageScriptDownload(const UpdatePackage& pkg)
{
	QUrl url = pkg.downloadScriptUrl;
	if (!url.isValid() || url.isRelative())
	{
		// Parser already performs a check for this but let's do this
		// again to make sure nothing got lost on the way.
		gLog << tr("Invalid download URL for package script \"%1\": %2")
			.arg(pkg.displayName, pkg.downloadScriptUrl.toString());
		finishWithError(EC_InvalidDownloadUrl);
		return;
	}
	gLog << tr("Downloading package script \"%1\" from URL: %2.").arg(pkg.displayName,
		pkg.downloadScriptUrl.toString());

	QNetworkRequest request;
	request.setRawHeader("User-Agent", Version::userAgent().toAscii());
	request.setUrl(url);
	QNetworkReply* pReply = d->pNam->get(request);
	d->currentlyDownloadedPackage = pkg;
	d->pNetworkReply = pReply;
	// Scripts are small enough that they can be downloaded "in one take",
	// without saving them continuously to a file.
	this->connect(pReply, SIGNAL(finished()),
		SLOT(onPackageScriptDownloadFinish()));
}

QString AutoUpdater::updateStorageDirPath()
{
	return Main::dataPaths->localDataLocationPath(DataPaths::UPDATE_PACKAGES_DIR_NAME);
}

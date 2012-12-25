//------------------------------------------------------------------------------
// updateinstaller.cpp
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
#include "updateinstaller.h"

#include "configuration/doomseekerconfig.h"
#include "log.h"
#include "main.h"
#include "strings.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>

const QString UPDATER_EXECUTABLE_FILENAME = "updater.exe";

UpdateInstaller::UpdateInstaller(QObject* pParent)
: QObject(pParent)
{
}

UpdateInstaller::~UpdateInstaller()
{
}

QString UpdateInstaller::errorCodeToStr(ErrorCode code)
{
	switch (code)
	{
		case EC_Ok:
			return tr("Ok");
		case EC_NothingToUpdate:
			return tr("Nothing to update.");
		case EC_UpdatePackageMissing:
			return tr("Update package or script are not found. Check log for details.");
		case EC_ProcessStartFailure:
			return tr("Failed to start updater process.");
		default:
			return tr("Unknown error: %1.").arg(code);
	}
}

QString UpdateInstaller::getPercentEncodedCurrentProcessArgs()
{
	QStringList argsEncoded;
	if (Main::bPortableMode)
	{
		argsEncoded << QUrl::toPercentEncoding("--portable");
	}
	if (!Main::argDataDir.isEmpty())
	{
		argsEncoded << QUrl::toPercentEncoding("--datadir");
		argsEncoded << QUrl::toPercentEncoding(Main::argDataDir);
	}
	return argsEncoded.join(" ");
}

bool UpdateInstaller::isAnythingToInstall() const
{
	return !gConfig.autoUpdates.updatePackagesFilenamesForInstallation.isEmpty();
}

QString UpdateInstaller::processErrorCodeToStr(ProcessErrorCode code)
{
	switch (code)
	{
		case PEC_Ok:
			return tr("Ok");
		case PEC_UnableToReadUpdateScript:
			return tr("Unable to read update script.");
		case PEC_NoInstallationDirectorySpecified:
			return tr("No installation directory specified.");
		case PEC_UnableToDeterminePathOfUpdater:
			return tr("Unable to determine path of updater.");
		case PEC_GeneralFailure:
			return tr("General failure.");
		default:
			return tr("Unknown process error code: %1.").arg(code);
	}
}

UpdateInstaller::ErrorCode UpdateInstaller::startInstallation()
{
	QStringList& packages = gConfig.autoUpdates.updatePackagesFilenamesForInstallation;
	if (!packages.isEmpty())
	{
		QString packageName = packages.first();
		gLog << tr("Installing update package \"%1\".").arg(packageName);
		QString packagesDir = Main::dataPaths->localDataLocationPath(DataPaths::UPDATE_PACKAGES_DIR_NAME);
		QString packagePath = Strings::combinePaths(packagesDir, packageName);
		QString scriptPath = packagePath + ".xml";
		QFile filePackage(packagePath);
		QFile fileScript(scriptPath);
		// Do some "does file exist" validation. This isn't 100% reliable but
		// should work correctly in most cases.
		bool isPackageOk = filePackage.exists();
		bool isScriptOk = fileScript.exists();
		if (isPackageOk && isScriptOk)
		{
			if (startUpdaterProcess(packagesDir, scriptPath))
			{
				// Everything was successful.
				// Remove the package from the update list.
				packages.takeFirst();
				gConfig.saveToFile();
			}
			else
			{
				return EC_ProcessStartFailure;
			}
		}
		else if (!isPackageOk)
		{
			gLog << tr("Cannot read update package \"%1\".").arg(packagePath);
			return EC_UpdatePackageMissing;
		}
		else
		{
			gLog << tr("Cannot read update script \"%1\".").arg(packagePath + ".xml");
			return EC_UpdatePackageMissing;
		}
	}
	else
	{
		return EC_NothingToUpdate;
	}
	return EC_Ok;
}

bool UpdateInstaller::startUpdaterProcess(const QString& packagesDir,
	const QString& scriptFilePath)
{
	QString updaterProgramPath = Strings::combinePaths(
		QCoreApplication::applicationDirPath(),
		UPDATER_EXECUTABLE_FILENAME);
	QFile updaterProgramFile(updaterProgramPath);
	QFileInfo programFileInfo(QCoreApplication::applicationFilePath());
	QStringList args;
	args << "--install-dir" << QCoreApplication::applicationDirPath();
	args << "--package-dir" << packagesDir;
	args << "--script" << scriptFilePath;
	args << "--exec" << QDir::toNativeSeparators(programFileInfo.absoluteFilePath());
	QString currentProcessArgs = getPercentEncodedCurrentProcessArgs();
	if (!currentProcessArgs.isEmpty())
	{
		args << "--args" << currentProcessArgs;
	}
	bool bStarted = QProcess::startDetached(updaterProgramPath, args);
	if (!bStarted)
	{
		gLog << tr("Failed to start updater process: %1 %2")
			.arg(updaterProgramPath).arg(args.join(" "));
	}
	return bStarted;
}


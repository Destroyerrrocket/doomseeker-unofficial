//------------------------------------------------------------------------------
// updateinstaller.h
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
#ifndef DOOMSEEKER_UPDATER_UPDATEINSTALLER_H
#define DOOMSEEKER_UPDATER_UPDATEINSTALLER_H

#include <QObject>
#include <QString>

class UpdateInstaller : public QObject
{
	Q_OBJECT

	public:
		/**
		 * @brief "--update-failed" values.
		 */
		enum ProcessErrorCode
		{
			PEC_Ok,
			PEC_UnableToReadUpdateScript = 1,
			PEC_NoInstallationDirectorySpecified = 2,
			PEC_UnableToDeterminePathOfUpdater = 3,
			PEC_GeneralFailure = 10000
		};

		enum ErrorCode
		{
			/// Update started properly.
			EC_Ok,
			/// No update needed.
			EC_NothingToUpdate,
			EC_UpdatePackageMissing,
			EC_ProcessStartFailure
		};

		static QString errorCodeToStr(ErrorCode code);
		static QString processErrorCodeToStr(ProcessErrorCode code);

		UpdateInstaller(QObject* pParent = NULL);
		~UpdateInstaller();

		bool isAnythingToInstall() const;
		/**
		 * @brief Starts update process.
		 *
		 * Reads config to determine which update is next in queue.
		 * Performs checks to see if update can be performed and launches
		 * the updater process.
		 *
		 * - If EC_Ok is returned then program should shutdown itself.
		 * - If EC_NothingToUpdate was returned then program should continue
		 *   normally.
		 * - If anything else is returned then program should notify
		 *   user of error.
		 *
		 * @return EC_Ok if update process was successfully started.
		 *         EC_NothingToUpdate is returned if no update packages
		 *         were detected. Everything else is an error.
		 */
		ErrorCode startInstallation();

	private:
		QString getPercentEncodedCurrentProcessArgs();
		bool startUpdaterProcess(const QString& packagesDir, const QString& scriptFilePath);
};

#endif

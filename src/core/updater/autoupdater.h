//------------------------------------------------------------------------------
// autoupdater.h
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
#ifndef DOOMSEEKER_UPDATER_AUTOUPDATER_H
#define DOOMSEEKER_UPDATER_AUTOUPDATER_H

#include <QNetworkReply>
#include <QObject>
#include <QString>

class UpdateChannel;
class UpdatePackage;

/**
 * @brief Deals with program updates/upgrades.
 *
 * AutoUpdater is a non-reusable state machine. This means that if start()
 * was called once then subsequent calls to start() are an error even if
 * the machine has completed its previous job. If updater needs to be 
 * launched multiple times then the old object needs to be disposed of
 * and a new object needs to be created.
 *
 * The current implementation is coupled with Doomseeker architecture.
 * The updater will extract information on plugins and Doomseeker version
 * and compare it to the version information included in the updater info
 * file.
 *
 * If versions for selected channels do not match then the actual installation
 * package is downloaded and stored in a space that is persistent between
 * OS reboots. A configuration setting is marked to install the update on next
 * program start.
 *
 * When AutoUpdater finishes its job it emits the finished() signal.
 * errorCode() must be called to check if operation was successful or
 * if the updater stopped on an error. lastNetworkError() may give
 * more insight if updater stopped on a remote error.
 */
class AutoUpdater : public QObject
{
	Q_OBJECT

	public:
		enum ErrorCode
		{
			EC_Ok = 0,
			/**
			 * @brief Update was aborted by the user or by the program.
			 */
			EC_Aborted,
			/**
			 * @brief No valid UpdateChannel was specified.
			 */
			EC_NullUpdateChannel,
			/**
			 * @brief Network error when downloading updater info file.
			 */
			EC_UpdaterInfoDownloadProblem,
			/**
			 * @brief Updater info file can't be parsed.
			 */
			EC_UpdaterInfoCannotParse,
			/**
			 * @brief File was parseable but there was no main program 
			 *        information inside.
			 */
			EC_UpdaterInfoMissingMainProgramNode,
			/**
			 * @brief One of packages has no revision info.
			 */
			EC_MissingRevisionInfo,
			/**
			 * @brief One of packages has no download URL.
			 */
			EC_MissingDownloadUrl
		};

		/**
		 * @brief Prefix for all plugins packages names.
		 */
		static const QString PLUGIN_PREFIX;
		/**
		 * @brief Package name for the main program.
		 *
		 * Other packages are plugins.
		 */
		static const QString MAIN_PROGRAM_PACKAGE_NAME;
		/**
		 * @brief URL to the JSON file which contains information on released
		 *        builds.
		 */
		static const QString UPDATER_INFO_URL;

		static QString errorCodeToString(ErrorCode code);

		AutoUpdater(QObject* pParent = NULL);
		~AutoUpdater();

		void abort();
		/**
		 * @brief setChannel() .
		 */
		const UpdateChannel& channel() const;
		ErrorCode errorCode() const;
		QString errorString() const;

		bool isRunning() const;

		/**
		 * @brief The network error that caused the updater to fail.
		 *
		 * This returns a valid value only if errorCode() is not equal
		 * to EC_Ok and if the failure was in fact caused by the network
		 * error.
		 */
		QNetworkReply::NetworkError lastNetworkError() const;
		/**
		 * @brief List of new update packages to install.
		 *
		 * When downloadAndInstallConfirmationRequested() signal is emitted
		 * this accessor can be used to obtain the list of updates
		 * that will be installed if user confirms the install.
		 */
		const QList<UpdatePackage>& newUpdatePackages() const;
		/**
		 * @brief Update channel name.
		 */
		void setChannel(const UpdateChannel& updateChannel);
		/**
		 * @brief Revisions set in this map will not be treated as updates
		 *        even if they differ from the currently installed one.
		 *
		 * @param packagesRevisions
		 *     Key - package name. Value - list of revision numbers.
		 */
		void setIgnoreRevisions(const QMap<QString, QList<unsigned long long> >& packagesRevisions);
		/**
		 * @brief Controls if the download&installation process is automated.
		 *
		 * If set to true then downloadAndInstallConfirmationRequested()
		 * signal will be emitted and the updater will suspend until
		 * confirmDownloadAndInstall() slot is called. If false then
		 * download&installation will proceed automatically as soon
		 * as new packages are detected.
		 *
		 * Default: false.
		 */
		void setRequireDownloadAndInstallConfirmation(bool b);
		void start();
		
	public slots:
		void confirmDownloadAndInstall();

	signals:
		void downloadAndInstallConfirmationRequested();
		void finished();

	private:
		class PrivData;

		PrivData* d;

		void finishWithError(ErrorCode code);

	private slots:
		void onUpdaterInfoDownloadFinish();
};

#endif

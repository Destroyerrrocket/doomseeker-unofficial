//------------------------------------------------------------------------------
// wadretriever.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __WADRETRIEVER_H__
#define __WADRETRIEVER_H__

#include <QList>
#include <QNetworkReply>
#include <QString>
#include <QUrl>

#include "protocols/fixednetworkaccessmanager.h"
#include "entities/waddownloadinfo.h"
#include "wadretriever/wadinstaller.h"
#include "wadseekermessagetype.h"

class NetworkReplyWrapperInfo;
class WadDownloadInfo;

/**
 * @brief Retrieves WADs from remote locations and install them in a save
 *        directory.
 *
 * Multiple WADs are downloaded in parallel network requests. The limit of
 * network requests that may be running at once is specified by
 * setMaxConcurrentWadDownloads() method.
 *
 * Once a file is retrieved successfully and matches a name of one of the
 * WADs specified on a list set through setWads() method, this file is stored in
 * path specified through setTargetSavePath() method.
 *
 * If a file that is retrieved from a remote location is identified as an
 * archive, WadRetriever will go through this file in search for all WADs
 * specified for download.
 *
 * For each WAD that is installed successfully the wadInstalled() signal is
 * emitted and the WAD is removed from the download list.
 */
class WadRetriever : public QObject
{
	Q_OBJECT

	public:
		WadRetriever();
		~WadRetriever();

		void abort();

		/**
		 * @brief Provides new URL for a WAD download.
		 *
		 * This will start download if necessary. Please note that only one
		 * download of a single WAD may be running at single time. However
		 * multiple different WADs may be downloaded concurrently.
		 *
		 * @param wad
		 *      WAD for which the URL will be added. If WAD is not on the list
		 *      set by setWads() the URL will not be added.
		 * @param url
		 *      Download URL that will be queued for specified WAD.
		 */
		void addUrl(const WadDownloadInfo& wad, const QUrl& url);

		/**
		 * @brief Returns true if all WADs are pending for download URLs.
		 */
		bool areAllWadsPendingUrls() const;

		/**
		 * @brief Checks if WadRetriever has specified WAD on the list.
		 */
		bool hasWad(const WadDownloadInfo& wad) const;

		/**
		 * @brief Returns true if at least one WAD is being downloaded or has
		 *        an URL on the queue.
		 */
		bool isAnyDownloadWorking() const;

		/**
		 * @brief Returns true if at least one WAD is pending for download URL.
		 */
		bool isAnyWadPendingUrl() const;

		/**
		 * @brief Returns true if wad is currently being downloaded.
		 */
		bool isDownloadingWad(const WadDownloadInfo& wad) const;

		/**
		 * @brief Number of WADs that are currently being downloaded.
		 */
		int numCurrentRunningDownloads() const;

		/**
		 * @brief Number of WADs which are not being currently downloaded.
		 *        and which have no URLs on the queue.
		 */
		int numDownloadsPendingUrls() const;

		/**
		 * @brief Number of WADs that are still being downloaded or waiting for
		 *        download to start.
		 */
		int numTotalWadsDownloads() const
		{
			return d.wads.size();
		}

		/**
		 * @brief Max concurrent downloads of different WADs.
		 *
		 * Default value: 3.
		 */
		void setMaxConcurrentWadDownloads(int value)
		{
			d.maxConcurrentWadDownloads = value;
		}

		/**
		 * @brief Directory where the installed WADs will be saved.
		 */
		void setTargetSavePath(const QString& path)
		{
			d.targetSavePath = path;
		}

		/**
		 * @brief "User-Agent" to use in WWW queries.
		 */
		void setUserAgent(const QString& value)
		{
			d.userAgent = value;
		}

		/**
		 * @brief Sets a list of WADs that will be retrieved.
		 *
		 * This will not start the download by itself. Download URLs must be
		 * provided first
		 *
		 * @param wads
		 *      List of WadDownloadInfo objects describing WADs that will be
		 *      downloaded and installed. Contents of this list are copied.
		 */
		void setWads(const QList<WadDownloadInfo>& wads);

		/**
		 * @brief Skips current URL for the specified file.
		 *
		 * If available a new file URL will be immediately taken from the queue
		 * and new download will begin.
		 */
		void skipCurrentUrl(const WadDownloadInfo& wad);

		/**
		 * @brief Directory where the installed WADs will be saved.
		 */
		const QString& targetSavePath() const
		{
			return d.targetSavePath;
		}

		/**
		 * @brief Attempt to install specified file as a downloaded WAD.
		 *
		 * This omits the download procedure treating data param as already
		 * downloaded file contents. If data is recognized as an archive
		 * it will be
		 */
		void tryInstall(const QString& filename, const QByteArray& data);

	signals:
		/**
		 * @brief Emitted when all WADs are successfully installed.
		 *
		 * All WADs on the list set by setWads() method must be successfully
		 * installed in order for this signal to fire. WADs which are not
		 * installed but have no more download URLs will not cause this signal
		 * to be emitted. The object will just wait for URLs for those WADs.
		 */
		void finished();

		/**
		 * @brief Emitted when WadRetriever has something to announce.
		 *
		 * Note that critical errors mean that WadRetriever has no means
		 * to continue and the whole Wadseeker operation should abort.
		 */
		void message(const QString& msg, WadseekerLib::MessageType type);

		/**
		 * @brief Emitted when WadRetriever has finished current downloads
		 *        but is still pending for more URLs.
		 */
		void pendingUrls();

		/**
		 * @brief Emitted when WadRetriever finishes a download.
		 *
		 * @b NOTE: This doesn't mean that the WAD was successfully installed.
		 * It only servers as a notification that a download has been
		 * completed.
		 */
		void wadDownloadFinished(WadDownloadInfo wadDownloadInfo);

		/**
		 * @brief Emitted when a WAD is being downloaded.
		 */
		void wadDownloadProgress(WadDownloadInfo wadDownloadInfo, qint64 current, qint64 total);

		/**
		 * @brief Emitted when a WAD download just started.
		 */
		void wadDownloadStarted(WadDownloadInfo wadDownloadInfo, const QUrl& url);

		/**
		 * @brief Emitted when a WAD is successfully installed.
		 *
		 * @param wad
		 *      Instance of WadDownloadInfo describing the WAD that was
		 *      installed.
		 */
		void wadInstalled(WadDownloadInfo wad);

	private:
		class WadRetrieverInfo
		{
			public:
				QList<QUrl> downloadUrls;
				NetworkReplyWrapperInfo* pNetworkReply;
				WadDownloadInfo* wad;

				WadRetrieverInfo(const WadDownloadInfo& wad);
				~WadRetrieverInfo();

				bool operator==(const WadDownloadInfo& wad) const;
				bool operator!=(const WadDownloadInfo& wad) const;

				/**
				 * @brief Objects are equal if WadDownloadInfo objects are
				 *        equal.
				 */
				bool operator==(const WadRetrieverInfo& other) const;
				bool operator!=(const WadRetrieverInfo& other) const;

			private:
				WadRetrieverInfo(const WadRetrieverInfo& other) {}
				WadRetrieverInfo& operator=(const WadRetrieverInfo& other) {}

		};

		class PrivData
		{
			public:
				bool bIsAborting;
				int maxConcurrentWadDownloads;
				FixedNetworkAccessManager* pNetworkAccessManager;
				QString targetSavePath;

				/**
				 * @brief User agent to use in WWW queries.
				 */
				QString userAgent;

				/**
				 * @brief Global list of used download URLs. Makes sure
				 *        no URL is used twice.
				 */
				QList<QUrl> usedDownloadUrls;
				QList<WadRetrieverInfo* > wads;

		};

		PrivData d;

		/**
		 * @brief Extracts next valid download URL.
		 *
		 * Modifies the downloadUrls and usedDownloadUrls lists.
		 *
		 * @return Next URL that points to this WAD or invalid QUrl if
		 *         URL cannot be extracted.
		 */
		QUrl extractNextValidUrl(WadRetrieverInfo& wadRetrieverInfo);
		WadRetrieverInfo* findRetrieverInfo(const WadDownloadInfo& wad);
		WadRetrieverInfo* findRetrieverInfo(const QString& wadName);
		WadRetrieverInfo* findRetrieverInfo(const QNetworkReply* pNetworkReply);
		const WadRetrieverInfo* findRetrieverInfo(const WadDownloadInfo& wad) const;
		const WadRetrieverInfo* findRetrieverInfo(const QString& wadName) const;
		const WadRetrieverInfo* findRetrieverInfo(const QNetworkReply* pNetworkReply) const;

		/**
		 * @brief Next WAD that is not being downloaded but has download URLs.
		 */
		WadRetrieverInfo* getNextWaitingRetrieverInfo();
		QList< WadDownloadInfo* > getWadDownloadInfoList();

		/**
		 * @brief True if URL is either on the queue or already used.
		 */
		bool hasUrl(const WadRetrieverInfo& wadRetrieverInfo, const QUrl& url) const;

		/**
		 * @brief Performs operations basing on the result contents.
		 *
		 * Pending WADs list will be modified here and signals will be emitted.
		 *
		 * @return If false is returned the tryInstall() method must return
		 *         immediatelly. Otherwise is is allowed to continue.
		 */
		bool parseInstallerResult(const WadInstaller::WadInstallerResult& result, const QString& filename, bool bWasArchive);

		/**
		 * @brief Removes WadRetrieverInfo and aborts any downloads in progress.
		 */
		void removeWadRetrieverInfo(WadRetrieverInfo* pWadRetrieverInfo);

		/**
		 * @brief Attempts to extract meaningful file data from
		 *        the QNetworkReply.
		 */
		void resolveDownloadFinish(QNetworkReply* pReply, WadRetrieverInfo* pWadRetrieverInfo);

		void setNetworkReply(WadRetrieverInfo& wadRetrieverInfo, QNetworkReply* pReply);
		void startNextDownloads();
		void startNetworkQuery(WadRetrieverInfo& wadRetrieverInfo, const QUrl& url);

		bool wasUrlUsed(const QUrl& url) const;

	private slots:
		void networkQueryDownloadProgress(QNetworkReply* pReply, qint64 current, qint64 total);
		void networkQueryError(QNetworkReply* pReply, QNetworkReply::NetworkError code);
		void networkQueryFinished(QNetworkReply* pReply);
};

#endif

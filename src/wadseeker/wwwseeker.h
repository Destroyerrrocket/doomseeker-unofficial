//------------------------------------------------------------------------------
// wwwseeker.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __WWWSEEKER_H__
#define __WWWSEEKER_H__

#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include <QString>
#include <QUrl>

class NetworkReplySignalWrapper;

/**
 *	@brief Search and protocol wrapper class.
 *
 *	WWWSeeker class seeks specified files by iterating through
 *	previously defined set of sites and parsing their HTML code
 *	if necessary.
 *
 *	This class handles URLs by selecting proper protocols for them.
 *	It hides how protocols work and provides simple interface:
 *	request a file, inform about download progress and
 *	send a proper signal when download is finished.
 *
 *	URLs with wildcards can be used, see: primaryFile
 */
class WWWSeeker : public QObject
{
	Q_OBJECT

	public:
		WWWSeeker();
		virtual ~WWWSeeker();

		/**
		 * @brief Adds a URL to a site that will be used in the search.
		 *
		 * URLs that were already used by this WWWSeeker object are rejected.
		 */
		void addSiteUrl(const QUrl& url);

		/**
		 * @brief Convenience method. Calls addSiteUrl() for each URL on The
		 * list.
		 *
		 * URLs that were already used by this WWWSeeker object are rejected.
		 */
		void addSitesUrls(const QList<QUrl>& urlsList);

		/**
		 * @brief Clears visited URLs list.
		 *
		 * This allows URLs to be reused by the current WWWSeeker object.
		 * Such URLs however need to be readded by using addSiteUrl() method.
		 */
		void clearVisitedUrlsList()
		{
			d.visitedUrls.clear();
		}

		/**
		 * @brief Checks if WWWSeeker is processing any data.
		 */
		bool isWorking() const
		{
			return d.bIsWorking;
		}

		int maxConcurrentSiteDownloads() const
		{
			return d.maxConcurrentSiteDownloads;
		}

		/**
		 * @brief Maximum amount of URLs the WWWSeeker will go through
		 * at the same time.
		 */
		void setMaxConcurrectSiteDownloads(int value)
		{
			d.maxConcurrentSiteDownloads = value;
		}

		void setUserAgent(const QString& userAgent);

		/**
		 * @brief Begins search for files.
		 *
		 * Search is performed by downloading sites specified by sitesUrls
		 * list and seeking out links on these sites that either directly
		 * point to one of the specified filenames or may lead to subsites
		 * that may contain links to these files.
		 *
		 * WWWSeeker will look for all filenames specified on the list.
		 *
		 * This will do nothing if isWorking() returns true.
		 */
		void startSearch(const QStringList& seekedFilenames);

		const QString& userAgent() const;

	public slots:
		/**
		 * @brief Aborts the current search operation.
		 *
		 * finished() signal is emitted once the abort completes.
		 */
		void abort();

	signals:
		/**
		 * @brief Emitted when seeker aborts or there is nothing more to search.
		 */
		void finished();

		/**
		 * @brief Emitted for each direct link to a file that is found.
		 *
		 * @param filename
		 *      Filename to which the URL refers to.
		 * @param url
		 *      URL under which the file resides.
		 */
		void linkFound(const QString& filename, const QUrl& url);

		/**
		 * @brief Emitted when a WWW site finishes download.
		 */
		void siteFinished(const QUrl& site, int subsiteLinksFound, int directLinksFound);

		/**
		 * @brief Emitted when a WWW site is being downloaded.
		 */
		void siteProgress(const QUrl& site, qint64 bytes, qint64 total);

		/**
		 * @brief Emitted when a download of a WWW site starts.
		 */
		void siteStarted(const QUrl& site);

	private:
		class NetworkQueryInfo
		{
			public:
				NetworkReplySignalWrapper* pSignalWrapper;
				QNetworkReply* pReply;

				NetworkQueryInfo(QNetworkReply* pReply);
				~NetworkQueryInfo();

				void deleteMembersLater();

				/**
				 * @brief NetworkQueryInfo objects are equal if their pReply
				 * is the same.
				 */
				bool operator==(const NetworkQueryInfo& other) const;
				bool operator!=(const NetworkQueryInfo& other) const
				{
					return ! (*this == other);
				}

				bool operator==(const QNetworkReply* pReply) const;
				bool operator!=(const QNetworkReply* pReply) const
				{
					return ! (*this == pReply);
				}
		};

		class PrivData
		{
			public:
				bool bIsAborting;
				bool bIsWorking;

				/**
				 * @brief Default value: 3
				 */
				int maxConcurrentSiteDownloads;

				/**
				 * @brief Currently running network queries.
				 *
				 * Amount here will not go above the maxConcurrentSiteDownloads.
				 */
				QList<NetworkQueryInfo*> networkQueries;

				QNetworkAccessManager* pNetworkAccessManager;

				QStringList seekedFilenames;

				/**
				 * @brief URLs that will be used in a search.
				 */
				QList<QUrl> sitesUrls;

				/**
				 * @brief Holds list of all used URLs.
				 *
				 * Prevents checking the same site twice.
				 */
				QList<QUrl> visitedUrls;

				/**
				 * @brief User Agent used in WWW queries.
				 */
				QString userAgent;
		};

		PrivData d;

		void addNetworkReply(QNetworkReply* pReply);
		NetworkQueryInfo* findNetworkQueryInfo(QNetworkReply* pReply);
		void startNextSites();
		bool wasUrlUsed(const QUrl& url) const;


	private slots:
		void networkQueryFinished(QNetworkReply* pReply);
};

#endif

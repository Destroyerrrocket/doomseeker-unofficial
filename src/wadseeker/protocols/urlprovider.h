//------------------------------------------------------------------------------
// urlprovider.h
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __URLPROVIDER_H__
#define __URLPROVIDER_H__

#include <QList>
#include <QUrl>

class MirrorStorage;

/**
 * @brief URL provider prepared to work with WWWSeeker and WadRetriever.
 *
 * This class is designed to remember all URLs for its lifetime and never
 * allow any duplicates. This means that each URL that was added to the
 * list will not be allowed to appear on this list again, even if one
 * of the remove methods were used.
 *
 * Mirror URLs are supported in a way that allows URLs to be associated with
 * each other as URLs pointing to the same file. These URLs will be returned
 * normally by first() method and operator[]. However, user may decide to 
 * remove the URL and all its mirrors at once, for example when user decides
 * that the mirrors do not contain the requested file.
 */
class URLProvider
{
	public:
		URLProvider();
		~URLProvider();

		/**
		 * @brief Adds mirror URL to the specified original URL.
		 *
		 * @b Note: If both URLs are not yet known, then a new
		 * mirror set is created.
		 */		
		void addMirror(const QUrl& originalUrl, const QUrl& mirrorUrl);

		/**
		 * @brief Adds all URLs on the list and treats them as mirrors
		 *        to the same resource.
		 */
		void addMirrorUrls(const QList<QUrl>& urls);
		
		/**
		 * @brief Adds a single URL to the list. No mirrors.
		 */
		void addUrl(const QUrl& url);
		
		/**
		 * @brief First URL on the list. Same as operator[0].
		 */
		const QUrl& first() const;
		
		/**
		 * @brief Checks if URL was known to the object during its entire 
		 *        lifetime.
		 *
		 * This URL might not appear anymore on the available URLs 
		 . list. Such URLs will return false on hasUrl().
		 */
		bool hasOrHadUrl(const QUrl& url) const;
		
		/**
		 * @brief Checks if URL is on the available URLs list.
		 */
		bool hasUrl(const QUrl& url) const;
		
		/**
		 * @brief True if there are no available URLs.
		 */
		bool isEmpty() const;
		
		/**
		 * @brief Number of currently available URLs.
		 */
		unsigned numUrls() const;
		
		/**
		 * @brief Accesses URL at specified index.
		 *
		 * The URLs appear on the list in the order they were added in.
		 * 
		 * @param index
		 *      URL index. This should be less than numUrls()
		 */
		const QUrl& operator[](int index) const;
		
		/**
		 * @brief Same as addUrl()
		 */
		URLProvider& operator<<(const QUrl& url);
		
		/**
		 * @brief Removes single URL from the available URLs list.
		 *
		 * This will not make the object forget the URL. hasOrHad() for this
		 * URL will still return true and re-adding the same URL will not be
		 * possible.
		 */
		void removeUrl(const QUrl& url);
		
		/**
		 * @brief Removes URL and all its mirrors from the available URLs list.
		 *
		 * This will not make the object forget the URLs. hasOrHad() for these
		 * URLs will still return true and re-adding the same URLs will not be
		 * possible.
		 */
		void removeUrlAndMirrors(const QUrl& url);
		
		/**
		 * @brief Returns the first URL from available URLs list and removes
		 *        that URL from that list.
		 */
		QUrl takeFirst();
	
	private:
		QList<QUrl> allUrls;
		QList<MirrorStorage*> mirrors;
		
		QList<MirrorStorage*> mirrorsWithUrl(const QUrl& url);
		
		void removeAllUrls(const MirrorStorage* pMirror);
};

#endif
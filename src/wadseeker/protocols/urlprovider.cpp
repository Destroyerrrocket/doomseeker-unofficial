//------------------------------------------------------------------------------
// urlprovider.cpp
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
#include "urlprovider.h"

#include "wwwseeker/urlparser.h"

class MirrorStorage
{
	public:
		QList<QUrl> urls;
		
		bool hasUrl(const QUrl& url) const
		{
			foreach (const QUrl& urlOnList, urls)
			{
				if (UrlParser::urlEqualsCaseInsensitive(urlOnList, url))
				{
					return true;
				}
			}
			
			return false;
		}
		
		bool isEmpty() const
		{
			return urls.isEmpty();
		}
};
///////////////////////////////////////////////////////////////////////////////
URLProvider::URLProvider()
{
}

URLProvider::~URLProvider()
{
	foreach(MirrorStorage* pMirror, mirrors)
	{
		delete pMirror;
	}
}

void URLProvider::addMirror(const QUrl& originalUrl, const QUrl& mirrorUrl)
{
	QList<MirrorStorage*> mirrors = mirrorsWithUrl(originalUrl);
	if (!mirrors.isEmpty())
	{
		if (!hasOrHadUrl(mirrorUrl))
		{
			MirrorStorage* pFirstMirror = mirrors.first();
			pFirstMirror->urls << mirrorUrl;
			allUrls << mirrorUrl;
		}
	}
	else
	{
		QList<QUrl> newUrls;
		newUrls << originalUrl;
		
		if (!hasOrHadUrl(mirrorUrl))
		{
			newUrls << mirrorUrl;
		}
		
		addMirrorUrls(newUrls);
	}
}

void URLProvider::addMirrorUrls(const QList<QUrl>& urls)
{
	MirrorStorage* pMirrorStorage = new MirrorStorage();

	foreach (const QUrl& url, urls)
	{
		if (!hasOrHadUrl(url))
		{
			pMirrorStorage->urls << url;
			allUrls << url;
		}
	}
	
	if (!pMirrorStorage->isEmpty())
	{
		mirrors << pMirrorStorage;
	}
	else
	{
		delete pMirrorStorage;
	}
}

void URLProvider::addUrl(const QUrl& url)
{
	if (!hasOrHadUrl(url))
	{
		MirrorStorage* pMirrorStorage = new MirrorStorage();
		pMirrorStorage->urls << url;
		
		allUrls << url;
		mirrors << pMirrorStorage;
	}
}

const QUrl& URLProvider::first() const
{
	return allUrls.first();
}

bool URLProvider::isEmpty() const
{
	return allUrls.isEmpty();
}

bool URLProvider::hasOrHadUrl(const QUrl& url) const
{
	foreach (MirrorStorage* pMirror, mirrors)
	{
		if (pMirror->hasUrl(url))
		{
			return true;
		}
	}
	
	return false;
}

bool URLProvider::hasUrl(const QUrl& url) const
{
	foreach (const QUrl& urlOnList, allUrls)
	{
		if (UrlParser::urlEqualsCaseInsensitive(urlOnList, url))
		{
			return true;
		}
	}
			
	return false;
}

QList<MirrorStorage*> URLProvider::mirrorsWithUrl(const QUrl& url)
{
	QList<MirrorStorage*> mirrorsWithUrls;
	foreach (MirrorStorage* pMirror, mirrors)
	{
		if (pMirror->hasUrl(url))
		{
			mirrorsWithUrls << pMirror;
		}
	}
	
	return mirrorsWithUrls;
}

unsigned URLProvider::numUrls() const
{
	return allUrls.size();
}

const QUrl& URLProvider::operator[](int index) const
{
	return allUrls[index];
}

URLProvider& URLProvider::operator<<(const QUrl& url)
{
	addUrl(url);
	return *this;
}

void URLProvider::removeAllUrls(const MirrorStorage* pMirror)
{
	foreach (const QUrl& url, pMirror->urls)
	{
		allUrls.removeAll(url);
	}
}

void URLProvider::removeUrl(const QUrl& url)
{
	allUrls.removeAll(url);
}

void URLProvider::removeUrlAndMirrors(const QUrl& url)
{
	QList<MirrorStorage*> mirrors = mirrorsWithUrl(url);
	foreach (MirrorStorage* pMirror, mirrors)
	{
		removeAllUrls(pMirror);
	}
}

QUrl URLProvider::takeFirst()
{
	QUrl url = first();
	removeUrl(url);
	return url;
}


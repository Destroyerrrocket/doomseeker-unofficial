//------------------------------------------------------------------------------
// wadretriever.cpp
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
#include "wadretriever.h"

#include "protocols/entities/networkreplywrapperinfo.h"
#include "entities/waddownloadinfo.h"

WadRetriever::WadRetriever()
{
	d.maxConcurrentWadDownloads = 3;
}

void WadRetriever::addUrl(const WadDownloadInfo& wad, const QUrl& url)
{

}

WadRetriever::WadRetrieverInfo* WadRetriever::findRetrieverInfo(const WadDownloadInfo& wad)
{
	QList<WadRetrieverInfo>::iterator it;
	for (it = d.wads.begin(); it != d.wads.end(); ++it)
	{
		WadRetrieverInfo& wadInfo = *it;
		if (wadInfo == wad)
		{
			return &wadInfo;
		}
	}

	return NULL;
}

WadRetriever::WadRetrieverInfo* WadRetriever::findRetrieverInfo(const QString& wadName)
{
	QList<WadRetrieverInfo>::iterator it;
	for (it = d.wads.begin(); it != d.wads.end(); ++it)
	{
		WadRetrieverInfo& wadInfo = *it;
		if (wadInfo == wadName)
		{
			return &wadInfo;
		}
	}

	return NULL;
}

WadRetriever::WadRetrieverInfo* WadRetriever::findRetrieverInfo(const QNetworkReply* pNetworkReply)
{
	QList<WadRetrieverInfo>::iterator it;
	for (it = d.wads.begin(); it != d.wads.end(); ++it)
	{
		WadRetrieverInfo& wadInfo = *it;
		if (wadInfo.pNetworkReply != NULL)
		{
			if (*wadInfo.pNetworkReply == pNetworkReply)
			{
				return &wadInfo;
			}
		}
	}

	return NULL;
}

bool WadRetriever::hasWad(const WadDownloadInfo& wad) const
{
	foreach (const WadRetrieverInfo& wadInfo, d.wads)
	{
		if (wadInfo == wad)
		{
			return true;
		}
	}

	return false;
}

int WadRetriever::numCurrentRunningDownloads() const
{
	int num = 0;

	// Count each retriever info.
	foreach (const WadRetrieverInfo& info, d.wads)
	{
		if (info.pNetworkReply != NULL)
		{
			++num;
		}
	}

	return num;
}

void WadRetriever::setWads(const QList<WadDownloadInfo>& wads)
{
	foreach (const WadDownloadInfo& wad, wads)
	{
		if (!hasWad(wad))
		{
			WadRetrieverInfo retrieverInfo(wad);

			d.wads << retrieverInfo;
		}
	}
}

void startNextDownloads()
{

}

////////////////////////////////////////////////////////////////////////////////

WadRetriever::WadRetrieverInfo::WadRetrieverInfo(const WadDownloadInfo& wad)
{
	this->pNetworkReply = NULL;
	this->wad = new WadDownloadInfo(wad);
}

WadRetriever::WadRetrieverInfo::~WadRetrieverInfo()
{
	if (pNetworkReply != NULL)
	{
		pNetworkReply->deleteMembersLater();
		delete pNetworkReply;
	}

	delete wad;
}

bool WadRetriever::WadRetrieverInfo::operator==(const WadDownloadInfo& wad) const
{
	return *this->wad == wad;
}

bool WadRetriever::WadRetrieverInfo::operator!=(const WadDownloadInfo& wad) const
{
	return *this->wad != wad;
}

bool WadRetriever::WadRetrieverInfo::operator==(const WadRetrieverInfo& other) const
{
	return *this->wad == *other.wad;
}

bool WadRetriever::WadRetrieverInfo::operator!=(const WadRetrieverInfo& other) const
{
	return !(*this == other);
}

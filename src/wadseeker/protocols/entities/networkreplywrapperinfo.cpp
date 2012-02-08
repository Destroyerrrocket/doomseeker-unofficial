//------------------------------------------------------------------------------
// networkreplywrapperinfo.cpp
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
#include "networkreplywrapperinfo.h"

#include "protocols/networkreplysignalwrapper.h"

NetworkReplyWrapperInfo::NetworkReplyWrapperInfo(QNetworkReply* pReply, const QUrl& requestUrl)
{
	this->pReply = pReply;
	this->requestUrl = requestUrl;

	if (pReply != NULL)
	{
		pSignalWrapper = new NetworkReplySignalWrapper(pReply);
	}
	else
	{
		pSignalWrapper = NULL;
	}
}

NetworkReplyWrapperInfo::~NetworkReplyWrapperInfo()
{
	if (pReply != NULL)
	{
		delete pSignalWrapper;
		delete pReply;
	}
}

void NetworkReplyWrapperInfo::deleteMembersLater()
{
	if (pReply != NULL)
	{
		delete pSignalWrapper;
		pReply->abort();
		pReply->deleteLater();

		pReply = NULL;
	}
}

bool NetworkReplyWrapperInfo::operator==(const NetworkReplyWrapperInfo& other) const
{
	return *this == other.pReply;
}

bool NetworkReplyWrapperInfo::operator==(const QNetworkReply* pReply) const
{
	return this->pReply == pReply;
}



//------------------------------------------------------------------------------
// networkreplysignalwrapper.cpp
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
#include "networkreplysignalwrapper.h"

NetworkReplySignalWrapper::NetworkReplySignalWrapper(QNetworkReply* pReply)
{
	this->pReply = pReply;

	this->connect(pReply, SIGNAL( downloadProgress() ),
		SLOT( downloadProgressSlot() ));
	this->connect(pReply, SIGNAL( error() ),
		SLOT( errorSlot() ));
	this->connect(pReply, SIGNAL( finished() ),
		SLOT( finishedSlot() ));
	this->connect(pReply, SIGNAL( metaDataChanged() ),
		SLOT( metaDataChangedSlot() ));
	this->connect(pReply, SIGNAL( sslErrors() ),
		SLOT( sslErrorsSlot() ));
	this->connect(pReply, SIGNAL( uploadProgress() ),
		SLOT( uploadProgressSlot() ));
}

void NetworkReplySignalWrapper::downloadProgressSlot(qint64 bytesReceived, qint64 bytesTotal)
{
	emit downloadProgress(pReply, bytesReceived, bytesTotal);
}

void NetworkReplySignalWrapper::errorSlot(QNetworkReply::NetworkError code)
{
	emit error(pReply, code);
}

void NetworkReplySignalWrapper::finishedSlot()
{
	emit finished(pReply);
}

void NetworkReplySignalWrapper::metaDataChangedSlot()
{
	emit metaDataChanged(pReply);
}

void NetworkReplySignalWrapper::sslErrorsSlot(const QList<QSslError>& errors)
{
	emit sslErrors(pReply, errors);
}

void NetworkReplySignalWrapper::uploadProgressSlot(qint64 bytesSent, qint64 bytesTotal)
{
	emit uploadProgress(pReply, bytesSent, bytesTotal);
}

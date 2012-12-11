//------------------------------------------------------------------------------
// autoupdater.cpp
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
#include "autoupdater.h"

#include "json.h"
#include "version.h"
#include <wadseeker/protocols/fixednetworkaccessmanager.h>
#include <QDebug>
#include <QNetworkRequest>
#include <cassert>

class AutoUpdater::PrivData
{
	public:
		bool bStarted;
		UpdateChannel channel;
		ErrorCode errorCode;
		FixedNetworkAccessManager* pNam;
		QNetworkReply* pNetworkReply;
};
//////////////////////////////////////////////////////////////////////////////

// This can be set to different values depending on target platform.
const QString AutoUpdater::UPDATER_INFO_URL = "http://doomseeker.drdteam.org/updates/update-info_win32.js";

const QString MAIN_PROGRAM_NODE = "doomseeker";

AutoUpdater::AutoUpdater(QObject* pParent)
: QObject(pParent)
{
	d = new PrivData();
	d->bStarted = false;
	d->errorCode = EC_Ok;
	d->pNam = new FixedNetworkAccessManager();
	d->pNetworkReply = NULL;
}

AutoUpdater::~AutoUpdater()
{
	if (d->pNetworkReply != NULL)
	{
		d->pNetworkReply->disconnect();
		d->pNetworkReply->abort();
		d->pNetworkReply->deleteLater();
	}
	d->pNam->disconnect();
	d->pNam->deleteLater();
	delete d;
}

void AutoUpdater::abort()
{
}

AutoUpdater::UpdateChannel AutoUpdater::channel() const
{
	return d->channel;
}

void AutoUpdater::confirmDownload()
{
}

AutoUpdater::ErrorCode AutoUpdater::errorCode() const
{
	return d->errorCode;
}

QString AutoUpdater::errorCodeToString(ErrorCode code)
{
	switch (code)
	{
		case EC_Ok:
			return QString();
		case EC_UpdaterInfoDownloadProblem:
			return tr("Failed to download updater info file.");
		case EC_UpdaterInfoCannotParse:
			return tr("Cannot parse updater info file.");
		case EC_UpdaterInfoMissingMainProgramNode:
			return tr("Main program node is missing from updater info file.");
		default:
			return tr("Unknown error.");
	}
}

QString AutoUpdater::errorString() const
{
	return errorCodeToString(errorCode());
}

void AutoUpdater::finishWithError(ErrorCode code)
{
	d->errorCode = code;
	emit finished();
}

QNetworkReply::NetworkError AutoUpdater::lastNetworkError() const
{
	if (errorCode() != EC_Ok && d->pNetworkReply != NULL)
	{
		return d->pNetworkReply->error();
	}
}

void AutoUpdater::onUpdaterInfoDownloadFinish()
{
	if (d->pNetworkReply->error() != QNetworkReply::NoError)
	{
		finishWithError(EC_UpdaterInfoDownloadProblem);
		return;
	}
	QByteArray json = d->pNetworkReply->readAll();
	QVariant var = QtJson::Json::parse(json);
	if (var.isValid())
	{
		QVariantMap metaData = var.toMap();
		if (metaData.contains(MAIN_PROGRAM_NODE))
		{
			
		}
		else
		{
			finishWithError(EC_UpdaterInfoMissingMainProgramNode);
		}
	}
	else
	{
		finishWithError(EC_UpdaterInfoCannotParse);
	}
}

void AutoUpdater::setChannel(UpdateChannel updateChannel)
{
	d->channel = updateChannel;
}

void AutoUpdater::start()
{
	if (d->bStarted)
	{
		qDebug() << "Cannot start AutoUpdater more than once.";
		assert(d->bStarted);
		return;
	}
	d->bStarted = true;
	QNetworkRequest request;
	request.setRawHeader("User-Agent", Version::userAgent().toAscii());
	request.setUrl(UPDATER_INFO_URL);
	QNetworkReply* pReply = d->pNam->get(request);
	// The updater info file should always be very small and
	// we can safely store it all in memory.
	this->connect(pReply,
		SIGNAL(finished()),
		SLOT(onUpdaterInfoDownloadFinish()));
	d->pNetworkReply = pReply;
}

QString AutoUpdater::updateChannelName(UpdateChannel updateChannel)
{
	switch (updateChannel)
	{
		case UC_Stable:
			return "stable";
		case UC_Beta:
			return "beta";
		default:
			return QString();
	}
}

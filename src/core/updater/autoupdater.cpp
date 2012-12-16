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

#include "updater/updatechannel.h"
#include "updater/updaterinfoparser.h"
#include "version.h"
#include <wadseeker/protocols/fixednetworkaccessmanager.h>
#include <QDebug>
#include <QNetworkRequest>
#include <cassert>

class AutoUpdater::PrivData
{
	public:
		bool bDownloadAndInstallRequireConfirmation;
		bool bIsRunning;
		bool bStarted;
		UpdateChannel channel;
		ErrorCode errorCode;
		FixedNetworkAccessManager* pNam;
		QNetworkReply* pNetworkReply;
};
//////////////////////////////////////////////////////////////////////////////

const QString AutoUpdater::PLUGIN_PREFIX = "p-";
const QString AutoUpdater::MAIN_PROGRAM_PACKAGE_NAME = "doomseeker";
// This can be set to different values depending on target platform.
const QString AutoUpdater::UPDATER_INFO_URL = "http://doomseeker.drdteam.org/updates/update-info_win32.js";

AutoUpdater::AutoUpdater(QObject* pParent)
: QObject(pParent)
{
	d = new PrivData();
	d->bDownloadAndInstallRequireConfirmation = false;
	d->bIsRunning = false;
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

const UpdateChannel& AutoUpdater::channel() const
{
	return d->channel;
}

void AutoUpdater::confirmDownloadAndInstall()
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
		case EC_NullUpdateChannel:
			return tr("No valid update channel was specified.");
		case EC_UpdaterInfoDownloadProblem:
			return tr("Failed to download updater info file.");
		case EC_UpdaterInfoCannotParse:
			return tr("Cannot parse updater info file.");
		case EC_UpdaterInfoMissingMainProgramNode:
			return tr("Main program node is missing from updater info file.");
		case EC_MissingRevisionInfo:
			return tr("Revision info on one of the packages is missing from the "
				"updater info file. Check the log for details.");
		case EC_MissingDownloadUrl:
			return tr("Download URL for one of the packages is missing from the "
				"updater info file. Check the log for details.");
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
	d->bIsRunning = false;
	d->errorCode = code;
	emit finished();
}

bool AutoUpdater::isRunning() const
{
	return d->bIsRunning;
}

QNetworkReply::NetworkError AutoUpdater::lastNetworkError() const
{
	if (errorCode() != EC_Ok && d->pNetworkReply != NULL)
	{
		return d->pNetworkReply->error();
	}
	return QNetworkReply::NoError;
}

void AutoUpdater::onUpdaterInfoDownloadFinish()
{
	if (d->pNetworkReply->error() != QNetworkReply::NoError)
	{
		finishWithError(EC_UpdaterInfoDownloadProblem);
		return;
	}
	QByteArray json = d->pNetworkReply->readAll();
	UpdaterInfoParser parser;
	ErrorCode parseResult = (ErrorCode) parser.parse(json);
	if (parseResult == EC_Ok)
	{
		// TODO Stuff
	}
	else
	{
		finishWithError(parseResult);
	}
}

void AutoUpdater::setChannel(const UpdateChannel& updateChannel)
{
	d->channel = updateChannel;
}

void AutoUpdater::setRequireDownloadAndInstallConfirmation(bool b)
{
	d->bDownloadAndInstallRequireConfirmation = b;
}

void AutoUpdater::start()
{
	if (d->bStarted)
	{
		qDebug() << "Cannot start AutoUpdater more than once.";
		// Always cause assertion failure. Program shouldn't
		// go into this state.
		assert(false && "Cannot start AutoUpdater more than once.");
		return;
	}
	d->bStarted = true;
	if (d->channel.isNull())
	{
		finishWithError(EC_NullUpdateChannel);
		return;
	}
	d->bIsRunning = true;
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

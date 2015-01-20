//------------------------------------------------------------------------------
// wadarchiveclient.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "wadarchiveclient.h"

#include "entities/waddownloadinfo.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

class WadArchiveClient::PrivData
{
public:
	QNetworkAccessManager *nam;
	QNetworkReply *reply;
	QString userAgent;
	QList<WadDownloadInfo> queue;

	QUrl buildUrl(const WadDownloadInfo &wad)
	{
		if (wad.name().contains("/"))
		{
			return QUrl();
		}
		return QUrl(QString("http://www.wad-archive.com/wadseeker/%1").arg(wad.name()));
	}
};


WadArchiveClient::WadArchiveClient()
{
	d = new PrivData();
	d->nam = new QNetworkAccessManager();
	d->reply = NULL;
}

WadArchiveClient::~WadArchiveClient()
{
	abort();
	if (d->reply != NULL)
	{
		d->reply->deleteLater();
	}
	d->nam->deleteLater();
	delete d;
}

void WadArchiveClient::abort()
{
	d->queue.clear();
	if (d->reply != NULL)
	{
		d->reply->abort();
	}
	else
	{
		emit finished();
	}
}

void WadArchiveClient::emitFinished()
{
	emit message(tr("Wad Archive finished."), WadseekerLib::NoticeImportant);
	emit finished();
}

void WadArchiveClient::enqueue(const WadDownloadInfo &wad)
{
	if (d->buildUrl(wad).isValid())
	{
		d->queue << wad;
	}
}

bool WadArchiveClient::isWorking() const
{
	return false;
}

void WadArchiveClient::setUserAgent(const QString &userAgent)
{
	d->userAgent = userAgent;
}

void WadArchiveClient::start()
{
	#ifndef NDEBUG
		qDebug() << "WadArchiveClient::start()";
	#endif
	emit message(tr("Wad Archive started."), WadseekerLib::Notice);
	startNextInQueue();
}

void WadArchiveClient::startNextInQueue()
{
	if (d->queue.isEmpty())
	{
		emitFinished();
		return;
	}
	WadDownloadInfo wad = d->queue.takeFirst();
	QUrl url = d->buildUrl(wad);
	#ifndef NDEBUG
		qDebug() << "wad archive search:" << url;
	#endif
	emit message(tr("Querying Wad Archive for %1").arg(wad.name()), WadseekerLib::Notice);

	QNetworkRequest request;
	request.setUrl(url);
	request.setRawHeader("User-Agent", d->userAgent.toAscii());
	d->reply = d->nam->get(request);
	this->connect(d->reply, SIGNAL(finished()), SLOT(onQueryFinished()));
}

void WadArchiveClient::onQueryFinished()
{
	emit message(tr("Wad Archive query finished."), WadseekerLib::Notice);
	d->reply->deleteLater();
	d->reply = NULL;
	startNextInQueue();
}

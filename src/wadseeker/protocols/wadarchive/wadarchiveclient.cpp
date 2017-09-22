//------------------------------------------------------------------------------
// wadarchiveclient.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "wadarchiveclient.h"

#include "entities/waddownloadinfo.h"
#include "protocols/json.h"
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

class WadArchiveClient::PrivData
{
public:
	WadDownloadInfo currentWad;
	QNetworkAccessManager *nam;
	QNetworkReply *reply;
	QString userAgent;
	QList<WadDownloadInfo> queue;
	QList<QUrl> urls;

	QUrl buildUrl(const WadDownloadInfo &wad)
	{
		if (!wad.isValid() || wad.name().contains("/"))
		{
			return QUrl();
		}
		return QUrl(QString("https://www.wad-archive.com/wadseeker/%1").arg(wad.name()));
	}

	QUrl buildBadUrlReporterUrl(const QUrl &url)
	{
		return QUrl(QString("https://www.wad-archive.com/wadseeker/missing/%1")
			.arg(QString(url.toString().toUtf8().toBase64())));
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
	return !d->queue.isEmpty() || d->reply != NULL;
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
	d->currentWad = d->queue.takeFirst();
	QUrl url = d->buildUrl(d->currentWad);
	#ifndef NDEBUG
		qDebug() << "wad archive search:" << url;
	#endif
	emit message(tr("Querying Wad Archive for %1").arg(d->currentWad.name()), WadseekerLib::Notice);

	QNetworkRequest request;
	request.setUrl(url);
	request.setRawHeader("User-Agent", d->userAgent.toUtf8());
	d->reply = d->nam->get(request);
	this->connect(d->reply, SIGNAL(finished()), SLOT(onQueryFinished()));
}

void WadArchiveClient::onQueryFinished()
{
	emit message(tr("Wad Archive query finished."), WadseekerLib::Notice);
	QVariantList elements = QtJson::Json::parse(d->reply->readAll()).toList();
	if (elements.size() > 0)
	{
		parseWadArchiveStructure(elements[0].toMap());
	}

	d->reply->deleteLater();
	d->reply = NULL;
	startNextInQueue();
}

void WadArchiveClient::parseWadArchiveStructure(const QVariantMap &map)
{
	QVariantList links = map["links"].toList();
	foreach (QVariant link, links)
	{
		d->urls << QUrl(link.toString());
		emit urlFound(d->currentWad.name(), QUrl(link.toString()));
	}
}

void WadArchiveClient::reportBadUrlIfOriginatingFromHere(const QUrl &url)
{
	if (d->urls.contains(url))
	{
		emit message(tr("Reporting bad URL to Wad Archive: %1").arg(url.toString()),
			WadseekerLib::Notice);
		QUrl reportUrl = d->buildBadUrlReporterUrl(url);
		#ifndef NDEBUG
			qDebug() << "Wad Archive report url:" << reportUrl;
		#endif
		QNetworkRequest request;
		request.setUrl(reportUrl);
		request.setRawHeader("User-Agent", d->userAgent.toUtf8());
		QNetworkReply *reply = d->nam->get(request);
		connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
	}
}

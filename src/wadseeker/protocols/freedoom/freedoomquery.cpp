//------------------------------------------------------------------------------
// freedoomquery.cpp
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
#include "freedoomquery.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QUrl>
#include "entities/modset.h"
#include "protocols/freedoom/freedoominfoparser.h"
#include "wadseekerversioninfo.h"

DClass<FreedoomQuery>
{
public:
	QString error;
	ModSet modSet;
	QNetworkAccessManager *nam;
	QNetworkReply *reply;

	QString freedoomUrl() const
	{
		return "https://freedoom.github.io/download.json";
	}
};
DPointeredNoCopy(FreedoomQuery)

FreedoomQuery::FreedoomQuery(QObject *parent)
: QObject(parent)
{
	d->nam = new QNetworkAccessManager();
	d->reply = NULL;
}

FreedoomQuery::~FreedoomQuery()
{
	if (d->reply != NULL)
	{
		d->reply->deleteLater();
	}
	d->nam->deleteLater();
}

void FreedoomQuery::start()
{
	QNetworkRequest request;
	request.setUrl(d->freedoomUrl());
	request.setRawHeader("User-Agent", WadseekerVersionInfo::userAgent().toUtf8());

	d->reply = d->nam->get(request);
	this->connect(d->reply, SIGNAL(finished()), SLOT(onNetworkQueryFinished()));
}

void FreedoomQuery::onNetworkQueryFinished()
{
	QByteArray contents = d->reply->readAll();

	if (d->reply->error() == QNetworkReply::NoError)
	{
		FreedoomInfoParser parser(contents);
		d->modSet = parser.parse();
	}
	else
	{
		d->error = d->reply->errorString();
	}

	d->reply->disconnect(this);
	d->reply->deleteLater();
	d->reply = NULL;
	emit finished();
}

const QString &FreedoomQuery::error() const
{
	return d->error;
}

const ModSet &FreedoomQuery::modSet() const
{
	return d->modSet;
}

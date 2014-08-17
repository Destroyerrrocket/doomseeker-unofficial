//------------------------------------------------------------------------------
// idgamesreply.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "idgamesreply.h"

#include <QDebug>
#include "protocols/idgames/idgamesresult.h"
#include "protocols/json.h"

class IdgamesReply::PrivData
{
	public:
		QNetworkReply *reply;
		IdgamesResult result;
};

IdgamesReply::IdgamesReply(QNetworkReply *reply)
{
	d = new PrivData();
	d->reply = reply;
	this->connect(reply, SIGNAL(finished()), SLOT(onNetworkFinished()));
}

IdgamesReply::~IdgamesReply()
{
	d->reply->deleteLater();
	delete d;
}

void IdgamesReply::abort()
{
	d->reply->abort();
}

void IdgamesReply::onNetworkFinished()
{
	if (d->reply->error() == QNetworkReply::NoError)
	{
		QString json = d->reply->readAll();
		d->result = IdgamesResult(QtJson::Json::parse(json));
		#ifndef NDEBUG
			qDebug() << "idgames onNetworkFinished()" << json;
			qDebug() << "idgames onNetworkFinished() parsed" << QtJson::Json::parse(json);
		#endif

	}
	else
	{
		d->result = IdgamesResult::mkError(tr("Network Error"), d->reply->errorString());
	}
	emit finished();
}

IdgamesResult IdgamesReply::result() const
{
	return d->result;
}

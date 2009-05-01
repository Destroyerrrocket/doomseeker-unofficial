//------------------------------------------------------------------------------
// http.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "http.h"
#include <QByteArray>

Http::Http()
{
	construct();
}

Http::Http(QString s)
{
	construct();

	setSite(s);
}

Http::~Http()
{
	construct();
}

void Http::construct()
{
	Qt::ConnectionType ct = Qt::DirectConnection;

	connect(&qHttp, SIGNAL( readyRead ( const QHttpResponseHeader& ) ), this, SLOT( read(const QHttpResponseHeader&)), ct);
	connect(&qHttp, SIGNAL( requestFinished ( int, bool ) ), this, SLOT( reqFinished(int, bool)), ct);
}

QList<Link>	Http::links()
{
	QList<Link> list;
	Link l = {"a", "b"};
	list.append(l);
	return list;
}

void Http::sendRequestGet(QString resource)
{
	qHttp.get(resource);
}

void Http::setSite(const QString& s)
{
	site = s;
	qHttp.setHost(s);
}

void Http::read(const QHttpResponseHeader& httpResp)
{
	qDebug() << "Responded!";
	qDebug() << "Status code:" << httpResp.statusCode() << httpResp.reasonPhrase();

	const QList<QPair<QString, QString> >& vals = httpResp.values();
	QList<QPair<QString, QString> >::const_iterator it;
	for (it = vals.begin(); it != vals.end(); ++it)
	{
		qDebug() << it->first << "=" << it->second;
	}

	data = qHttp.readAll();
	qDebug() << "Bytes read size:" << data.count() << "| Content length:" << httpResp.contentLength();
	for (int i = 0; i < data.count(); ++i)
	{
		printf("%c", (char)data.at(i));
	}
	qDebug() << "";
	emit dataReceived();
}

void Http::reqFinished(int id, bool error)
{
	if (error)
		qDebug() << "Request error string:" << qHttp.errorString().toAscii().constData();
}

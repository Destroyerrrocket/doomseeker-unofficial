//------------------------------------------------------------------------------
// http.h
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
#ifndef __HTTP_H_
#define __HTTP_H_

#include <QDebug>
#include <QHttp>
#include <QHttpRequestHeader>
#include <QHttpResponseHeader>
#include <QList>
#include <Qt>
#include <QWaitCondition>

struct Link
{
	QString url;
	QString text;
};

class Http : public QObject
{
	Q_OBJECT

	public:
		Http();
		Http(QString);
		~Http();

		QList<Link>		links();
		void 			sendRequestGet(QString);
		void 			setSite(const QString&);

	signals:
		void dataReceived();

	protected slots:
		void read(const QHttpResponseHeader&);
		void reqFinished(int, bool);

	protected:
		QByteArray				data;
		QHttp 					qHttp;
		QString 				site;
		QWaitCondition			waitCondition;

	private:
		void					construct();
};

#endif

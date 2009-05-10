//------------------------------------------------------------------------------
// protocol.h
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
#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include <QObject>
#include <QUrl>

class Protocol : public QObject
{
	Q_OBJECT

	public:
		Protocol();

		virtual void		abort() {};

		void				get(const QUrl& url);

		const QByteArray&	lastData() const { return data; }
		QUrl				lastLink() const;

	signals:
		void error(const QString&);
		void nameOfCurrentlyDownloadedResource(const QString&);
		void finishedReceiving(const QString& error);
		void notice(const QString&);
		void size(unsigned int);

	protected:
		virtual QString		defaultScheme()=0;
		virtual void 		sendGet()=0;

		QByteArray	data;
		int			port;
		QString		resource;
		QUrl	 	site;
		unsigned 	sizeCur;
		unsigned	sizeMax;
};

#endif

//------------------------------------------------------------------------------
// zdaemonmasterclient.h
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __ZDAEMONMASTERCLIENT_H__
#define __ZDAEMONMASTERCLIENT_H__

#include "masterserver/masterclient.h"

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWaitCondition>

class EnginePlugin;

class ZDaemonMasterClient : public MasterClient
{
	Q_OBJECT

	public:
		ZDaemonMasterClient();

		const EnginePlugin*		plugin() const;
		bool					readMasterResponse(QByteArray &data);
		//void					refresh();

	protected:

		bool					getServerListRequest(QByteArray &data);

		QNetworkAccessManager	*netAccessManager;
		QWaitCondition			listWaitCondition;

		QByteArray				packetBuffer;
		quint32					packetMask;

		//quint32					defaultIP;
		QString					key;

	protected slots:
		void	createQueryRequest();
		void	listFetched(QNetworkReply *reply);

	signals:
		void	request();
};

#endif /* __ZDAEMONMASTERCLIENT_H__ */

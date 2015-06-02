//------------------------------------------------------------------------------
// serversstatuswidget.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __SERVERSSTATUSWIDGET_H__
#define __SERVERSSTATUSWIDGET_H__

#include "serverapi/serverptr.h"
#include <QLabel>

class EnginePlugin;
class MasterClient;
class Server;

class ServersStatusWidget : public QLabel
{
	Q_OBJECT

	public:
		ServersStatusWidget(const EnginePlugin *plugin);

	public slots:
		void deregisterServer(const ServerPtr &server);
		/**
		* @brief Changes the appearance of the widget basing on the boolean
		* value.
		*
		* If set to true, apperance will be clear. If set to false, appearance
		* will be "grayed out".
		*/
		void setMasterEnabledStatus(bool bEnabled);
		void registerServerIfSamePlugin(ServerPtr server);
		void updateDisplay();

	signals:
		void clicked(const EnginePlugin* plugin);

	private:
		void mousePressEvent(QMouseEvent* event);
		void paintEvent(QPaintEvent *event);
		void registerServer(ServerPtr server);

		bool bMasterIsEnabled;
		QPixmap icon;
		QPixmap iconDisabled;
		unsigned int numBots;
		unsigned int numPlayers;
		const EnginePlugin *plugin;
		MasterClient *serverList;

	private slots:
		void addServer(const ServerPtr &server);
		void registerServers();
		void removeServer(const ServerPtr &server);
};

#endif /* __SERVERSSTATUSWIDGET_H__ */

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
class ServerList;

class ServersStatusWidget : public QLabel
{
	Q_OBJECT

	public:
		ServersStatusWidget(const EnginePlugin *plugin, const ServerList *serverList);

	public slots:
		/**
		* @brief Changes the appearance of the widget basing on the boolean
		* value.
		*
		* If set to true, apperance will be clear. If set to false, appearance
		* will be "grayed out".
		*/
		void setMasterEnabledStatus(bool bEnabled);
		void updateDisplay();

	signals:
		void clicked(const EnginePlugin* plugin);

	private:
		void increaseCounters(ServerPtr server);
		void mousePressEvent(QMouseEvent* event);
		void paintEvent(QPaintEvent *event);
		void registerServer(ServerPtr server);
		void resetStatus();

		bool enabled;
		QPixmap icon;
		QPixmap iconDisabled;
		unsigned int numBots;
		unsigned int numPlayers;
		const EnginePlugin *plugin;
		const ServerList *serverList;

	private slots:
		void decreaseCountersAndUpdateDisplay(const ServerPtr &server);
		void increaseCountersAndUpdateDisplay(const ServerPtr &server);
		void deregisterServerIfSamePlugin(const ServerPtr &server);
		void registerServerIfSamePlugin(ServerPtr server);
};

#endif /* __SERVERSSTATUSWIDGET_H__ */

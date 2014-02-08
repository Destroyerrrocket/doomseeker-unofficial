//------------------------------------------------------------------------------
// customservers.cpp
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
#include "customservers.h"
#include "configuration/doomseekerconfig.h"
#include "plugins/engineplugin.h"
#include "serverapi/server.h"
#include "main.h"
#include "log.h"
#include <QHostInfo>

void CustomServers::decodeConfigEntries(const QString& str, QList<CustomServerInfo>& outCustomServerInfoList)
{
	outCustomServerInfoList.clear();

	int openingBracketIndex = 0;
	int closingBracketIndex = 0;
	bool bSeekClosingBracket = false;
	for (int i = 0; i < str.length(); ++i)
	{
		if (!bSeekClosingBracket && str[i] == '(')
		{
			openingBracketIndex = i;
			bSeekClosingBracket = true;
		}
		else if (bSeekClosingBracket && str[i] == ')')
		{
			closingBracketIndex = i;
			bSeekClosingBracket = false;

			QString entry = str.mid(openingBracketIndex + 1, closingBracketIndex - (openingBracketIndex + 1));
			QStringList entryList = entry.split(";");

			if (entryList.size() == 3)
			{
				CustomServerInfo customServerInfo;
				customServerInfo.engine = QUrl::fromPercentEncoding(entryList[0].toAscii());

				int engineIndex = Main::enginePlugins->pluginIndexFromName(customServerInfo.engine);
				customServerInfo.engineIndex = engineIndex;

				customServerInfo.host = QUrl::fromPercentEncoding(entryList[1].toAscii());

				bool ok = false;
				int port = QString(entryList[2]).toInt(&ok);
				if (ok && port >= 1 && port <= 65535)
				{
					customServerInfo.port = port;
				}
				else if (engineIndex >= 0)
				{
					const PluginLoader::Plugin* pPlugin = (*Main::enginePlugins)[engineIndex];
					customServerInfo.port = pPlugin->info()->data()->defaultServerPort;
				}
				else
				{
					customServerInfo.port = 1;
				}

				outCustomServerInfoList << customServerInfo;
			} // end of if
		} // end of else if
	} // end of for
}

void CustomServers::readConfig(QObject* receiver, const char* slotUpdated, const char* slotBegunRefreshing)
{
	QList<CustomServerInfo> customServerInfoList = gConfig.doomseeker.customServers.toList();
	setServers(customServerInfoList, receiver, slotUpdated, slotBegunRefreshing);
}

void CustomServers::setServers(const QList<CustomServerInfo>& csiList, QObject* receiver, const char* slotUpdated, const char* slotBegunRefreshing)
{
	emptyServerList();

	foreach (const CustomServerInfo& customServerInfo, csiList)
	{
		if (customServerInfo.engineIndex < 0)
		{
			// Unknown engine.
			gLog << tr("Unknown game for custom server %1:%2")
				.arg(customServerInfo.host).arg(customServerInfo.port);
			continue;
		}
		
		QHostAddress address;
		if (!address.setAddress(customServerInfo.host))
		{
			QHostInfo hostInfo(QHostInfo::fromName(customServerInfo.host));
			if (hostInfo.addresses().size() == 0)
			{
				// Can't decipher address.
				gLog << tr("Failed to resolve address for custom server %1:%2")
					.arg(customServerInfo.host).arg(customServerInfo.port);
				continue;
			}
			address = hostInfo.addresses().first();
		}

		const EnginePlugin* pInterface = (*Main::enginePlugins)[customServerInfo.engineIndex]->info();
		Server* p = pInterface->server(address, customServerInfo.port);
		if(p == NULL)
		{
			gLog << tr("Plugin returned NULL \"Server*\" for custom server %1:%2. "
				"This is a problem with the plugin.")
					.arg(customServerInfo.host).arg(customServerInfo.port);
			continue;
		}
		p->setCustom(true);

		connect(p, SIGNAL( updated(Server*, int) ), receiver, slotUpdated);
		connect(p, SIGNAL( begunRefreshing(Server*) ), receiver, slotBegunRefreshing);
		servers() << p;
	}
}

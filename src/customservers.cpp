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
#include "main.h"
#include <QHostInfo>

QList<CustomServerInfo>* CustomServers::decodeConfigEntries(const QString& str)
{
	QList<CustomServerInfo>* csiList = new QList<CustomServerInfo>();

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
				CustomServerInfo csi; // CSI: Miami
				csi.engine = QUrl::fromPercentEncoding(entryList[0].toAscii());
				csi.engineIndex = Main::enginePlugins->pluginIndexFromName(csi.engine);

                csi.host = QUrl::fromPercentEncoding(entryList[1].toAscii());

                bool ok = false;
                int port = QString(entryList[2]).toInt(&ok);
                if (ok && port >= 1 && port <= 65535)
                    csi.port = port;
                else if (csi.engineIndex >= 0)
                    csi.port = (*Main::enginePlugins)[csi.engineIndex]->info->pInterface->defaultServerPort();
                else
                    csi.port = 1;

                csiList->append(csi);
			} // end of if
		} // end of else if
	} // end of for

	return csiList;
}

void CustomServers::readConfig(Config* cfg, QObject* receiver, const char* slotUpdated, const char* slotBegunRefreshing)
{
	if (cfg == NULL)
		return;

	SettingsData* setting = cfg->setting("CustomServers");
	QList<CustomServerInfo>* csiList = decodeConfigEntries(setting->string());

	setServers(*csiList, receiver, slotUpdated, slotBegunRefreshing);

	delete csiList;
}

void CustomServers::setServers(const QList<CustomServerInfo>& csiList, QObject* receiver, const char* slotUpdated, const char* slotBegunRefreshing)
{
	emptyServerList();

	QList<CustomServerInfo>::const_iterator cit;
	for (cit = csiList.begin(); cit != csiList.end(); ++cit)
	{
		if (cit->engineIndex < 0)
			continue;

		QHostInfo hi(QHostInfo::fromName(cit->host));
		if (hi.addresses().size() == 0)
			continue;

		const EnginePlugin* pInterface = (*Main::enginePlugins)[cit->engineIndex]->info->pInterface;
		Server* p = pInterface->server(hi.addresses().first(), cit->port);
		if(p == NULL)
			continue;
		p->setCustom(true);

		connect(p, SIGNAL( updated(Server*, int) ), receiver, slotUpdated);
		connect(p, SIGNAL( begunRefreshing(Server*) ), receiver, slotBegunRefreshing);
		servers.append(p);
	}
}

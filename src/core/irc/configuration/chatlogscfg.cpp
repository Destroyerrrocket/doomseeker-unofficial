//------------------------------------------------------------------------------
// chatlogscfg.cpp
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "chatlogscfg.h"

#include "irc/configuration/ircconfig.h"
#include "datapaths.h"

class ChatLogsCfg::PrivData
{
public:
};


ChatLogsCfg::ChatLogsCfg()
{
	d = new PrivData();
}

ChatLogsCfg::~ChatLogsCfg()
{
	delete d;
}

QString ChatLogsCfg::chatLogsRootDir() const
{
	return value("ChatLogsRootDir", gDefaultDataPaths->localDataLocationPath(
		DataPaths::CHATLOGS_DIR_NAME)).toString();
}

void ChatLogsCfg::setChatLogsRootDir(const QString &val)
{
	setValue("ChatLogsRootDir", val);
}

bool ChatLogsCfg::isStoreLogs() const
{
	return value("StoreLogs", true).toBool();
}

void ChatLogsCfg::setStoreLogs(bool b)
{
	setValue("StoreLogs", b);
}

bool ChatLogsCfg::isRestoreChatFromLogs() const
{
	return value("RestoreChatFromLogs", true).toBool();
}

void ChatLogsCfg::setRestoreChatFromLogs(bool b)
{
	setValue("RestoreChatFromLogs", b);
}

void ChatLogsCfg::setValue(const QString &key, const QVariant &value)
{
	gIRCConfig.ini()->section("Logs").setValue(key, value);
}

QVariant ChatLogsCfg::value(const QString &key, const QVariant &defValue) const
{
	return gIRCConfig.ini()->section("Logs").value(key, defValue);
}

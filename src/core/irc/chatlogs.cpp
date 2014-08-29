//------------------------------------------------------------------------------
// chatlogs.cpp
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
#include "chatlogs.h"

#include "irc/configuration/chatlogscfg.h"
#include "irc/entities/ircnetworkentity.h"
#include <QDir>

class ChatLogs::PrivData
{
public:
	QString rootPath() const
	{
		return ChatLogsCfg().chatLogsRootDir();
	}
};


ChatLogs::ChatLogs()
{
	d = new PrivData();
}

ChatLogs::~ChatLogs()
{
	delete d;
}

QString ChatLogs::logFilePath(const IRCNetworkEntity &entity, const QString &recipient) const
{
	QString fileName = recipient;
	if (recipient.trimmed().isEmpty())
	{
		fileName = "@main";
	}
	return QString("%1/%2.txt").arg(networkDirPath(entity), fileName);
}

bool ChatLogs::mkLogDir(const IRCNetworkEntity &entity)
{
	QDir dir(networkDirPath(entity));
	return dir.mkpath(".");
}

QString ChatLogs::networkDirPath(const IRCNetworkEntity &entity) const
{
	return QString("%1/%2").arg(d->rootPath(), entity.description());
}

//------------------------------------------------------------------------------
// chatlogrotate.cpp
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
#include "chatlogrotate.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include "irc/entities/ircnetworkentity.h"
#include "irc/chatlogarchive.h"
#include "irc/chatlogs.h"
#include "log.h"


class ChatLogRotate::PrivData
{
public:
	int maxSize;
	int removalAgeDaysThreshold;
};


ChatLogRotate::ChatLogRotate()
{
	d = new PrivData();
	d->removalAgeDaysThreshold = -1;
	d->maxSize = 5 * 1024 * 1024;
}

ChatLogRotate::~ChatLogRotate()
{
	delete d;
}

void ChatLogRotate::setMaxSize(int size)
{
	d->maxSize = size;
}

void ChatLogRotate::setRemovalAgeDaysThreshold(int age)
{
	d->removalAgeDaysThreshold = age;
}

void ChatLogRotate::rotate(const IRCNetworkEntity &network, const QString &recipient)
{
	archivizeCurrent(network, recipient);
	purgeOld(network, recipient);
}

void ChatLogRotate::archivizeCurrent(const IRCNetworkEntity &network, const QString &recipient)
{
	if (d->maxSize <= 0)
	{
		return;
	}
	QFile file(ChatLogs().logFilePath(network, recipient));
	qint64 size = file.size();
	if (size > d->maxSize)
	{
		mkBackupDir(network, recipient);
		QString newName = ChatLogArchive::mkArchiveFilePath(network, recipient);
		gLog << QString("IRC: Archiving log to file '%1'").arg(newName);
		file.rename(newName);
	}
}

void ChatLogRotate::purgeOld(const IRCNetworkEntity &network, const QString &recipient)
{
	if (d->removalAgeDaysThreshold < 0)
	{
		return;
	}
	QString dirPath = ChatLogArchive::archiveDirPath(network, recipient);
	foreach (const QString &entry, ChatLogArchive::listArchivedLogsSortedByTime(network, recipient))
	{
		QString entryPath = QString("%1/%2").arg(dirPath, entry);
		if (isEligibleForRemoval(entryPath))
		{
			gLog << QString("IRC: Removed archived log file '%1'").arg(entryPath);;
			QFile file(entryPath);
			file.remove();
		}
	}
}

bool ChatLogRotate::isEligibleForRemoval(const QFileInfo &entry) const
{
	return entry.isFile() && entry.lastModified().daysTo(QDateTime::currentDateTime()) > d->removalAgeDaysThreshold;
}

void ChatLogRotate::mkBackupDir(const IRCNetworkEntity &network, const QString &recipient)
{
	QDir().mkpath(ChatLogArchive::archiveDirPath(network, recipient));
}

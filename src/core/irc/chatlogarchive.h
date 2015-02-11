//------------------------------------------------------------------------------
// chatlogarchive.h
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
#ifndef idccf54adf_3557_4e8c_bde9_b6fc5469a592
#define idccf54adf_3557_4e8c_bde9_b6fc5469a592

#include "dptr.h"

#include <QString>

class IRCNetworkEntity;

class ChatLogArchive
{
public:
	ChatLogArchive();
	~ChatLogArchive();

	static QString archiveDirPath(const IRCNetworkEntity &network, const QString &recipient);
	static QStringList listArchivedLogsSortedByTime(const IRCNetworkEntity &network, const QString &recipient);
	static QString mkArchiveFilePath(const IRCNetworkEntity &network, const QString &recipient);

private:
	DPtr<ChatLogArchive> d;
};

#endif

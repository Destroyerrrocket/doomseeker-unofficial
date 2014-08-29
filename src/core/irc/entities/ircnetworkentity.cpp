//------------------------------------------------------------------------------
// ircnetworkentity.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ircnetworkentity.h"

#include "irc/chatnetworknamer.h"

class IRCNetworkEntity::PrivData
{
	public:
		QString address;
		QStringList autojoinChannels;
		QStringList autojoinCommands;
		bool bAutojoinNetwork;
		QString description;
		QString nickservCommand;
		QString nickservPassword;
		QString password;
		unsigned short port;
};

COPYABLE_D_POINTERED_DEFINE(IRCNetworkEntity);

IRCNetworkEntity::IRCNetworkEntity()
{
	d = new PrivData();
	d->bAutojoinNetwork = false;
	d->port = 6667;
	d->nickservCommand = "/msg nickserv identify %1";
}

IRCNetworkEntity::~IRCNetworkEntity()
{
	delete d;
}

const QString &IRCNetworkEntity::address() const
{
	return d->address;
}

const QStringList &IRCNetworkEntity::autojoinChannels() const
{
	return d->autojoinChannels;
}

QStringList &IRCNetworkEntity::autojoinChannels()
{
	return d->autojoinChannels;
}

const QStringList &IRCNetworkEntity::autojoinCommands() const
{
	return d->autojoinCommands;
}

QStringList &IRCNetworkEntity::autojoinCommands()
{
	return d->autojoinCommands;
}

bool IRCNetworkEntity::isAutojoinNetwork() const
{
	return d->bAutojoinNetwork;;
}

const QString &IRCNetworkEntity::description() const
{
	return d->description;
}

const QString &IRCNetworkEntity::nickservCommand() const
{
	return d->nickservCommand;
}

const QString &IRCNetworkEntity::nickservPassword() const
{
	return d->nickservPassword;
}

const QString &IRCNetworkEntity::password() const
{
	return d->password;
}

unsigned short IRCNetworkEntity::port() const
{
	return d->port;
}


void IRCNetworkEntity::setAddress(const QString &v)
{
	d->address = v;
}

void IRCNetworkEntity::setAutojoinChannels(const QStringList &v)
{
	d->autojoinChannels = v;
}

void IRCNetworkEntity::setAutojoinCommands(const QStringList &v)
{
	d->autojoinCommands = v;
}

void IRCNetworkEntity::setAutojoinNetwork(bool v)
{
	d->bAutojoinNetwork = v;
}

void IRCNetworkEntity::setDescription(const QString &v)
{
	d->description = ChatNetworkNamer::convertToValidName(v);
}

void IRCNetworkEntity::setNickservCommand(const QString &v)
{
	d->nickservCommand = v;
}

void IRCNetworkEntity::setNickservPassword(const QString &v)
{
	d->nickservPassword = v;
}

void IRCNetworkEntity::setPassword(const QString &v)
{
	d->password = v;
}

void IRCNetworkEntity::setPort(unsigned short v)
{
	d->port = v;
}


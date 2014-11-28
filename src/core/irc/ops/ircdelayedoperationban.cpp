//------------------------------------------------------------------------------
// ircdelayedoperationban.cpp
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
#include "ircdelayedoperationban.h"

#include "irc/entities/ircuserprefix.h"
#include "irc/ircresponseparser.h"
#include "irc/ircnetworkadapter.h"

class IRCDelayedOperationBan::PrivData
{
public:
	QString channel;
	QString nickname;
	QString reason;
	IRCNetworkAdapter *network;

	QString cleanNickname() const
	{
		return network->userPrefixes().cleanNickname(nickname);
	}
};


IRCDelayedOperationBan::IRCDelayedOperationBan(IRCNetworkAdapter *network,
	const QString &channel, const QString &nickname, QObject *parent)
: IRCDelayedOperation(parent)
{
	d = new PrivData();
	d->channel = channel;
	d->nickname = nickname;
	d->network = network;
	this->connect(d->network->responseParser(),
		SIGNAL(whoIsUser(QString, QString, QString, QString)),
		SLOT(onWhoIsUser(QString, QString, QString, QString)));
}

IRCDelayedOperationBan::~IRCDelayedOperationBan()
{
	delete d;
}

void IRCDelayedOperationBan::start()
{
	d->network->sendMessage(QString("/whois %1").arg(d->cleanNickname()));
}

void IRCDelayedOperationBan::onWhoIsUser(const QString& nickname, const QString& user,
	const QString& hostName, const QString& realName)
{
	QString banString = "*!*@" + hostName;
	d->network->sendMessage(QString("/mode %1 +b %2").arg(d->channel, banString));
	d->network->sendMessage(QString("/kick %1 %2 %3").arg(d->channel, d->nickname, d->reason));
	this->deleteLater();
}

void IRCDelayedOperationBan::setReason(const QString &reason)
{
	d->reason = reason;
}

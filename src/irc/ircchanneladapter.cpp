//------------------------------------------------------------------------------
// ircchanneladapter.cpp
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
#include "ircchanneladapter.h"
#include "irc/ircglobal.h"
#include "irc/ircuserinfo.h"
#include "irc/ircuserlist.h"

IRCChannelAdapter::IRCChannelAdapter(IRCNetworkAdapter* pNetwork, const QString& recipient)
: IRCChatAdapter(pNetwork, recipient)
{
	users = new IRCUserList();
}

IRCChannelAdapter::~IRCChannelAdapter()
{
	delete users;
}

void IRCChannelAdapter::appendNameToCachedList(const QString& name)
{
	if (users->appendNameToCachedList(name))
	{
		IRCUserInfo user = users->userCopy(name);
		emit nameAdded(user);
	}
}

void IRCChannelAdapter::appendNamesToCachedList(const QStringList& names)
{
	users->appendNamesToCachedList(names);
}

void IRCChannelAdapter::emitCachedNameListUpdated()
{
	emit nameListUpdated(*users);
}

void IRCChannelAdapter::emitChatMessage(const QString& sender, const QString& content)
{
	// Ensure that all nickname artifacts are preserved.
	const IRCUserInfo* pUserInfo = users->user(sender);
	
	QString actualSenderName = sender;
	if (pUserInfo != NULL)
	{
		actualSenderName = pUserInfo->prefixedName();
	}
	
	IRCChatAdapter::emitChatMessage(actualSenderName, content);
}

bool IRCChannelAdapter::hasUser(const QString& nickname)
{
	return users->hasUser(nickname);
}

void IRCChannelAdapter::removeNameFromCachedList(const QString& name)
{
	IRCUserInfo user = users->userCopy(name);

	if (!users->removeNameFromCachedList(name))
	{
		emit error(QString("Attempted to remove name \"%1\" from the \"%2\" channel's name list but no such name is on the list.").arg(name, this->recipientName));
	}
	else
	{
		emit nameRemoved(user);
	}
}

void IRCChannelAdapter::userChangesNickname(const QString& oldNickname, const QString& newNickname)
{
	printf("IRCChannelAdapter::userChangesNickname()\n");

	if (hasUser(oldNickname))
	{
		IRCUserInfo oldName = users->userCopy(oldNickname);
	
		users->changeNick(oldNickname, newNickname);
		emit nameRemoved(oldName);
		emit nameAdded(users->userCopy(newNickname));

		emit messageColored(tr("%1 is now known as %2").arg(oldNickname, newNickname), 
			IRCGlobal::COLOR_CHANNEL_ACTION);
	}
}

void IRCChannelAdapter::userJoins(const QString& nickname, const QString& fullSignature)
{
	printf("IRCChannelAdapter::userJoins()\n");

	appendNameToCachedList(nickname);

	emit messageColored(tr("User %1 [%2] has joined the channel.").arg(nickname, fullSignature), 
		IRCGlobal::COLOR_CHANNEL_ACTION);
}

void IRCChannelAdapter::userLeaves(const QString& nickname, const QString& farewellMessage, IRCQuitType quitType)
{
	printf("IRCChannelAdapter::userLeaves()\n");

	if (!hasUser(nickname))
	{
		// Nothing to do here. This user was not even on the channel.
		return;
	}

	removeNameFromCachedList(nickname);

	switch (quitType)
	{
		case IRCChatAdapter::ChannelPart:
			emit messageColored(tr("User %1 has left the channel. (PART: %2)").arg(nickname, farewellMessage), 
				IRCGlobal::COLOR_CHANNEL_ACTION);
			break;
			
		case IRCChatAdapter::NetworkKill:
			emit messageColored(tr("Connection for user %1 has been killed. (KILL: %2)").arg(nickname, farewellMessage), 
				IRCGlobal::COLOR_NETWORK_ACTION);
			break;

		case IRCChatAdapter::NetworkQuit:
			emit messageColored(tr("User %1 has quit the network. (QUIT: %2)").arg(nickname, farewellMessage), 
				IRCGlobal::COLOR_NETWORK_ACTION);
			break;

		default:
			emit error(tr("Unknown quit type from user %1.").arg(nickname));
			break;
	}
}

void IRCChannelAdapter::userModeChanges(const QString& nickname, unsigned flagsAdded, unsigned flagsRemoved)
{
	const IRCUserInfo* pUserInfo = this->users->user(nickname);
	IRCUserInfo oldInfoCopy = *pUserInfo;
	if (pUserInfo != NULL)
	{
		unsigned flags = pUserInfo->flags();
		flags |= flagsAdded;
		flags &= ~flagsRemoved;
	
		this->users->setUserFlags(nickname, flags);
		
		emit nameRemoved(oldInfoCopy);
		emit nameAdded(this->users->userCopy(nickname));
	}
}

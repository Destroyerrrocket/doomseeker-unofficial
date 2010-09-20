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

IRCChannelAdapter::IRCChannelAdapter(IRCNetworkAdapter* pNetwork, const QString& recipient)
: IRCChatAdapter(pNetwork, recipient)
{
}

bool IRCChannelAdapter::addName(const QString& name)
{
	if (cachedNames.isEmpty())
	{
		cachedNames.append(name);
	}

	if (hasUser(name))
	{
		printf("HasNotUser: %s\n", name.toAscii().constData());
		return false;
	}

	// Lower-case both names to avoid crazy Penguin name sorting.
	QString nameLower = name.toLower();
	for (int i = 0; i < cachedNames.size(); ++i)
	{
		const QString& cachedName = cachedNames[i];
		QString cachedNameLower = cachedName.toLower();
		if (nameLower <= cachedNameLower )
		{
			cachedNames.insert(i, name);
			return true;
		}
	}

	// This name should be sorted last.
	cachedNames.append(name);
	return true;
}

void IRCChannelAdapter::appendNameToCachedList(const QString& name)
{
	if (addName(name))
	{
		emit nameAdded(name);
	}
}

void IRCChannelAdapter::appendNamesToCachedList(const QStringList& names)
{
	foreach (const QString& name, names)
	{
		addName(name);
	}
}

void IRCChannelAdapter::emitCachedNameListUpdated()
{
	emit nameListUpdated(cachedNames);
}

bool IRCChannelAdapter::hasUser(const QString& nickname)
{
	return cachedNames.contains(nickname, Qt::CaseInsensitive);
}

void IRCChannelAdapter::removeNameFromCachedList(const QString& name)
{
	QRegExp regExp(name, Qt::CaseInsensitive, QRegExp::FixedString);
	int index = cachedNames.indexOf(name);
	if (index == -1)
	{
		emit error(QString("Attempted to remove name \"%1\" from the \"%2\" channel's name list but no such name is on the list.").arg(name, this->recipientName));
		return;
	}

	cachedNames.removeAt(index);
	emit nameRemoved(name);
}

void IRCChannelAdapter::userChangesNickname(const QString& oldNickname, const QString& newNickname)
{
	printf("IRCChannelAdapter::userChangesNickname()\n");

	if (hasUser(oldNickname))
	{
		removeNameFromCachedList(oldNickname);
		appendNameToCachedList(newNickname);

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

		case IRCChatAdapter::NetworkQuit:
			emit messageColored(tr("User %1 has quit the network. (QUIT: %2)").arg(nickname, farewellMessage), 
				IRCGlobal::COLOR_NETWORK_ACTION);
			break;

		default:
			emit error(tr("Unknown quit type from user %1.").arg(nickname));
			break;
	}
}


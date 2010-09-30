//------------------------------------------------------------------------------
// ircuserlist.cpp
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
#include "ircuserlist.h"
#include "irc/ircuserinfo.h"

IRCUserList::~IRCUserList()
{
	for (int i = 0; i < usersArray.size(); ++i)
	{
		delete usersArray[i];
	}
}

bool IRCUserList::appendNameToCachedList(const QString& nickname)
{
	IRCUserInfo userInfo(nickname);
	return appendNameToCachedList(userInfo);
}

bool IRCUserList::appendNameToCachedList(const IRCUserInfo& userInfo)
{
	int index = this->indexOfName(userInfo.cleanNickname());
	if (index >= 0)
	{
		*usersArray[index] = userInfo;
		return false;
	}
	else
	{
		IRCUserInfo* pUserInfo = new IRCUserInfo(userInfo);
		usersArray.append(pUserInfo);
		return true;
	}
}

void IRCUserList::appendNamesToCachedList(const QStringList& names)
{
	foreach (const QString& name, names)
	{
		appendNameToCachedList(name);
	}
}

bool IRCUserList::changeNick(const QString& oldNickname, const QString& newNickname)
{
	const IRCUserInfo* pExistingInfo = user(oldNickname);
	if (pExistingInfo == NULL)
	{
		return false;
	}
	
	unsigned flags = pExistingInfo->flags();

	removeNameFromCachedList(oldNickname);
	
	IRCUserInfo newUserInfo(newNickname);
	newUserInfo.setFlags(flags);
	
	appendNameToCachedList(newUserInfo.prefixedName());
	return true;
}

bool IRCUserList::hasUser(const QString& nickname) const
{
	return (indexOfName(nickname) != -1);
}

int	IRCUserList::indexOfName(const QString& nickname) const
{
	for (int i = 0; i < usersArray.size(); ++i)
	{
		const IRCUserInfo& storedUser = *usersArray[i];
		if (storedUser == nickname)
		{
			return i;
		}
	}

	return -1;
}

bool IRCUserList::removeNameFromCachedList(const QString& nickname)
{
	int index = indexOfName(nickname);
	if (index < 0)
	{
		return false;
	}

	delete usersArray[index];
	usersArray.remove(index);
	return true;
}

void IRCUserList::setUserFlags(const QString& nickname, unsigned flags)
{
	int index = this->indexOfName(nickname);
	if (index >= 0)
	{
		IRCUserInfo& userInfo = *usersArray[index];
		userInfo.setFlags(flags);
	}
}

QStringList IRCUserList::toStringList() const
{
	QStringList nicksList;
	for (int i = 0; i < usersArray.size(); ++i)
	{
		nicksList << usersArray[i]->prefixedName();
	}

	return nicksList;
}

const IRCUserInfo* IRCUserList::user(const QString& nickname) const
{
	int index = this->indexOfName(nickname);
	if (index < 0)
	{
		return NULL;
	}
	
	return usersArray[index];
}

IRCUserInfo IRCUserList::userCopy(const QString& nickname) const
{
	const IRCUserInfo* pUserInfo = user(nickname);
	if (pUserInfo == NULL)
	{
		return IRCUserInfo("");
	}
	
	return *pUserInfo;
}

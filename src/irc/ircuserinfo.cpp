//------------------------------------------------------------------------------
// ircuserinfo.cpp
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
#include "ircuserinfo.h"
#include "irc/ircglobal.h"

IRCUserInfo::IRCUserInfo(const QString& nickname, const QString& fullSignature)
{
	this->userFlags = 0;
	this->fullSignature = fullSignature;

	if (nickname.isEmpty())
	{
		return;
	}

	this->userName = nickname;
	this->userFlags = this->extractFlags(this->userName);
}

unsigned IRCUserInfo::convertModeCharToFlag(char c)
{
	switch (c)
	{
		case 'o':
			return FLAG_OP;
			
		case 'v':
			return FLAG_VOICE;
			
		default:
			return 0;
	}
}

unsigned IRCUserInfo::convertNickCharToFlag(char c)
{
	switch (c)
	{
		case '@':
			return FLAG_OP;
			
		case '+':
			return FLAG_VOICE;
			
		default:
			return 0;
	}
}

QString IRCUserInfo::cleanNicknameLowerCase() const
{
	return IRCGlobal::toIrcLower(this->cleanNickname());
}

unsigned IRCUserInfo::extractFlags(QString& name)
{
	// This is a recurrent method.
	// Recurrency continues until zero is returned.

	if (name.isEmpty())
	{
		return 0;
	}

	unsigned flag = convertNickCharToFlag(name[0].toAscii());
	if (flag != 0)
	{
		name.remove(0, 1);
		// Call the function again to seek more flags.
		return flag | extractFlags(name);
	}
	
	return flag;
}

QString IRCUserInfo::extractHostnameFromFullSignature() const
{
	if (!this->fullSignature.isEmpty())
	{
		int indexOfDelimiterChar = this->fullSignature.indexOf('@');
		QString hostName = this->fullSignature;
		hostName.remove(0, indexOfDelimiterChar + 1);
		
		return hostName;
	}
	
	return "";
}

bool IRCUserInfo::isFlag(unsigned flag) const
{
	unsigned combined = flag & userFlags;
	return combined != 0;
}

bool IRCUserInfo::isSameNickname(const IRCUserInfo& otherUser) const
{
	return ((*this) == otherUser);
}

bool IRCUserInfo::operator==(const IRCUserInfo& otherUser) const
{
	QString thisNickname = this->cleanNicknameLowerCase();
	QString otherNickname = otherUser.cleanNicknameLowerCase();

	return (thisNickname.compare(otherNickname) == 0);
}

bool IRCUserInfo::operator<=(const IRCUserInfo& otherUser) const
{
	if (this->isOp() ^ otherUser.isOp())
	{
		return this->isOp();
	}
	// Op overrides voice.
	else if (this->isVoiced() ^ otherUser.isVoiced())
	{
		return this->isVoiced();
	}
	
	QString thisNickname = this->cleanNicknameLowerCase();
	QString otherNickname = otherUser.cleanNicknameLowerCase();
	
	bool bIsThisAlphabeticallySmaller = (thisNickname.compare(otherNickname) <= 0);
	
	return bIsThisAlphabeticallySmaller;
}

QString IRCUserInfo::prefixedName() const 
{
	QString userName = this->userName;

	if (this->isOp())
	{
		userName.prepend('@');
		return userName;
	}
	
	if (this->isVoiced())
	{
		userName.prepend('+');
		return userName;
	}	

	return userName;
}

QString IRCUserInfo::prefixedNameLowerCase() const
{
	return IRCGlobal::toIrcLower(this->prefixedName());
}

void IRCUserInfo::setFlag(unsigned flag, bool bSet)
{
	if (bSet)
	{
		this->userFlags |= flag;
	}
	else
	{
		this->userFlags &= ~flag;
	}
}

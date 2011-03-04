//------------------------------------------------------------------------------
// ircdelayedoperation.cpp
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
#include "ircdelayedoperation.h"
#include "irc/ircuserinfo.h"

IRCDelayedOperation::IRCDelayedOperation(OperationType operationType, const QString& nickname, const QString& channel)
{
	this->channelName = channel;
	this->nick = IRCUserInfo(nickname).cleanNickname();
	this->operType = operationType;
}

bool IRCDelayedOperation::operator==(const IRCDelayedOperation& other) const
{
	IRCUserInfo userInfo(this->nick);

	return this->operType == other.operType
		&& (this->channelName.compare(other.channelName, Qt::CaseInsensitive) == 0)
		&& userInfo == other.nick;
		
}


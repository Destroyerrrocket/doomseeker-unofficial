//------------------------------------------------------------------------------
// ircdelayedoperationlist.cpp
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
#include "ircdelayedoperationlist.h"
#include "irc/ircuserinfo.h"

IRCDelayedOperationList::~IRCDelayedOperationList()
{
	clear();
}

void IRCDelayedOperationList::add(const IRCDelayedOperation& operation)
{
	operationsArray << new IRCDelayedOperation(operation);
}

void IRCDelayedOperationList::clear()
{
	for (int i = 0; i < operationsArray.size(); ++i)
	{
		delete operationsArray[i];
	}

	operationsArray.clear();
}

const IRCDelayedOperation* IRCDelayedOperationList::operationForChannel(IRCDelayedOperation::OperationType operationType, const QString& channel) const
{
	for (int i = 0; i < operationsArray.size(); ++i)
	{
		const IRCDelayedOperation& operation = *operationsArray[i];
		if (operationType == operation.operationType()
		&& (channel.compare(operation.channel(), Qt::CaseInsensitive) == 0))
		{
			return &operation;
		}
	}

	return NULL;
}

const IRCDelayedOperation* IRCDelayedOperationList::operation(const IRCDelayedOperation& operation) const
{
	for (int i = 0; i < operationsArray.size(); ++i)
	{
		if (*operationsArray[i] == operation)
		{
			return operationsArray[i];
		}
	}

	return NULL;
}

const IRCDelayedOperation* IRCDelayedOperationList::operationForNickname(
	IRCDelayedOperation::OperationType operationType, const QString& nickname) const
{
	for (int i = 0; i < operationsArray.size(); ++i)
	{
		const IRCDelayedOperation& operation = *operationsArray[i];
		if (operationType == operation.operationType()
		&&  nickname == operation.nickname())
		{
			return &operation;
		}
	}

	return NULL;
}

void IRCDelayedOperationList::remove(const IRCDelayedOperation* pointer)
{
	for (int i = 0; i < operationsArray.size(); ++i)
	{
		if (operationsArray[i] == pointer)
		{
			delete operationsArray[i];
			operationsArray.remove(i);
			return;
		}
	}
}

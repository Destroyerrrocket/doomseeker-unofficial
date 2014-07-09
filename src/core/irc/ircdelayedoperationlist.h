//------------------------------------------------------------------------------
// ircdelayedoperationlist.h
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
#ifndef __IRCDELAYEDOPERATIONLIST_H__
#define __IRCDELAYEDOPERATIONLIST_H__

#include "irc/ircdelayedoperation.h"
#include <QVector>

/**
 *	@brief Container making operations on IRCDelayedOperation objects more
 *	convenient.
 */
class IRCDelayedOperationList
{
	public:
		~IRCDelayedOperationList();

		void add(const IRCDelayedOperation& operation);
		void clear();
		bool hasOperation(const IRCDelayedOperation& operationPattern) const
		{
			return operation(operationPattern) != NULL;
		}

		const IRCDelayedOperation* operationForChannel(IRCDelayedOperation::OperationType operationType, const QString& channel) const;

		/**
		 *	@brief Extracts operation that equals the passed object.
		 */
		const IRCDelayedOperation* operation(const IRCDelayedOperation& operation) const;

		const IRCDelayedOperation* operationForNickname(IRCDelayedOperation::OperationType operationType, const QString& nickname) const;

		IRCDelayedOperationList& operator<<(const IRCDelayedOperation& operation)
		{
			this->add(operation);
			return *this;
		}

		/**
		 *	@brief Removes operation from the list.
		 *
		 *	@param pointer
		 *		This must point to a valid object from within this list.
		 *		Such pointer can be obtained through one of the operation()
		 *		methods.
		 */
		void remove(const IRCDelayedOperation* pointer);

	private:
		QVector<IRCDelayedOperation*> operationsArray;
};

#endif

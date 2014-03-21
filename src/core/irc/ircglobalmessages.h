//------------------------------------------------------------------------------
// ircglobalmessages.h
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
#ifndef __IRCGLOBALMESSAGES_H__
#define __IRCGLOBALMESSAGES_H__

#include "irc/ircglobal.h"
#include <QObject>

class IRCAdapterBase;
class IRCMessageClass;

/**
 *	@brief Singleton designed to deliver messages to whatever chat tab
 *	is currently opened.
 */
class IRCGlobalMessages : public QObject
{
	Q_OBJECT

	public:
		static IRCGlobalMessages& instance()
		{
			if (pInstance == NULL)
			{
				pInstance = new IRCGlobalMessages();
			}
			
			return *pInstance;
		}
		
		void emitError(const QString& message, IRCAdapterBase* pMessageSender = NULL);		
		void emitMessage(const QString& message, IRCAdapterBase* pMessageSender = NULL)
		{
			emit this->message(message, pMessageSender);
		}
		
		void emitMessageWithClass(const QString& message, const IRCMessageClass& messageClass, IRCAdapterBase* pMessageSender = NULL)
		{
			emit this->messageWithClass(message, messageClass, pMessageSender);
		}
		
	signals:
		void message(const QString& message, IRCAdapterBase* pMessageSender);
		void messageWithClass(const QString& message, const IRCMessageClass& messageClass, IRCAdapterBase* pMessageSender);	
	
	private:
		static IRCGlobalMessages* pInstance;

		IRCGlobalMessages()
		{
		}
};

#endif

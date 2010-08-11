//------------------------------------------------------------------------------
// ircchatadapter.h
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
#ifndef __IRCCHATADAPTER_H__
#define __IRCCHATADAPTER_H__

#include "irc/ircadapterbase.h"
#include <QStringList>

class IRCNetworkAdapter;

/**
 *	@brief Handles chatting through IRC.
 *
 *	This class covers both channel and private messaging. This can be done
 *	because the IRC protocol as stated in RFC 1459 doesn't see a difference
 *	between transmitting messages between channel and user or between user and
 *	user directly.
 */
class IRCChatAdapter : public IRCAdapterBase
{
	public:
		/**
		 *	@param pNetwork
		 *		A pointer to the network this channel or user exists on.
		 */
		IRCChatAdapter(IRCNetworkAdapter* pNetwork);
		
		/**
		 *	All messages starting with '/' character will be passed to the
		 *	parent network directly. Also the pOrigin parameter for the parent
		 *	network will be set to point to 'this'.
		 *	
		 *	All other messages will be sent as:
		 *	@code
		 *	/PRIVMSG <recipient> :<message>
		 *	@endcode
		 */
		void					doSendMessage(const QString& message, IRCAdapterBase* pOrigin);				
		
	protected:
		IRCNetworkAdapter*		pNetwork;
		QString					recipient;
		
		/**
		 *	Makes sure the sent message will not exceed the IRC character limit.
		 *
		 *	@param words
		 *		A list of words from which a "sentence" will be build. This
		 *		list is directly modified and only unused words are left when
		 *		this method is done processing.
		 */
		QString					extractMessageLine(QStringList& words, int maxLength);
		
		void					sendChatMessage(const QString& message);
};


#endif
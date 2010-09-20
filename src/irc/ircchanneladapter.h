//------------------------------------------------------------------------------
// ircchanneladapter.h
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
#ifndef __IRCCHANNELADAPTER_H__
#define __IRCCHANNELADAPTER_H__

#include <QStringList>
#include "irc/ircchatadapter.h"

/**
 *	@brief Class type that is used for conversations within a channel.
 */
class IRCChannelAdapter : public IRCChatAdapter
{
	Q_OBJECT

	public:
		IRCChannelAdapter(IRCNetworkAdapter* pNetwork, const QString& recipient);

		AdapterType				adapterType() const { return ChannelAdapter; }

		/**
		 *	@brief Appends a single name to the sorted cachedNames list.
		 *
		 *	Ensures that no duplicate names are found on the lsit. 
		 *	This will emit nameAdded() signal. 
		 */
		void					appendNameToCachedList(const QString& name);

		/**
		 *	@brief Appends a list of names to cachedNames list. This ensures that no
		 *	duplicate names are found on the list.
		 */
		void					appendNamesToCachedList(const QStringList& names);

		/**
		 *	@brief Emits cached list of names. This should be called
		 *	when end of names list message is received for this channel.
		 */
		void					emitCachedNameListUpdated();

		bool					hasUser(const QString& nickname);

		/**
		 *	@brief Removes a name from the sortecd cachedNames list.
		 *
		 *	This will emit nameRemoved() signal.
		 */
		void					removeNameFromCachedList(const QString& name);

		void					userChangesNickname(const QString& oldNickname, const QString& newNickname);
		void					userJoins(const QString& nickname, const QString& fullSignature);
		void					userLeaves(const QString& nickname, const QString& farewellMessage, IRCQuitType quitType);

	signals:
		void					nameAdded(const QString& name);
		void					nameListUpdated(const QStringList& names);
		void					nameRemoved(const QString& name);
	
	private:
		QStringList				cachedNames;

		/**
		 *	@brief Adds a name to the cachedNames list.
		 *
		 *	This ensures that there are no duplicate names and that
		 *	the list remains sorted.
		 *
		 *	@return True if a name was actually added. False otherwise.
		 */
		bool					addName(const QString& name);
};

#endif

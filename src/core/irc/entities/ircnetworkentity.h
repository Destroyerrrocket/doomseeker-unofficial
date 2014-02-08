//------------------------------------------------------------------------------
// ircnetworkentity.h
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
#ifndef __IRCNETWORKENTITY_H__
#define __IRCNETWORKENTITY_H__

#include "global.h"
#include <QString>
#include <QStringList>

/**
 * @brief Data structure that describes and defines a connection to an IRC
 * network or server.
 */
class MAIN_EXPORT IRCNetworkEntity
{
	public:
		IRCNetworkEntity();
		COPYABLE_D_POINTERED_DECLARE(IRCNetworkEntity);
		virtual ~IRCNetworkEntity();

		/**
		 * @brief Address of the server or network to connect to.
		 */
		const QString &address() const;

		/**
		 * @brief List of channels to which a /join command will be issued
		 * automatically when a connection with this network is established.
		 */
		const QStringList &autojoinChannels() const;
		QStringList &autojoinChannels();
		/**
		 * @brief List of commands executed on network join.
		 */
		const QStringList &autojoinCommands() const;
		QStringList &autojoinCommands();

		/**
		 * @brief A short, human-readable description for the network.
		 * (Preferably a single word).
		 */
		const QString &description() const;

		/**
		 * @brief Join this network when Doomseeker starts up.
		 */
		bool isAutojoinNetwork() const;

		bool isValid() const
		{
			return !address().isEmpty() && port() != 0;
		}

		const QString &nickservCommand() const;
		const QString &nickservPassword() const;

		/**
		 * @brief Sorts by description.
		 */
		bool operator< (const IRCNetworkEntity& other) const
		{
			return description().toLower().trimmed() < other.description().toLower().trimmed();
		}

		/**
		 * @brief Checks to see if this is the same server.
		 */
		bool operator== (const IRCNetworkEntity& other) const
		{
			return address() == other.address() && port() == other.port();
		}

		/**
		 * @brief Password for the server or network. Ignored if empty.
		 */
		const QString &password() const;

		/**
		 * @brief Port of the server or network to connect to.
		 *
		 * Default value: 6667
		 */
		unsigned short port() const;

		void setAddress(const QString &v);
		void setAutojoinChannels(const QStringList &v);
		void setAutojoinCommands(const QStringList &v);
		void setAutojoinNetwork(bool v);
		void setDescription(const QString &v);
		void setNickservCommand(const QString &v);
		void setNickservPassword(const QString &v);
		void setPassword(const QString &v);
		void setPort(unsigned short v);

	private:
		class PrivData;
		PrivData *d;
};

#endif

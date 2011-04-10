//------------------------------------------------------------------------------
// ircresponseparser.h
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
#ifndef __IRCRESPONSEPARSER_H__
#define __IRCRESPONSEPARSER_H__

#include <QObject>
#include <QString>

class IRCResponseParser : public QObject
{
	Q_OBJECT

	public:
		void			parse(const QString& message);
		
	signals:
		/**
		 *	@brief Emitted when user successfuly joins the network.
		 *
		 *	@param nickname
		 *		Nickname with which the user has just registered.
		 */
		void			helloClient(const QString& nickname);
		
		void			kick(const QString& channel, const QString& byWhom, const QString& whoIsKicked, const QString& reason);
		void			kill(const QString& victim, const QString& comment);
	
		/**
		 *	@brief Carries info about MODE for display.
		 */
		void			modeInfo(const QString& channel, const QString& whoSetThis, const QString& modeParams);
		void			namesListReceived(const QString& channel, const QStringList& names);
		void			namesListEndReceived(const QString& channel);
		void			nicknameInUse(const QString& nickname);
		void			noSuchNickname(const QString& nickname);
		void			parseError(const QString& error);
		void			privMsgReceived(const QString& recipient, const QString& sender, const QString& content);
		void			sendPongMessage(const QString& sendWhere);
		void			userChangesNickname(const QString& oldNickname, const QString& newNickname);
		void			userJoinsChannel(const QString& channel, const QString& nickname, const QString& fullSignature);
		void			userModeChanged(const QString& channel, const QString& nickname, unsigned flagsAdded, unsigned flagsRemoved);
		void			userPartsChannel(const QString& channel, const QString& nickname, const QString& farewellMessage);
		void			userQuitsNetwork(const QString& nickname, const QString& farewellMessage);
		
		/**
		 *	@brief Emitted with response 311 RPL_WHOISUSER.
		 */
		void			whoIsUser(const QString& nickname, const QString& user, const QString& hostName, const QString& realName);
		
	private:
		enum FlagModes
		{
			FlagModeAdd,
			FlagModeRemove,
			FlagModeError
		};
		
		QString			joinAndTrimColonIfNecessary(const QStringList& strList) const;
	
		/**
		 *	@brief Will return FlagModeError if the given character cannot be 
		 *	translated.
		 */
		FlagModes		getFlagMode(char c);
	
		void			parseMessage(const QString& prefix, const QString& sender, const QString& type, QStringList params);
		
		/**
		 *	@brief Will emit userModeChanged() for each character in flagsString.
		 *
		 *	This method will "damage" the passed nicknames parameter.
		 *	Use with care.
		 */
		void			parseUserModeMessage(const QString& channel, QString flagsString, QStringList& nicknames);
		
		QString&		trimColonIfNecessary(QString& str) const;
};

#endif
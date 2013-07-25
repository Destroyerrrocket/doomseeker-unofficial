//------------------------------------------------------------------------------
// ircuserinfo.h
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
#ifndef __IRCUSERINFO_H__
#define __IRCUSERINFO_H__

#include <QString>

/**
 *	@brief Holds information flags about given nickname.
 */
class IRCUserInfo
{
	public:
		static const unsigned FLAG_OP			= 0x1;
		static const unsigned FLAG_VOICE		= 0x2;
		static const unsigned FLAG_HALFOP		= 0x4;
		
		/**
		 *	@brief Converts a character from MODE list to a flag.
		 */
		static unsigned		convertModeCharToFlag(char c);
		
		/**
		 *	@brief Converts a character fron nickname to a flag.
		 */
		static unsigned		convertNickCharToFlag(char c);
		
		/**
		 *	@brief Full user signature with nickname and hostname.
		 *
		 *	It must be noted that this variable value might not be
		 *	valid even if IRCUserInfo is containing a valid nickname
		 *	information. In fact this value will always be empty
		 *	for chat adapters. However network adapters will require
		 *	this signature info to deliver bans.
		 */
		QString				fullSignature;

		/**
		 *	@brief Constructor will set flags based on nickname's
		 *	prefix.
		 */
		IRCUserInfo(const QString& nickname, const QString& fullSignature = "");

		/**
		 *	@brief Returns nickname with no prefixes, contrary to the
		 *	prefixedName() .
		 */
		const QString&		cleanNickname() const { return userName; }

		/**
		 *	@brief Returns cleanNickname() with a call to 
		 *	IRCGlobal::toIrcLower() .
		 */
		QString				cleanNicknameLowerCase() const;
		
		QString				extractHostnameFromFullSignature() const;

		unsigned			flags() const { return this->userFlags; }

		bool				isFlag(unsigned flag) const;
		bool				isHalfOp() const { return isFlag(FLAG_HALFOP); }

		/**
		 *	@brief Check if this user and user specified as parameter
		 *	are the same user.
		 *
		 *	Calls the operator==.
		 */
		bool				isSameNickname(const IRCUserInfo& otherUser) const; 

		bool				isOp() const { return isFlag(FLAG_OP); }
		bool				isVoiced() const { return isFlag(FLAG_VOICE); }

		bool				operator==(const IRCUserInfo& otherUser) const;
		
		/**
		 *	@brief This operator can be used for sorting.
		 *
		 *	@return True if this user's nickname is earlier in alphabet
		 *	than other user's nickname. Also true if this user has one of the
		 *	following flags over the other user: op, voice (in this order).
		 */
		bool				operator<=(const IRCUserInfo& otherUser) const;

		/**
		 *	@brief Will generate prefix based on the user flags.
		 */
		QString				prefixedName() const;

		/**
		 *	@brief Returns prefixedName() with a call to 
		 *	IRCGlobal::toIrcLower() .
		 */
		QString				prefixedNameLowerCase() const;

		void				setFlag(unsigned flag, bool bSet);
		void				setFlags(unsigned flags) { this->userFlags = flags; }

		void				setIsOp(bool b) { setFlag(FLAG_OP, b); }
		void				setIsVoiced(bool b) { setFlag(FLAG_VOICE, b); }

	private:
		unsigned			userFlags;
		QString				userName;

		/**
		 *	@param name
		 *		Name from which the flags will be extracted. Also the name
		 *		itself will be stripped of any special chars.
		 *
		 *	@return userFlags values.
		 */
		unsigned			extractFlags(QString& name);

};

#endif

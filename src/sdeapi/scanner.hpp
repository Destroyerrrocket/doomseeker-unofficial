// Emacs style mode select   -*- C++ -*-
// =============================================================================
// ### ### ##   ## ###  #   ###  ##   #   #  ##   ## ### ##  ### ###  #  ###
// #    #  # # # # #  # #   #    # # # # # # # # # # #   # #  #   #  # # #  #
// ###  #  #  #  # ###  #   ##   # # # # # # #  #  # ##  # #  #   #  # # ###
//   #  #  #     # #    #   #    # # # # # # #     # #   # #  #   #  # # #  #
// ### ### #     # #    ### ###  ##   #   #  #     # ### ##  ###  #   #  #  #
//                                     --= http://bitowl.com/sde/ =--
// =============================================================================
// Copyright (C) 2008 "Blzut3" (admin@maniacsvault.net)
// The SDE Logo is a trademark of GhostlyDeath (ghostlydeath@gmail.com)
// =============================================================================
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
// =============================================================================
// Description:
// =============================================================================

#ifndef __SCANNER_HPP__
#define __SCANNER_HPP__

#include <QObject>
#include <QString>

#include "global.h"

enum ETokenType
{
	TK_Identifier,	// Ex: SomeIdentifier
	TK_StringConst,	// Ex: "Some String"
	TK_IntConst,	// Ex: 27
	TK_FloatConst,	// Ex: 1.5
	TK_BoolConst,	// Ex: true
	TK_Void,		// void
	TK_String,		// str
	TK_Int,			// int
	TK_Float,		// float
	TK_Bool,		// bool
	TK_AndAnd,		// &&
	TK_OrOr,		// ||
	TK_EqEq,		// ==
	TK_NotEq,		// !=
	TK_GtrEq,		// >=
	TK_LessEq,		// <=
	TK_ShiftLeft,	// <<
	TK_ShiftRight,	// >>

	TK_NoToken = -1,
};

/**
 * Scanner reads scripts by checking individual tokens.
 * @author Blzut3
 */
class MAIN_EXPORT Scanner
{
	public:
		Scanner(const char* data, unsigned int length);
		~Scanner();

		/**
		 * This function will scan the next token and return if it is of the
		 * specified type.  If it returns true, then the position will be moved
		 * to after the token, otherwise it will be reset.
		 */
		bool		checkToken(char token);
		/**
		 * Gets whatever token is next.
		 */
		ETokenType	nextToken();
		/**
		 * Requires that the next token be of the specified type.  Errors will
		 * be printed if that is not the case.
		 */
		void		mustGetToken(char token);
		/**
		 * Returns true if there is still more to read.
		 */
		bool		tokensLeft() { return (error != 0) || (pos < length); }

		QString			str;
		unsigned int	number;
		double			decimal;
		bool			boolean;
		char			lastToken;
	protected:
		/**
		 * Moves the position ahead any whitespace that there might be from the
		 * current position.
		 */
		void		checkForWhitespace(unsigned int *nPos = NULL, unsigned int *nLpos = NULL);
		void		token(unsigned int &pos, unsigned int &lpos, unsigned int &line, char token, bool report=false);
		bool		next(QString &out, unsigned int &pos, unsigned int &lpos, char type, bool report=false);

		bool			error;
		char*			data;
		unsigned int	length;
		unsigned int	line;
		unsigned int	lpos;
		unsigned int	pos;
};

#endif /* __SCANNER_HPP__ */

//------------------------------------------------------------------------------
// scanner.hpp
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

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
		Scanner(const char* data, int length=-1);
		~Scanner();

		/**
		 * This function will scan the next token and return if it is of the
		 * specified type.  If it returns true, then the position will be moved
		 * to after the token, otherwise it will be reset.
		 */
		bool		checkToken(char token);
		/**
		 * Gets whatever token is next returning true on success.
		 * @param expandState Used by checkToken, leave as true.
		 */
		bool		nextToken(bool autoExpandState=true);

		/**
		 * Returns true if there is still more to read.
		 */
		bool		tokensLeft() { return scanPos < length; }

		QString			str;
		unsigned int	number;
		double			decimal;
		bool			boolean;
		char			token;
	protected:
		/**
		 * Moves the position ahead any whitespace that there might be from the
		 * current position.
		 */
		void		checkForWhitespace();
		/**
		 * Transfers nextState over for use.
		 */
		void		expandState();
		/**
		 * Changes the values in line and lineStart, does not change the actual
		 * scanning position in the file.
		 */
		void		incrementLine();

		class ParserState
		{
            public:
                QString			str;
                unsigned int	number;
                double			decimal;
                bool			boolean;
                char			token;
                unsigned int	tokenLine;
                unsigned int	tokenLinePosition;
		}				nextState;

		char*			data;
		unsigned int	length;

		unsigned int	line;
		unsigned int	lineStart;
		unsigned int	tokenLine;
		unsigned int	tokenLinePosition;
		unsigned int	scanPos;

		bool			needNext; // If checkToken returns false this will be false.
};

#endif /* __SCANNER_HPP__ */

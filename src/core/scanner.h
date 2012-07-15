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
	TK_Identifier,		// Ex: SomeIdentifier
	TK_StringConst,		// Ex: "Some String"
	TK_IntConst,		// Ex: 27
	TK_FloatConst,		// Ex: 1.5
	TK_BoolConst,		// Ex: true
	TK_AndAnd,			// &&
	TK_OrOr,			// ||
	TK_EqEq,			// ==
	TK_NotEq,			// !=
	TK_GtrEq,			// >=
	TK_LessEq,			// <=
	TK_ShiftLeft,		// <<
	TK_ShiftRight,		// >>
	TK_Increment,		// ++
	TK_Decrement,		// --
	TK_PointerMember,	// ->
	TK_ScopeResolution,	// ::
	TK_MacroConcat,		// ##
	TK_AddEq,			// +=
	TK_SubEq,			// -=
	TK_MulEq,			// *=
	TK_DivEq,			// /=
	TK_ModEq,			// %=
	TK_ShiftLeftEq,		// <<=
	TK_ShiftRightEq,	// >>=
	TK_AndEq,			// &=
	TK_OrEq,			// |=
	TK_XorEq,			// ^=
	TK_Ellipsis,		// ...

	TK_NumSpecialTokens,

	TK_NoToken = -1
};

/**
 * Scanner reads scripts by checking individual tokens.
 * @author Blzut3
 */
class MAIN_EXPORT Scanner
{
	public:
		struct ParserState
		{
			QString			str;
			unsigned int	number;
			double			decimal;
			bool			boolean;
			char			token;
			unsigned int	tokenLine;
			unsigned int	tokenLinePosition;
			unsigned int	scanPos;
		};

		enum MessageLevel
		{
			ML_ERROR,
			ML_WARNING,
			ML_NOTICE
		};

		Scanner(const char* data, int length=-1);
		~Scanner();

		void			checkForMeta();
		/**
		 * Moves the position ahead any whitespace that there might be from the
		 * current position.
		 */
		void			checkForWhitespace();
		/**
		 * This function will scan the next token and return if it is of the
		 * specified type.  If it returns true, then the position will be moved
		 * to after the token, otherwise it will be reset.
		 */
		bool			checkToken(char token);
		/**
		 * Transfers nextState over for use.
		 */
		void			expandState();
		int				currentLine() const { return state.tokenLine; }
		int				currentLinePos() const { return state.tokenLinePosition; }
		int				currentPos() const { return logicalPosition; }
		unsigned int	currentScanPos() const { return scanPos; }
		bool			nextString();
		/**
		 * Gets whatever token is next returning true on success.
		 * @param expandState Used by checkToken, leave as true.
		 */
		bool			nextToken(bool autoExpandState=true);
		void			mustGetToken(char token);
		void			rewind(); /// Only can rewind one step.
		const char*		scriptData() const { return data; }
		void			scriptMessage(MessageLevel level, const char* error, ...) const;
		void			setScriptIdentifier(const QString &ident) { scriptIdentifier = ident; }
		int				skipLine();

		/**
		 * Returns true if there is still more to read.
		 */
		bool			tokensLeft() { return scanPos < length; }

		const ParserState &operator*() const { return state; }
		const ParserState *operator->() const { return &state; }

		static const QString& escape(QString &str);
		static const QString& unescape(QString &str);

		static void		setMessageHandler(void (*handler)(MessageLevel, const char*, va_list)) { messageHandler = handler; }

		ParserState		state;

	protected:
		/**
		 * Changes the values in line and lineStart, does not change the actual
		 * scanning position in the file.
		 */
		void		incrementLine();

	private:
		ParserState		nextState, prevState;

		char*			data;
		unsigned int	length;

		unsigned int	line;
		unsigned int	lineStart;
		unsigned int	logicalPosition;
		unsigned int	scanPos;

		bool			needNext; // If checkToken returns false this will be false.

		QString			scriptIdentifier;

		static void		(*messageHandler)(MessageLevel, const char*, va_list);
};

#endif /* __SCANNER_HPP__ */

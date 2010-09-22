//------------------------------------------------------------------------------
// ircrequestparser.h
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
#ifndef __IRCREQUESTPARSER_H__
#define __IRCREQUESTPARSER_H__

#include <QString>

/**
 *	@brief Parses request and interprets them in a way that emulates
 *	mIRC (or any even slightly sane IRC client for that matter).
 *
 *	The above statement means that a non-RFC compliant request form that users 
 *	grew accustomed to will be onverted to RFC 1459 compliant form.
 *
 *	For example:
 *	- If command begins with '/' character this character will be always 
 *	  trimmed. If command doesn't begin with '/' character error will be 
 *	  returned.
 *	  @b Example: "/who <nickname>" will be converted to "who <nickname>"
 *	- ':' characters will be inserted into some commands.
 *	  @b Example: "/part #channel I am leaving!" to 
 *	  "part #channel :I am leaving!"
 *	
 *	For more information refer to parse() method.
 */
class IRCRequestParser
{
	public:
		enum IRCRequestParseResult
		{
			Ok,
			ErrorMessageTooLong,
			ErrorMessageEmpty,
			ErrorInputNotPrependedWithSlash,
			ErrorInputInsufficientParameters,
			QuitCommand
		};	
		
		/**
		 *	@brief Parses input string and returns it through output string.
		 *	Additional information is passed through return value.
		 *
		 *	@param input
		 *		Input message in common format. See IRCRequestParser 
		 *		description.
		 *	@param output [out]
		 *		Message in RFC 1459 format.
		 *
		 *	@return
		 *	This method will:
		 *	- Check if message fits in the RFC 1459 max message length. If not
		 *	  ErrorMessageTooLong is returned.
		 *	- Return ErrorMessageEmpty if input is empty or consists only of
		 *	  whitespace.
		 *	- Check if input starts with '/' character.
		 *	  If not ErrorInputNotPrependedWithSlash is returned.
		 *	- Return ErrorInputInsufficientParameters if parsed command expects
		 *	  more parameters to work correctly.
		 *	- QuitCommand is returned if "/quit" command is specified.
		 *	- Ok is returned if it is ok to send the output message.
		 */
		static IRCRequestParseResult	parse(QString input, QString& output);
};

#endif
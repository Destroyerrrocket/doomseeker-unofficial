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
// Copyright (C) 2008 GhostlyDeath (ghostlydeath@gmail.com)
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

#include <cstdio>
#include <cmath>

#include "sdeapi/scanner.hpp"
#include "sdeapi/config.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

Scanner::Scanner(const char* data, int length) : line(1), lineStart(0), scanPos(0), needNext(true)
{
	if(length == -1)
		length = strlen(data);
	this->length = length;
	this->data = new char[length];
	memcpy(this->data, data, length);
}

Scanner::~Scanner()
{
	delete[] data;
}

void Scanner::checkForWhitespace()
{
	int comment = 0; // 1 = till next new line, 2 = till end block
	while(scanPos < length)
	{
		char cur = data[scanPos];
		char next = scanPos+1 < length ? data[scanPos+1] : 0;
		if(comment == 2)
		{
			if(cur != '*' || next != '/')
				scanPos++;
			else
			{
				comment = 0;
				scanPos += 2;
			}
			continue;
		}

		if(cur == ' ' || cur == '\t')
			scanPos++;
		else if(cur == '\n' || cur == '\r')
		{
			scanPos++;
			incrementLine();
			if(comment == 1)
				comment = 0;

			// Do a quick check for Windows style new line
			if(cur == '\r' && next == '\n')
				scanPos++;
		}
		else if(cur == '/')
		{
			switch(next)
			{
				case '/':
					comment = 1;
					break;
				case '*':
					comment = 2;
					break;
				default:
					return;
			}
			scanPos += 2;
		}
		else
		{
			if(comment == 0)
				return;
			else
				scanPos++;
		}
	}
}

bool Scanner::checkToken(char token)
{
	if(needNext)
	{
		if(!nextToken())
			return false;
	}

	// An int can also be a float.
	if(this->token == token || (this->token == TK_IntConst && token == TK_FloatConst))
	{
		needNext = true;
		return true;
	}
	needNext = false;
	return false;
}

void Scanner::incrementLine()
{
	line++;
	lineStart = scanPos;
}

bool Scanner::nextToken()
{
	if(!needNext)
	{
		needNext = true;
		return true;
	}

	checkForWhitespace();
	if(scanPos >= length)
		return false;

	int start = scanPos;
	int end = scanPos;
	token = TK_NoToken;
	bool floatHasDecimal = false;
	bool floatHasExponent = false;
	bool stringFinished = false; // Strings are the only things that can have 0 length tokens.

	char cur = data[scanPos++];
	// Determine by first character
	if(cur == '_' || (cur >= 'A' && cur <= 'Z') || (cur >= 'a' && cur <= 'z'))
		token = TK_Identifier;
	else if(cur >= '0' && cur <= '9')
		token = TK_IntConst;
	else if(cur == '.')
	{
		floatHasDecimal = true;
		token = TK_FloatConst;
	}
	else if(cur == '"')
	{
		end = ++start; // Move the start up one character so we don't have to trim it later.
		token = TK_StringConst;
	}
	else
	{
		end = scanPos;
		token = cur;

		// Now check for operator tokens
		if(scanPos < length)
		{
			char next = data[scanPos];
			if(cur == '&' && next == '&')
				token = TK_AndAnd;
			else if(cur == '|' && next == '|')
				token = TK_OrOr;
			else if(cur == '<' && next == '<')
				token = TK_ShiftLeft;
			else if(cur == '>' && next == '>')
				token = TK_ShiftRight;
			else if(next == '=')
			{
				switch(cur)
				{
					case '=':
						token = TK_EqEq;
						break;
					case '!':
						token = TK_NotEq;
						break;
					case '>':
						token = TK_GtrEq;
						break;
					case '<':
						token = TK_LessEq;
						break;
					default:
						break;
				}
			}

			if(token != cur)
			{
				scanPos++;
				end = scanPos;
			}
		}
	}

	if(start == end)
	{
		while(scanPos < length)
		{
			cur = data[scanPos];
			switch(token)
			{
				default:
					break;
				case TK_Identifier:
					if(cur != '_' && (cur < 'A' || cur > 'Z') && (cur < 'a' || cur > 'z') && (cur < '0' || cur > '9'))
						end = scanPos;
					break;
				case TK_IntConst:
					if(cur == '.' || (scanPos-start != 0 && cur == 'e'))
						token = TK_FloatConst;
					else
					{
						if(cur < '0' || cur > '9')
							end = scanPos;
						break;
					}
				case TK_FloatConst:
					if(cur < '0' || cur > '9')
					{
						if(!floatHasDecimal && cur == '.')
						{
							floatHasDecimal = true;
							break;
						}
						else if(!floatHasExponent && cur == 'e')
						{
							floatHasDecimal = true;
							floatHasExponent = true;
							if(scanPos+1 < length)
							{
								char next = data[scanPos+1];
								if((next < '0' || next > '9') && next != '+' && next != '-')
									end = scanPos;
								else
									scanPos++;
							}
							break;
						}
						end = scanPos;
					}
					break;
				case TK_StringConst:
					if(cur == '"')
					{
						stringFinished = true;
						end = scanPos;
						scanPos++;
					}
					else if(cur == '\\')
						scanPos++; // Will add two since the loop automatically adds one
					break;
			}
			if(start == end && !stringFinished)
				scanPos++;
			else
				break;
		}
	}

	if(end-start > 0 || stringFinished)
	{
		str = QByteArray(data+start, end-start);
		if(token == TK_FloatConst)
		{
			decimal = str.toDouble();
			number = static_cast<int> (decimal);
			boolean = (number != 0);
		}
		else if(token == TK_IntConst)
		{
			number = str.toUInt();
			decimal = number;
			boolean = (number != 0);
		}
		else if(token == TK_Identifier)
		{
			// Check for a boolean constant.
			if(str.compare("true") == 0)
			{
				token = TK_BoolConst;
				boolean = true;
			}
			else if(str.compare("false") == 0)
			{
				token = TK_BoolConst;
				boolean = false;
			}
		}
		else if(token == TK_StringConst)
		{
			str = Config::unescape(str);
		}
		return true;
	}
	return false;
}

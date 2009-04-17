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

#include <cmath>

#include "sdeapi/scanner.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

Scanner::Scanner(char* data, unsigned int length)
: number(0), decimal(0), boolean(false), lastToken(0), error(false), data(data), length(length), line(0), lpos(0), pos(0)
{
	checkForWhitespace();
}

Scanner::~Scanner()
{
}

void Scanner::mustGetToken(char token)
{
	this->token(pos, lpos, line, token, true);
}

bool Scanner::checkToken(char token)
{
	if(error)
		return false;
	unsigned int nPos = pos;
	unsigned int nLpos = lpos;
	unsigned int nLine = line;
	this->token(nPos, nLpos, nLine, token, false);
	if(!error)
	{
		pos = nPos;
		lpos = nLpos;
		line = nLine;
		return true;
	}
	else
		error = false;
	return false;
}

ETokenType Scanner::nextToken()
{
	if(pos >= length)
		return TK_NoToken;

	if(data[pos] >= '0' && data[pos] <= '9')
	{
		if(checkToken(TK_Identifier))
			return TK_Identifier;
		else if(checkToken(TK_FloatConst))
		{
			double integerPart = 0.0;
			if(modf(decimal, &integerPart) != 0.0)
				return TK_FloatConst;
			else
			{
				number = static_cast<unsigned int> (integerPart);
				return TK_IntConst;
			}
		}
		else if(checkToken(TK_IntConst))
			return TK_IntConst;
	}
	else if(data[pos] >= 'a' && data[pos] <= 'z')
	{
		if(checkToken(TK_BoolConst))
			return TK_BoolConst;
		else if(checkToken(TK_Void))
			return TK_Void;
		else if(checkToken(TK_String))
			return TK_String;
		else if(checkToken(TK_Int))
			return TK_Int;
		else if(checkToken(TK_Bool))
			return TK_Bool;
		else if(checkToken(TK_Identifier))
			return TK_Identifier;
	}
	else if((data[pos] >= 'A' && data[pos] <= 'Z') || data[pos] == '_')
	{
		if(checkToken(TK_Identifier))
			return TK_Identifier;
	}
	else if(data[pos] == '"')
	{
		if(checkToken(TK_StringConst))
			return TK_StringConst;
	}
	else
	{
		if(checkToken(TK_AndAnd))
			return TK_AndAnd;
		else if(checkToken(TK_OrOr))
			return TK_OrOr;
		else if(checkToken(TK_EqEq))
			return TK_EqEq;
		else if(checkToken(TK_NotEq))
			return TK_NotEq;
		else if(checkToken(TK_GtrEq))
			return TK_GtrEq;
		else if(checkToken(TK_LessEq))
			return TK_LessEq;
		else if(checkToken(TK_ShiftLeft))
			return TK_ShiftLeft;
		else if(checkToken(TK_ShiftRight))
			return TK_ShiftRight;
	}
	if(checkToken(data[pos]))
		return static_cast<ETokenType> (data[pos]);
	return TK_NoToken;
}

void Scanner::checkForWhitespace(unsigned int *nPos, unsigned int *nLpos)
{
	unsigned int & uPos = (nPos ? (*nPos) : pos);
	unsigned int & uLpos = (nLpos ? (*nLpos) : lpos);
	while(data[uPos] == ' ' || data[uPos] == '\0' || data[uPos] == '	' || data[uPos] == '\n' || data[uPos] == '\r' ||
		(data[uPos] == '/' && uPos+1 < length && (data[uPos+1] == '/' || data[uPos+1] == '*')))
	{
		//comment
		if(data[uPos] == '/' && uPos+1 < length && (data[uPos+1] == '/' || data[uPos+1] == '*'))
		{
			uPos += 2;
			if(uPos == length)
				return;
			if(data[uPos-1] == '*') // multiline
			{
				while(true)
				{
					// Since we may be skipping end of lines lets check for them.
					if(data[uPos] == '\r' || data[uPos] == '\n')
					{
						if(data[uPos] == '\r' && uPos+1 < length && data[uPos+1] == '\n')
							uPos++;
						line++;
						uLpos = 0;
					}

					uPos++;
					if(data[uPos-1] == '*' && uPos < length && data[uPos] == '/')
						break;
					if(uPos == length)
						return;
				}
			}
			else // single line
			{
				// Go until the end of line is reached.
				while(data[uPos] != '\r' && data[uPos] != '\n')
				{
					uPos++;
					if(uPos == length)
						return;
				}
			}
		}

		if(data[uPos] == '\r' || data[uPos] == '\n')
		{
			if(data[uPos] == '\r' && uPos+1 < length && data[uPos+1] == '\n')
				uPos++;
			line++;
			uLpos = 0;
		}

		uPos++;
		uLpos++;
		if(uPos >= length)
			return;
	}
}

//For exmaple GENERIC_TOKEN(void) would do all the needed errors and such for looking for void
#define GENERIC_GETTOKEN(token) \
{ \
	QString ident; \
	if(next(ident, pos, lpos, TK_Identifier, report)) \
	{ \
		Qt::CaseSensitivity cs = Qt::CaseInsensitive; \
		if(ident.compare(token, cs) == 0) \
		{ \
			str = ident; \
		} \
		else \
		{ \
			error = true; \
			if(report) \
				printf("Line %d:%d: Expected \"%s\", but got \"%s\" instead.\n", line, lpos, token, ident.toAscii().constData()); \
		} \
	} \
	break; \
}
//For searching for symbols like == or <<
#define GENERIC_DOUBLETOKEN(token) \
{ \
	const char* tkn = token; \
	if(pos >= length-1) \
	{ \
		error = true; \
		if(report) \
			printf("Line %d:%d: Expected \"%s\".\n", line, lpos, token); \
	} \
	if(data[pos] == tkn[0] && data[pos+1] == tkn[1]) \
	{ \
		str = token; \
		pos += 2; \
		lpos += 2; \
	} \
	else \
	{ \
		error = true; \
		if(report) \
			printf("Line %d:%d: Expected \"%s\", but got \"%c%c\" instead.\n", line, lpos, token, data[pos], data[pos+1]); \
	} \
	break; \
}
void Scanner::token(unsigned int &pos, unsigned int &lpos, unsigned int &line, char token, bool report)
{
	if(error)
		return;
	if(pos >= length)
	{
		if(report)
			printf("Unexpected end of file.\n");
		return;
	}
	switch(token)
	{
		case TK_Identifier:
		{
			QString ident;
			if(next(ident, pos, lpos, TK_Identifier, report))
			{
				str = ident;
			}
			break;
		}
		case TK_StringConst:
		{
			QString stringConst;
			if(next(stringConst, pos, lpos, TK_StringConst, report))
			{
				str = stringConst;
			}
			break;
		}
		case TK_IntConst:
		{
			QString integer;
			if(next(integer, pos, lpos, TK_IntConst, report))
				number = integer.toInt();
			break;
		}
		case TK_FloatConst:
		{
			QString integer;
			if(next(integer, pos, lpos, TK_FloatConst, report))
				decimal = integer.toDouble();
			break;
		}
		case TK_BoolConst:
		{
			QString ident;
			if(next(ident, pos, lpos, TK_Identifier, report))
			{
				Qt::CaseSensitivity cs = Qt::CaseInsensitive;
				if(ident.compare("true", cs) == 0)
					boolean = true;
				else if(ident.compare("false", cs) == 0)
					boolean = false;
				else
				{
					error = true;
					if(report)
						printf("Line %d:%d: Expected true/false, but got \"%s\" instead.\n", line, lpos, ident.toAscii().constData());
					break;
				}
			}
			break;
		}
		case TK_Void:
			GENERIC_GETTOKEN("void");
		case TK_String:
			GENERIC_GETTOKEN("str");
		case TK_Int:
			GENERIC_GETTOKEN("int");
		case TK_Float:
			GENERIC_GETTOKEN("float");
		case TK_Bool:
			GENERIC_GETTOKEN("bool");
		case TK_AndAnd:
			GENERIC_DOUBLETOKEN("&&");
		case TK_OrOr:
			GENERIC_DOUBLETOKEN("||");
		case TK_EqEq:
			GENERIC_DOUBLETOKEN("==");
		case TK_NotEq:
			GENERIC_DOUBLETOKEN("!=");
		case TK_GtrEq:
			GENERIC_DOUBLETOKEN(">=");
		case TK_LessEq:
			GENERIC_DOUBLETOKEN("<=");
		case TK_ShiftLeft:
			GENERIC_DOUBLETOKEN("<<");
		case TK_ShiftRight:
			GENERIC_DOUBLETOKEN(">>");
		default:
			if(data[pos] != token)
			{
				error = true;
				if(report)
					printf("Line %d:%d: Expected '%c', but got '%c' instead.\n", line, lpos, token, data[pos]);
			}
			pos++;
			lpos++;
			break;
	}
	if(!error)
		lastToken = token;
	else
		lastToken = 0;

	checkForWhitespace(&pos, &lpos);
}

//Find the next identifier by looping until we find an invalid character.
bool Scanner::next(QString &out, unsigned int &pos, unsigned int &lpos, char type, bool report)
{
	if(pos >= length)
	{
		out = QString();
		return false;
	}

	unsigned int end = pos;
	bool special = false;
	bool special2 = false;
	for(unsigned int i = pos;i < length;i++)
	{
		if(type == TK_Identifier)
		{
			if(!((data[i] >= 'a' && data[i] <= 'z') || (data[i] >= 'A' && data[i] <= 'Z') ||
				data[i] == '_' || (i != pos && data[i] >= '0' && data[i] <= '9')))
			{
				break;
			}
		}
		else if(type == TK_StringConst)
		{
			if(!special) // Did we start the string?
			{
				if(data[i] != '"')
					break;
				special = true;
			}
			else
			{
				if(!special2 && data[i] == '"')
					break;
				if(data[i] == '\\')
					special2 = !special2;
				else
					special2 = false;
			}
		}
		else if(type == TK_IntConst)
		{
			if(!(data[i] >= '0' && data[i] <= '9'))
				break;
		}
		else
		{
			if(data[i] == '.')
			{
				if(special)
					break;
				special = true;
			}
			else if(!(data[i] >= '0' && data[i] <= '9'))
				break;
		}
		end = i+1;
	}
	if(end == pos)
	{
		error = true;
		if(report)
		{
			if(type == TK_Identifier)
				printf("Line %d:%d: Expected an identifier, but got '%c' instead.\n", line, lpos, data[pos]);
			else if(type == TK_StringConst)
				printf("Line %d:%d: Expected a string constant, but got '%c' instead.\n", line, lpos, data[pos]);
			else if(type == TK_IntConst)
				printf("Line %d:%d: Expected an integer, but got '%c' instead.\n", line, lpos, data[pos]);
			else
				printf("Line %d:%d: Expected a decimal value, but got '%c' instead.\n", line, lpos, data[pos]);
		}
		return false;
	}
	QString result = QString::fromAscii(&data[pos], end - pos);
	//strip \\ and \" and cut out the opening quote
	if(type == TK_StringConst)
	{
		result = result.mid(1, result.length()-1);
		while(result.indexOf("\\\\") != -1)
			result.replace(result.indexOf("\\\\"), 2, '\\');
		while(result.indexOf("\\\"") != -1)
			result.replace(result.indexOf("\\\""), 2, '\\');
	}
	result.append('\0');
	lpos += end - pos;
	pos = end;
	if(type == TK_StringConst)
	{
		pos++;
		lpos++;
	}

	out = result;
	return true;
}

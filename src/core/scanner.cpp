//------------------------------------------------------------------------------
// scanner.cpp
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

#include <cstdio>
#include <cmath>

#include "scanner.h"

void (*Scanner::messageHandler)(MessageLevel, const char*, va_list) = NULL;

static const char* const TokenNames[TK_NumSpecialTokens] =
{
	"Identifier",
	"String Constant",
	"Integer Constant",
	"Float Constant",
	"Boolean Constant",
	"Logical And",
	"Logical Or",
	"Equals",
	"Not Equals",
	"Greater Than or Equals"
	"Less Than or Equals",
	"Left Shift",
	"Right Shift",
	"Increment",
	"Decrement",
	"Pointer Member",
	"Scope Resolution",
	"Macro Concatenation",
	"Assign Sum",
	"Assign Difference",
	"Assign Product",
	"Assign Quotient",
	"Assign Modulus",
	"Assign Left Shift",
	"Assign Right Shift",
	"Assign Bitwise And",
	"Assign Bitwise Or",
	"Assign Exclusive Or",
	"Ellipsis"
};

////////////////////////////////////////////////////////////////////////////////

Scanner::Scanner(const char* data, int length) : line(1), lineStart(0), logicalPosition(0), scanPos(0), needNext(true)
{
	if(length == -1)
		length = strlen(data);
	this->length = length;
	this->data = new char[length];
	memcpy(this->data, data, length);

	checkForWhitespace();

	state.scanPos = scanPos;
}

Scanner::~Scanner()
{
	delete[] data;
}

// Here's my answer to the preprocessor screwing up line numbers. What we do is
// after a new line in CheckForWhitespace, look for a comment in the form of
// "/*meta:filename:line*/"
void Scanner::checkForMeta()
{
	if(scanPos+10 < length)
	{
		char metaCheck[8];
		memcpy(metaCheck, data+scanPos, 7);
		metaCheck[7] = 0;
		if(strcmp(metaCheck, "/*meta:") == 0)
		{
			scanPos += 7;
			int metaStart = scanPos;
			int fileLength = 0;
			int lineLength = 0;
			while(scanPos < length)
			{
				char thisChar = data[scanPos];
				char nextChar = scanPos+1 < length ? data[scanPos+1] : 0;
				if(thisChar == '*' && nextChar == '/')
				{
					lineLength = scanPos-metaStart-1-fileLength;
					scanPos += 2;
					break;
				}
				if(thisChar == ':' && fileLength == 0)
					fileLength = scanPos-metaStart;
				scanPos++;
			}
			if(fileLength > 0 && lineLength > 0)
			{
				setScriptIdentifier(QString::fromAscii(data+metaStart, fileLength));
				QString lineNumber = QString::fromAscii(data+metaStart+fileLength+1, lineLength);
				line = atoi(lineNumber.toAscii().constData());
				lineStart = scanPos;
			}
		}
	}
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
			{
				if(cur == '\n' || cur == '\r')
				{
					scanPos++;
					if(comment == 1)
						comment = 0;

					// Do a quick check for Windows style new line
					if(cur == '\r' && next == '\n')
						scanPos++;
					incrementLine();
				}
				else
					scanPos++;
			}
			else
			{
				comment = 0;
				scanPos += 2;
			}
			continue;
		}

		if(cur == ' ' || cur == '\t' || cur == 0)
			scanPos++;
		else if(cur == '\n' || cur == '\r')
		{
			scanPos++;
			if(comment == 1)
				comment = 0;

			// Do a quick check for Windows style new line
			if(cur == '\r' && next == '\n')
				scanPos++;
			incrementLine();
			checkForMeta();
		}
		else if(cur == '/' && comment == 0)
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
		if(!nextToken(false))
			return false;
	}

	// An int can also be a float.
	if(nextState.token == token || (nextState.token == TK_IntConst && token == TK_FloatConst))
	{
		needNext = true;
		expandState();
		return true;
	}
	needNext = false;
	return false;
}

void Scanner::expandState()
{
	scanPos = nextState.scanPos;
	logicalPosition = scanPos;
	checkForWhitespace();

	prevState = state;
	state = nextState;
}

void Scanner::incrementLine()
{
	line++;
	lineStart = scanPos;
}

bool Scanner::nextString()
{
	nextState.tokenLine = line;
	nextState.tokenLinePosition = scanPos - lineStart;
	nextState.token = TK_NoToken;
	if(!needNext)
		scanPos = state.scanPos;
	checkForWhitespace();
	if(scanPos >= length)
		return false;

	int start = scanPos;
	int end = scanPos;
	bool quoted = data[scanPos] == '"';
	if(quoted) // String Constant
	{
		end = ++start; // Remove starting quote
		scanPos++;
		while(scanPos < length)
		{
			char cur = data[scanPos];
			if(cur == '"')
				end = scanPos;
			else if(cur == '\\')
			{
				scanPos += 2;
				continue;
			}
			scanPos++;
			if(start != end)
				break;
		}
	}
	else // Unquoted string
	{
		while(scanPos < length)
		{
			char cur = data[scanPos];
			switch(cur)
			{
				default:
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					end = scanPos;
					break;
			}
			if(start != end)
				break;
			scanPos++;
		}
		if(scanPos == length)
			end = scanPos;
	}
	if(end-start > 0)
	{
		nextState.scanPos = scanPos;
		QString thisString = QString::fromAscii(data+start, end-start);
		if(quoted)
			unescape(thisString);
		nextState.str = thisString;
		nextState.token = TK_StringConst;
		expandState();
		needNext = true;
		return true;
	}
	checkForWhitespace();
	return false;
}

bool Scanner::nextToken(bool autoExpandState)
{
	if(!needNext)
	{
		needNext = true;
		if(autoExpandState)
			expandState();
		return true;
	}

	nextState.tokenLine = line;
	nextState.tokenLinePosition = scanPos - lineStart;
	nextState.token = TK_NoToken;
	if(scanPos >= length)
	{
		if(autoExpandState)
			expandState();
		return false;
	}

	unsigned int start = scanPos;
	unsigned int end = scanPos;
	int integerBase = 10;
	bool floatHasDecimal = false;
	bool floatHasExponent = false;
	bool stringFinished = false; // Strings are the only things that can have 0 length tokens.

	char cur = data[scanPos++];
	// Determine by first character
	if(cur == '_' || (cur >= 'A' && cur <= 'Z') || (cur >= 'a' && cur <= 'z'))
		nextState.token = TK_Identifier;
	else if(cur >= '0' && cur <= '9')
	{
		if(cur == '0')
			integerBase = 8;
		nextState.token = TK_IntConst;
	}
	else if(cur == '.' && scanPos < length && data[scanPos] != '.')
	{
		floatHasDecimal = true;
		nextState.token = TK_FloatConst;
	}
	else if(cur == '"')
	{
		end = ++start; // Move the start up one character so we don't have to trim it later.
		nextState.token = TK_StringConst;
	}
	else
	{
		end = scanPos;
		nextState.token = cur;

		// Now check for operator tokens
		if(scanPos < length)
		{
			char next = data[scanPos];
			if(cur == '&' && next == '&')
				nextState.token = TK_AndAnd;
			else if(cur == '|' && next == '|')
				nextState.token = TK_OrOr;
			else if(
				(cur == '<' && next == '<') ||
				(cur == '>' && next == '>')
			)
			{
				// Next for 3 character tokens
				if(scanPos+1 > length && data[scanPos+1] == '=')
				{
					scanPos++;
					nextState.token = cur == '<' ? TK_ShiftLeftEq : TK_ShiftRightEq;
					
				}
				else
					nextState.token = cur == '<' ? TK_ShiftLeft : TK_ShiftRight;
			}
			else if(cur == '#' && next == '#')
				nextState.token = TK_MacroConcat;
			else if(cur == ':' && next == ':')
				nextState.token = TK_ScopeResolution;
			else if(cur == '+' && next == '+')
				nextState.token = TK_Increment;
			else if(cur == '-')
			{
				if(next == '-')
					nextState.token = TK_Decrement;
				else if(next == '>')
					nextState.token = TK_PointerMember;
			}
			else if(cur == '.' && next == '.' &&
				scanPos+1 < length && data[scanPos+1] == '.')
			{
				nextState.token = TK_Ellipsis;
				++scanPos;
			}
			else if(next == '=')
			{
				switch(cur)
				{
					case '=':
						nextState.token = TK_EqEq;
						break;
					case '!':
						nextState.token = TK_NotEq;
						break;
					case '>':
						nextState.token = TK_GtrEq;
						break;
					case '<':
						nextState.token = TK_LessEq;
						break;
					case '+':
						nextState.token = TK_AddEq;
						break;
					case '-':
						nextState.token = TK_SubEq;
						break;
					case '*':
						nextState.token = TK_MulEq;
						break;
					case '/':
						nextState.token = TK_DivEq;
						break;
					case '%':
						nextState.token = TK_ModEq;
						break;
					case '&':
						nextState.token = TK_AndEq;
						break;
					case '|':
						nextState.token = TK_OrEq;
						break;
					case '^':
						nextState.token = TK_XorEq;
						break;
					default:
						break;
				}
			}

			if(nextState.token != cur)
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
			switch(nextState.token)
			{
				default:
					break;
				case TK_Identifier:
					if(cur != '_' && (cur < 'A' || cur > 'Z') && (cur < 'a' || cur > 'z') && (cur < '0' || cur > '9'))
						end = scanPos;
					break;
				case TK_IntConst:
					if(cur == '.' || (scanPos-1 != start && cur == 'e'))
						nextState.token = TK_FloatConst;
					else if((cur == 'x' || cur == 'X') && scanPos-1 == start)
					{
						integerBase = 16;
						break;
					}
					else
					{
						switch(integerBase)
						{
							default:
								if(cur < '0' || cur > '9')
									end = scanPos;
								break;
							case 8:
								if(cur < '0' || cur > '7')
									end = scanPos;
								break;
							case 16:
								if((cur < '0' || cur > '9') && (cur < 'A' || cur > 'F') && (cur < 'a' || cur > 'f'))
									end = scanPos;
								break;
						}
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
		// Handle small tokens at the end of a file.
		if(scanPos == length && !stringFinished)
			end = scanPos;
	}

	nextState.scanPos = scanPos;
	if(end-start > 0 || stringFinished)
	{
		nextState.str = QByteArray(data+start, end-start);
		if(nextState.token == TK_FloatConst)
		{
			if(floatHasDecimal && nextState.str.length() == 1)
			{
				// Don't treat a lone '.' as a decimal.
				nextState.token = '.';
			}
			else
			{
				nextState.decimal = nextState.str.toDouble(NULL);
				nextState.number = static_cast<int> (nextState.decimal);
				nextState.boolean = (nextState.number != 0);
			}
		}
		else if(nextState.token == TK_IntConst)
		{
			nextState.number = nextState.str.toUInt(NULL, integerBase);
			nextState.decimal = nextState.number;
			nextState.boolean = (nextState.number != 0);
		}
		else if(nextState.token == TK_Identifier)
		{
			// Check for a boolean constant.
			if(nextState.str.compare("true") == 0)
			{
				nextState.token = TK_BoolConst;
				nextState.boolean = true;
			}
			else if(nextState.str.compare("false") == 0)
			{
				nextState.token = TK_BoolConst;
				nextState.boolean = false;
			}
		}
		else if(nextState.token == TK_StringConst)
		{
			nextState.str = unescape(nextState.str);
		}
		if(autoExpandState)
			expandState();
		return true;
	}
	nextState.token = TK_NoToken;
	if(autoExpandState)
		expandState();
	return false;
}

void Scanner::mustGetToken(char token)
{
	if(!checkToken(token))
	{
		expandState();
		if(token < TK_NumSpecialTokens && state.token < TK_NumSpecialTokens)
			scriptMessage(Scanner::ERROR, "Expected '%s' but got '%s' instead.", TokenNames[token], TokenNames[state.token]);
		else if(token < TK_NumSpecialTokens && state.token >= TK_NumSpecialTokens)
			scriptMessage(Scanner::ERROR, "Expected '%s' but got '%c' instead.", TokenNames[token], state.token);
		else if(token >= TK_NumSpecialTokens && state.token < TK_NumSpecialTokens)
			scriptMessage(Scanner::ERROR, "Expected '%c' but got '%s' instead.", token, TokenNames[state.token]);
		else
			scriptMessage(Scanner::ERROR, "Expected '%c' but got '%c' instead.", token, state.token);
	}
}

void Scanner::rewind()
{
	needNext = false;
	nextState = state;
	state = prevState;
	scanPos = state.scanPos;

	line = prevState.tokenLine;
	logicalPosition = prevState.tokenLinePosition;
}

void Scanner::scriptMessage(MessageLevel level, const char* error, ...) const
{
	const char* messageLevel;
	switch(level)
	{
		default:
			messageLevel = "Notice";
			break;
		case WARNING:
			messageLevel = "Warning";
			break;
		case ERROR:
			messageLevel = "Error";
			break;
	}

	char* newMessage = new char[strlen(error) + scriptIdentifier.length() + 25];
	sprintf(newMessage, "%s:%d:%d:%s: %s\n", scriptIdentifier.toAscii().constData(), currentLine(), currentLinePos(), messageLevel, error);
	va_list list;
	va_start(list, error);
	if(messageHandler)
		messageHandler(level, newMessage, list);
	else
		vfprintf(stderr, newMessage, list);
	va_end(list);
	delete[] newMessage;

	if(!messageHandler && level == ERROR)
		exit(0);
}

int Scanner::skipLine()
{
	int ret = currentPos();
	while(logicalPosition < length)
	{
		char thisChar = data[logicalPosition];
		char nextChar = logicalPosition+1 < length ? data[logicalPosition+1] : 0;
		if(thisChar == '\n' || thisChar == '\r')
		{
			ret = logicalPosition++; // Return the first newline character we see.
			if(nextChar == '\r')
				logicalPosition++;
			incrementLine();
			checkForWhitespace();
			break;
		}
		logicalPosition++;
	}
	if(logicalPosition > scanPos)
	{
		scanPos = logicalPosition;
		checkForWhitespace();
		needNext = true;
		logicalPosition = scanPos;
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
// NOTE: Be sure that '\\' is the first thing in the array otherwise it will re-escape.
static char escapeCharacters[] = {'\\', '"', 0};
const QString& Scanner::escape(QString &str)
{
	for(unsigned int i = 0;escapeCharacters[i] != 0;i++)
	{
		// += 2 because we'll be inserting 1 character.
		for(int p = 0;p < str.length() && (p = str.indexOf(escapeCharacters[i], p)) != -1;p += 2)
		{
			str.insert(p, '\\');
		}
	}
	return str;
}
const QString& Scanner::unescape(QString &str)
{
	for(unsigned int i = 0;escapeCharacters[i] != 0;i++)
	{
		QString sequence = "\\" + QString(escapeCharacters[i]);
		for(int p = 0;p < str.length() && (p = str.indexOf(sequence, p)) != -1;p++)
			str.replace(str.indexOf(sequence, p), 2, escapeCharacters[i]);
	}
	return str;
}

//------------------------------------------------------------------------------
// ircresponseparseresult.cpp
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ircresponseparseresult.h"

#include "irc/constants/ircresponsetype.h"

// This is returned by IRCResponseParseResult::type() if the result is invalid.
// Do not modify contents of this var!
const IRCResponseType invalidResponseType;

IRCResponseParseResult::IRCResponseParseResult()
{
	d.pResponseType = NULL;
	d.bWasParsed = false;
}

IRCResponseParseResult::IRCResponseParseResult(const IRCResponseType& responseType, bool bWasParsed)
{
	d.bWasParsed = bWasParsed;
	d.pResponseType = new IRCResponseType(responseType);
}

IRCResponseParseResult::IRCResponseParseResult(const IRCResponseParseResult& other)
{
	d.pResponseType = NULL;
	copyIn(other);
}

IRCResponseParseResult::~IRCResponseParseResult()
{
	if (d.pResponseType != NULL)
	{
		delete d.pResponseType;
	}
}

void IRCResponseParseResult::copyIn(const IRCResponseParseResult& other)
{
	d.bWasParsed = other.d.bWasParsed;
	if (d.pResponseType != NULL)
	{
		delete d.pResponseType;
		d.pResponseType = NULL;
	}

	if (other.d.pResponseType != NULL)
	{
		d.pResponseType = new IRCResponseType(*other.d.pResponseType);
	}
}

IRCResponseParseResult& IRCResponseParseResult::operator=(const IRCResponseParseResult& other)
{
	if (this != &other)
	{
		copyIn(other);
	}

	return *this;
}

const IRCResponseType& IRCResponseParseResult::type() const
{
	if (d.pResponseType == NULL)
	{
		return invalidResponseType;
	}

	return *d.pResponseType;
}

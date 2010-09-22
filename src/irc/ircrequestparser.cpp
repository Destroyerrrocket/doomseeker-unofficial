//------------------------------------------------------------------------------
// ircrequestparser.cpp
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
#include "ircrequestparser.h"
#include "irc/ircclient.h"
#include <QStringList>

IRCRequestParser::IRCRequestParseResult IRCRequestParser::parse(QString input, QString& output)
{
	input = input.trimmed();

	if (input.isEmpty())
	{
		return ErrorMessageEmpty;
	}
	
	if (input[0] != '/')
	{
		return ErrorInputNotPrependedWithSlash;
	}
	
	input.remove(0, 1);
	if (input.isEmpty())
	{
		return ErrorMessageEmpty;
	}
	
	if (input.length() > IRCClient::MAX_MESSAGE_LENGTH)
	{
		return ErrorMessageTooLong;
	}
	
	QStringList inputSplit = input.split(" ", QString::SkipEmptyParts);
	QString message = inputSplit.takeFirst();
	message = message.toUpper();
	
	// As of this point inputSplit param list doesn't contain the message
	// type anymore.
	
	if (message == "QUIT")
	{
		output = QString("%1 :%2").arg(message, inputSplit.join(" "));
		return QuitCommand;	
	}
	else if (message == "PART")
	{
		if (inputSplit.isEmpty())
		{
			return ErrorInputInsufficientParameters;
		}
	
		QString channel = inputSplit.takeFirst();
		QString farewellMessage = inputSplit.join(" ");
		output = QString("%1 %2 :%3").arg(message, channel, farewellMessage);
	}
	else if (message == "KICK")
	{
		if (inputSplit.length() < 2)
		{
			return ErrorInputInsufficientParameters;
		}
	
		QString channel = inputSplit.takeFirst();
		QString user = inputSplit.takeFirst();
		QString reason = inputSplit.join(" ");
		output = QString("%1 %2 %3 :%4").arg(message, channel, user, reason);
	}
	else
	{
		output = QString("%1 %2").arg(message, inputSplit.join(" "));
	}
	
	return Ok;
}

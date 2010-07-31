//------------------------------------------------------------------------------
// ircresponseparser.cpp
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
#include "ircresponseparser.h"

#include <QRegExp>
#include <QStringList>
#include "log.h"
#include "strings.h"

void IRCResponseParser::parse(const QString& message)
{
	QString formattedMessage = message.trimmed();

	QRegExp prefixRegExp("(^:\\S+\\s)(.*)");
	prefixRegExp.indexIn(formattedMessage);
	
	QString prefix = prefixRegExp.cap(1);
	QString remainder = formattedMessage.mid(prefix.length());
	
	prefix = Strings::triml(prefix, ":");
	
	// Obtain message sender from the prefix.
	QString sender;
	int indexExclamation = prefix.indexOf('!');
	if (indexExclamation > 0)
	{
		sender = prefix.left(indexExclamation);
	}
	else
	{
		sender = prefix;
	}

	// Parse the actual message.	
	if (remainder.startsWith("PING "))
	{
		QStringList splitString = remainder.split(" ");
		QString pongToWhom = splitString[1];
		
		emit sendPongMessage(pongToWhom);
	}		
	else if (remainder.startsWith("NICK "))
	{
		QString oldNickname = sender;
		QString newNickname = remainder.split(" ")[1];
		newNickname = Strings::triml(newNickname, ":");
		
		emit userChangesNickname(oldNickname, newNickname);
	}
	else if (remainder.startsWith("JOIN "))
	{
		QString channel = remainder.split(" ")[1];
		channel = Strings::triml(channel, ":");
		
		emit userJoinsChannel(channel, sender, prefix);
	}
	else if (remainder.startsWith("PART "))
	{
		QString channel = remainder.split(" ")[1];
		channel = Strings::triml(channel, ":");
		
		emit userPartsChannel(channel, sender);
	}
}

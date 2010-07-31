//------------------------------------------------------------------------------
// ircchatadapter.cpp
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
#include "ircchatadapter.h"

#include "irc/ircnetworkadapter.h"

IRCChatAdapter::IRCChatAdapter(IRCNetworkAdapter* pNetwork)
{
	this->pNetwork = pNetwork;
}

void IRCChatAdapter::doSendMessage(const QString& message, IRCAdapterBase* pOrigin)
{
	if (message.startsWith("/"))
	{
		pNetwork->doSendMessage(message, this);
	}
	else
	{
		sendChatMessage(message);
	}
}


QString IRCChatAdapter::extractMessageLine(QStringList& words, int maxLength)
{
	QString sentence = "";

	while(!words.isEmpty())
	{
		QString word = words.first();
	
		// Remember to add the spacebar into the equation.
		if (sentence.length() + word.length() + 1 > maxLength)
		{
			if (sentence.isEmpty())
			{
				// We need to split the word.
				QString splitWord = word.left(maxLength);
				splitWord[maxLength - 1] = '-';
				
				QString wordRemainder = word.right(word.size() - maxLength);
				words.prepend(wordRemainder);
				
				sentence = splitWord;
			}

			break;
		}
		
		sentence += word + " ";
		words.pop_front();
	}
	
	return sentence.trimmed();
}

void IRCChatAdapter::sendChatMessage(const QString& message)
{
	// Here we will split too long messages to make sure they don't
	// exceed the 512 character limit as stated in RFC 1459.
	
	QString ircCall = QString("/PRIVMSG %1 ").arg(recipient);
	
	int maxLength = IRCClient::MAX_MESSAGE_LENGTH - ircCall.length();
	
	QStringList wordLines = message.split("\n");
	
	foreach(QString line, wordLines)
	{
		QStringList words = line.split(" ");
		
		while (!words.isEmpty())
		{
			QString sentence = extractMessageLine(words, maxLength);
			pNetwork->doSendMessage(ircCall + sentence, this);
		}
	}
}

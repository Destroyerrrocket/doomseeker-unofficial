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

IRCChatAdapter::IRCChatAdapter(IRCNetworkAdapter* pNetwork, const QString& recipient)
{
	this->pNetwork = pNetwork;
	this->recipientName = recipient;
}

IRCChatAdapter::~IRCChatAdapter()
{
	if (this->pNetwork != NULL)
	{
		// Prevent the situation where this->pNetwork is
		// NULLified while we still may need it.
		IRCNetworkAdapter* pTmpNetwork = this->pNetwork;

		pTmpNetwork->detachChatWindow(this);
	}
}

void IRCChatAdapter::doSendMessage(const QString& message, IRCAdapterBase* pOrigin)
{
	// If network is null and we can still send messages this might be a bug 
	// in the application.
	if (pNetwork == NULL)
	{
		emit error("This chat window is not attached to any network. This is probably a bug in Doomseeker.");
		return;
	}

	// Trim whitespaces to make sure the '/' character gets detected properly.
	QString messageTrimmed = message.trimmed();

	if (messageTrimmed.startsWith("/"))
	{
		// This is a IRC comand. Send it as is to the network.
		pNetwork->doSendMessage(messageTrimmed, this);
	}
	else
	{
		// This is a chat message. We need to process this before
		// sending.
		sendChatMessage(messageTrimmed);
	}
}

void IRCChatAdapter::emitChatMessage(const QString& sender, const QString& content)
{
	emit message(QString("<%1>: %2").arg(sender, content));
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
			// If there is no valid way of splitting the sentence; if
			// the word itself is longer than maxLength we have to split
			// the word. Dash will be appened to the end of such word.
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
	
	QString ircCall = QString("/PRIVMSG %1 :").arg(recipientName);
	int maxLength = IRCClient::MAX_MESSAGE_LENGTH - ircCall.length();
	QStringList wordLines = message.split("\n");
	
	foreach(QString line, wordLines)
	{
		QStringList words = line.split(" ");
		
		while (!words.isEmpty())
		{
			QString sentence = extractMessageLine(words, maxLength);
			pNetwork->doSendMessage(ircCall + sentence, this);

			// Since IRC doesn't do this we need to echo the sent 
			// message back to the client.
			const QString& myName = pNetwork->myNickname();
			emitChatMessage(myName, sentence);
		}
	}
}

void IRCChatAdapter::setNetwork(IRCNetworkAdapter* pNetwork)
{
	this->pNetwork = pNetwork;
}

void IRCChatAdapter::setRecipient(const QString& name)
{
	this->recipientName = name;
	emit titleChange();
}

QString IRCChatAdapter::title() const
{
	return recipientName;
}

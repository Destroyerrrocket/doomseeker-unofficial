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
#include "irc/ircglobal.h"
#include "log.h"
#include "strings.h"

void IRCResponseParser::parse(const QString& message)
{
	// Remember to add a spacebar after each string here.
	static const QString RPL_NAMREPLY = "353 ";
	static const QString RPL_ENDOFNAMES = "366 ";

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

	QStringList msgParameters = remainder.split(" ");

	// Parse the actual message.	
	if (remainder.startsWith("PING "))
	{
		QString pongToWhom = msgParameters[1];
		
		emit sendPongMessage(pongToWhom);
	}		
	else if (remainder.startsWith("NICK "))
	{
		QString oldNickname = sender;
		QString newNickname = msgParameters[1];
		newNickname = Strings::triml(newNickname, ":");
		
		emit userChangesNickname(oldNickname, newNickname);
	}
	else if (remainder.startsWith("JOIN "))
	{
		QString channel = msgParameters[1];
		channel = Strings::triml(channel, ":");
		
		emit userJoinsChannel(channel, sender, prefix);
	}
	else if (remainder.startsWith("PART "))
	{
		QString farewellMessage = QString();
		QString channel = msgParameters[1];
		
		if (msgParameters.size() > 2)
		{
			msgParameters.pop_front();
			msgParameters.pop_front();

			farewellMessage = msgParameters.join(" ");
			if (farewellMessage[0] == ':')
			{
				farewellMessage.remove(0, 1);
			}
		}
		
		emit userPartsChannel(channel, sender, farewellMessage);
	}
	else if (remainder.startsWith("PRIVMSG "))
	{
		QString recipient = msgParameters[1];

		// Remove the two now redundant parameters from the list
		msgParameters.pop_front();
		msgParameters.pop_front();

		// Join the list to form message contents.
		QString content = msgParameters.join(" ");
		if (content[0] == ':')
		{
			content = content.remove(0, 1);
		}

		if (!IRCGlobal::isChannelName(recipient))
		{
			// If recipient name is not the channel the
			// "recipient" QString will point to this client's user.
			// In order to get a proper recipient we need to use the
			// "sender" QString instead.
			recipient = sender;
		}

		emit privMsgReceived(recipient, sender, content);
	}
	else if (remainder.startsWith("QUIT "))
	{
		msgParameters.pop_front();
		// This is the :Quit: part of the message (??)
		msgParameters.pop_front();

		QString farewellMessage = QString();
		if (!msgParameters.isEmpty())
		{
			farewellMessage = msgParameters.join(" ");
			if (farewellMessage[0] == ':')
			{
				farewellMessage.remove(0, 1);
			}
		}

		emit userQuitsNetwork(sender, farewellMessage);
	}
	else if (remainder.startsWith(RPL_NAMREPLY))
	{
		// Namelists.
		// Remove the message type signature.
		msgParameters.pop_front();

		// Attempt to extract the channel name. For some reason 
		// irc.skulltag.net returns a '=' character between the message type 
		// signature and the channel name. RFC 1459 doesn't say anything about
		// such behavior, at least not in the chapter 6. We should protect 
		// ourselves from such unwelcome surprises.
		QString channelName = "";
		while (!IRCGlobal::isChannelName(channelName) && !msgParameters.isEmpty())
		{
			channelName = msgParameters.takeFirst();
		}

		if (channelName.isEmpty())
		{
			emit parseError(tr("Received name list but no channel name."));
			return;
		}

		// Remaining values will be user names. Send them all as a strings list.
		// Remember to remove the ":" character from the first name.
		if (!msgParameters.isEmpty())
		{
			msgParameters[0] = msgParameters[0].remove(0, 1);
		}

		emit namesListReceived(channelName, msgParameters);
	}
	else if (remainder.startsWith(RPL_ENDOFNAMES))
	{
		QString channel = msgParameters[2];
		emit namesListEndReceived(channel);
	}
}

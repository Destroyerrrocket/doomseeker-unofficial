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

#include <QDateTime>
#include <QRegExp>
#include <QStringList>
#include "irc/constants/ircresponsetype.h"
#include "irc/ircglobal.h"
#include "irc/ircmessageclass.h"
#include "irc/ircuserinfo.h"
#include "log.h"
#include "strings.h"

IRCResponseParser::FlagModes IRCResponseParser::getFlagMode(char c)
{
	switch (c)
	{
		case '+':
			return FlagModeAdd;

		case '-':
			return FlagModeRemove;

		default:
			return FlagModeError;
	}
}

QString IRCResponseParser::joinAndTrimColonIfNecessary(const QStringList& strList) const
{
	QString joined = strList.join(" ");
	return this->trimColonIfNecessary(joined);
}

IRCResponseParseResult IRCResponseParser::parse(const QString& message)
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

	QStringList msgParameters = remainder.split(" ");
	if (!msgParameters.isEmpty())
	{
		QString type = msgParameters[0];
		msgParameters.pop_front();

		return parseMessage(prefix, sender, type, msgParameters);
	}

	// Return invalid result.
	return IRCResponseParseResult();
}

IRCResponseParseResult IRCResponseParser::parseMessage(const QString& prefix, const QString& sender, const QString& type, QStringList params)
{
	IRCResponseType responseType(type);
	IRCResponseType::MsgType enumType = responseType.type();

	switch (enumType)
	{
		case IRCResponseType::HelloClient:
		{
			QString nickname = params.takeFirst();

			emit helloClient(nickname);
			break;
		}

		case IRCResponseType::RPLWhoIsUser:
		{
			// First param is unnecessary
			params.takeFirst();

			// Extract user info.
			QString nickname = params.takeFirst();
			QString user = params.takeFirst();
			QString hostName = params.takeFirst();
			QString realName = joinAndTrimColonIfNecessary(params);

			emit whoIsUser(nickname, user, hostName, realName);
			break;
		}

		case IRCResponseType::RPLTopic:
		{
			params.takeFirst(); // Own nickname.
			QString channel = params.takeFirst();
			QString topic = joinAndTrimColonIfNecessary(params);
			QString msg = tr("Topic: %1").arg(topic);
			emit printWithClass(msg, channel, IRCMessageClass(IRCMessageClass::ChannelAction));
			break;
		}

		case IRCResponseType::RPLTopicWhoTime:
		{
			params.takeFirst(); // Own nickname.
			QString channel = params.takeFirst();
			QString who = params.takeFirst();
			qint64 timestampSeconds = params.takeFirst().toLongLong();
			QDateTime date = QDateTime::fromMSecsSinceEpoch(timestampSeconds * 1000);
			QString msg = tr("Topic set by %1 on %2.").arg(who, date.toString("yyyy-MM-dd hh:mm:ss"));
			emit printWithClass(msg, channel, IRCMessageClass(IRCMessageClass::ChannelAction));
			break;
		}

		case IRCResponseType::RPLNamReply:
		{
			// Namelists.

			// Attempt to extract the channel name. For some reason
			// irc.skulltag.net returns a '=' character between the message type
			// signature and the channel name. RFC 1459 doesn't say anything about
			// such behavior, at least not in the chapter 6. We should protect
			// ourselves from such unwelcome surprises.
			QString channelName = "";
			while (!IRCGlobal::isChannelName(channelName) && !params.isEmpty())
			{
				channelName = params.takeFirst();
			}

			if (channelName.isEmpty())
			{
				emit parseError(tr("RPLNamReply: Received names list but no channel name."));
				return IRCResponseParseResult();
			}

			// Remaining values will be user names. Send them all as a strings list.
			// Remember to remove the ":" character from the first name.
			if (!params.isEmpty())
			{
				params[0] = params[0].remove(0, 1);
			}

			emit namesListReceived(channelName, params);
			break;
		}

		case IRCResponseType::RPLEndOfNames:
		{
			QString channel = params[1];
			emit namesListEndReceived(channel);
			break;
		}

		case IRCResponseType::RPLMOTDStart:
		case IRCResponseType::RPLMOTD:
		case IRCResponseType::RPLEndOfMOTD:
		{
			// First param is username, drop it.
			params.takeFirst();

			if (enumType == IRCResponseType::RPLMOTDStart)
			{
				// We will print additional separator line if it's a start of
				// the MOTD.
				emit print("\n----------------------", QString());
			}

			emit print(joinAndTrimColonIfNecessary(params), QString());

			if (enumType == IRCResponseType::RPLEndOfMOTD)
			{
				// Again, we will print additional separator line if it's the
				// end of the MOTD.
				emit print("----------------------\n", QString());
			}

			break;
		}

		// Extract correct message and print it.
		case IRCResponseType::RPLLUserClient:
		case IRCResponseType::RPLLUserMe:
		{
			// Drop the first param.
			params.takeFirst();

			// Join and print the rest.
			emit print(joinAndTrimColonIfNecessary(params), QString());
			break;
		}

		// Extract correct message and print it.
		case IRCResponseType::RPLLUserOp:
		case IRCResponseType::RPLLUserUnknown:
		case IRCResponseType::RPLLUserChannels:
		{
			// Drop the first param.
			params.takeFirst();

			// Here the first param is always an integer, and colon is located
			// afterwards.
			QString number = params.takeFirst();
			emit print(number + " " + joinAndTrimColonIfNecessary(params), QString());
			break;
		}

		case IRCResponseType::ERRNoSuchNick:
		{
			// Drop the first param.
			params.takeFirst();

			// This is the real nickname.
			QString nickname = params.takeFirst();

			emit noSuchNickname(nickname);
			break;
		}

		case IRCResponseType::ERRNicknameInUse:
		{
			// Drop the first param.
			params.takeFirst();

			QString nickname = params.takeFirst();

			emit nicknameInUse(nickname);
			break;
		}

		case IRCResponseType::ERRChanOpPrivIsNeeded:
		{
			params.takeFirst(); // User
			QString channel = params.takeFirst();
			QString reason = joinAndTrimColonIfNecessary(params);
			emit printWithClass(reason, channel, IRCMessageClass(IRCMessageClass::ChannelAction));
			break;
		}

		case IRCResponseType::Join:
		{
			QString channel = params[0];
			trimColonIfNecessary(channel);

			emit userJoinsChannel(channel, sender, prefix);
			break;
		}

		case IRCResponseType::Kick:
		{
			QString channel = params[0];
			QString whoIsKicked = params[1];
			params.pop_front();
			params.pop_front();

			QString reason = joinAndTrimColonIfNecessary(params);

			emit kick(channel, sender, whoIsKicked, reason);
			break;
		}

		case IRCResponseType::Kill:
		{
			QString victim = params[0];
			params.pop_front();

			QString comment = joinAndTrimColonIfNecessary(params);

			emit kill(victim, comment);
			break;
		}

		case IRCResponseType::Mode:
		{
			QString channel = params[0];
			QString flagsString = params[1];
			params.pop_front();
			params.pop_front();

			// If there are no more params left on the list then this modes
			// are for the channel itself. Otherwise they are for the users.
			if (!params.isEmpty())
			{
				emit modeInfo(channel, sender, flagsString + " " + params.join(" "));
				parseUserModeMessage(channel, flagsString, params);
			}

			break;
		}

		case IRCResponseType::Nick:
		{
			QString oldNickname = sender;
			QString newNickname = params[0];
			trimColonIfNecessary(newNickname);

			emit userChangesNickname(oldNickname, newNickname);
			break;
		}

		case IRCResponseType::Part:
		{
			QString farewellMessage = QString();
			QString channel = params[0];

			if (params.size() > 1)
			{
				params.pop_front();

				farewellMessage = joinAndTrimColonIfNecessary(params);
			}

			emit userPartsChannel(channel, sender, farewellMessage);
			break;
		}

		case IRCResponseType::Ping:
		{
			QString pongToWhom = params[0];

			emit sendPongMessage(pongToWhom);
			break;
		}

		case IRCResponseType::PrivMsg:
		case IRCResponseType::Notice:
		{
			QString recipient = params[0];

			// Remove the now redundant parameter from the list
			params.pop_front();

			// Join the list to form message contents.
			QString content = joinAndTrimColonIfNecessary(params);

			if (!IRCGlobal::isChannelName(recipient))
			{
				// If recipient name is not the channel the
				// "recipient" QString will point to this client's user.
				// In order to get a proper recipient we need to use the
				// "sender" QString instead.
				recipient = sender;
			}

			if (responseType == IRCResponseType::PrivMsg)
			{
				emit privMsgReceived(recipient, sender, content);
			}
			else if (responseType == IRCResponseType::Notice)
			{
				emit print(QString("[%1]: %2").arg(sender, content), QString());
			}
			else
			{
				emit parseError(tr("Type '%1' was incorrectly parsed in PrivMsg block.").arg(type));
			}

			break;
		}

		case IRCResponseType::Quit:
		{
			// This is the :Quit: part of the message (??)
			params.pop_front();

			QString farewellMessage = QString();
			farewellMessage = joinAndTrimColonIfNecessary(params);

			emit userQuitsNetwork(sender, farewellMessage);
			break;
		}

		case IRCResponseType::Topic:
		{
			QString channel = params.takeFirst();
			QString topic = joinAndTrimColonIfNecessary(params);
			QString msg = tr("New topic set by user %1:\n%2").arg(sender, topic);
			emit printWithClass(msg, channel, IRCMessageClass(IRCMessageClass::ChannelAction));
			break;
		}

		case IRCResponseType::Invalid:
		{
			// Messages below 100 are some generic server responses to connect
			// event.
			if (responseType.numericType() > 1)
			{
				emit print(joinAndTrimColonIfNecessary(params), "");
				return IRCResponseParseResult(responseType, true);
			}

			return IRCResponseParseResult(responseType, false);
		}

		default:
			emit parseError(tr(
				"IRCResponseParser: Type '%1' was recognized but there has been no parse code implemented for it.\
(yep, it's a bug in the application!)"
				).arg(type));
			return IRCResponseParseResult(responseType, true);


	}

	return IRCResponseParseResult(responseType, true);
}

void IRCResponseParser::parseUserModeMessage(const QString& channel, QString flagsString, QStringList& nicknames)
{
	// For each flag character there should be one nickname on the list.
	// If there are less nicknames than characters we will simply abort.
	// Of course add/subtract characters are not counted here.

	// The first character should always define the flagMode.
	FlagModes flagMode = getFlagMode(flagsString[0].toAscii());

	if (flagMode == FlagModeError)
	{
		emit parseError(tr("MODE flags string from IRC server are incorrect: \"%1\". Information for channel \"%2\" might not be correct anymore.").arg(flagsString, channel));
		return;
	}

	for (int i = 1; i < flagsString.size(); ++i)
	{
		char flagChar = flagsString[i].toAscii();

		FlagModes tmpFlagMode = getFlagMode(flagChar);
		if (tmpFlagMode == FlagModeError)
		{
			if (nicknames.empty())
			{
				return;
			}

			unsigned flag = IRCUserInfo::convertModeCharToFlag(flagChar);
			if (flag != 0)
			{
				unsigned flagsAdded = 0;
				unsigned flagsRemoved = 0;

				QString name = nicknames[0];

				switch (flagMode)
				{
					case FlagModeAdd:
						flagsAdded = flag;
						break;

					case FlagModeRemove:
						flagsRemoved = flag;
						break;

					default:
						emit parseError(tr("IRCResponseParser::parseUserModeMessage(): wrong FlagMode. Information for channel \"%2\" might not be correct anymore."));
						return;
				}

				emit userModeChanged(channel, name, flagsAdded, flagsRemoved);
			}

			// Drop a name from the list and continue.
			nicknames.pop_front();
		}
		else
		{
			flagMode = tmpFlagMode;
			continue;
		}
	}
}

QString& IRCResponseParser::trimColonIfNecessary(QString& str) const
{
	if (!str.isEmpty() && str[0] == ':')
	{
		str.remove(0, 1);
	}

	return str;
}

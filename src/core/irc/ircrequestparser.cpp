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
#include "irc/ircadapterbase.h"
#include "irc/ircclient.h"
#include "irc/ircctcpparser.h"
#include "irc/ircglobal.h"
#include "irc/ircmessageclass.h"
#include "irc/ircnetworkadapter.h"
#include "irc/ircuserinfo.h"
#include <QStringList>

class IRCRequestParser::PrivData
{
	public:
		IRCAdapterBase *adapter;
		QString output;
		QString message;
		QStringList tokens;
};

IRCRequestParser::IRCRequestParser()
{
	d = new PrivData();
	d->adapter = NULL;
}

IRCRequestParser::~IRCRequestParser()
{
	delete d;
}

IRCNetworkAdapter *IRCRequestParser::network()
{
	return d->adapter->network();
}

const QString &IRCRequestParser::output() const
{
	return d->output;
}

IRCRequestParser::IRCRequestParseResult IRCRequestParser::parse(IRCAdapterBase* pAdapter, QString input)
{
	d->adapter = pAdapter;
	d->output.clear();
	d->tokens.clear();
	d->message.clear();

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

	QStringList inputSplit = input.split(" ", QString::SkipEmptyParts);
	d->message = inputSplit.takeFirst().toUpper();
	d->tokens = inputSplit;

	IRCRequestParser::IRCRequestParseResult result = buildOutput();
	if (result == Ok)
	{
		if (d->output.toUtf8().length() > IRCClient::MAX_MESSAGE_LENGTH)
		{
			return ErrorMessageTooLong;
		}
	}

	return result;
}

IRCRequestParser::IRCRequestParseResult IRCRequestParser::buildOutput()
{
	if (d->message == "QUIT")
	{
		d->output = QString("%1 :%2").arg(d->message, d->tokens.join(" "));
		return QuitCommand;
	}
	else if (d->message == "PART")
	{
		if (d->tokens.isEmpty())
		{
			return ErrorInputInsufficientParameters;
		}

		QString channel = d->tokens.takeFirst();
		QString farewellMessage = d->tokens.join(" ");
		d->output = QString("%1 %2 :%3").arg(d->message, channel, farewellMessage);
	}
	else if (d->message == "KICK")
	{
		if (d->tokens.length() < 2)
		{
			return ErrorInputInsufficientParameters;
		}

		QString channel = d->tokens.takeFirst();
		QString user = d->tokens.takeFirst();
		QString reason = d->tokens.join(" ");
		d->output = QString("%1 %2 %3 :%4").arg(d->message, channel, user, reason);
	}
	else if (d->message == "PRIVMSG" || d->message == "NOTICE")
	{
		// Notice and Privmsg are handled the same way.
		if (d->tokens.length() < 2)
		{
			return ErrorInputInsufficientParameters;
		}

		QString recipient = d->tokens.takeFirst();
		QString content = d->tokens.join(" ");
		d->output = QString("%1 %2 :%3").arg(d->message, recipient, content);
		if (d->message == "PRIVMSG")
		{
			IRCCtcpParser ctcp(network(), network()->myNickname(),
				recipient, content, IRCCtcpParser::Send);
			if (ctcp.parse())
			{
				switch (ctcp.echo())
				{
					case IRCCtcpParser::PrintAsNormalMessage:
						network()->printMsgLiteral(recipient, ctcp.printable(), IRCMessageClass::Ctcp);
						break;
					case IRCCtcpParser::DisplayInServerTab:
						network()->printWithClass(ctcp.printable(), QString(), IRCMessageClass::Ctcp);
						break;
					case IRCCtcpParser::DontShow:
						break;
				}
			}
			else
			{
				emit echoPrivmsg(recipient, content);
			}
		}
	}
	else if (d->message == "MSG")
	{
		// This is an alias to the PRIVMSG command but it is so popular
		// that I decided to implement this permanently.
		parse(d->adapter, QString("/PRIVMSG %1").arg(d->tokens.join(" ")));
	}
	else if (d->message == "QUERY")
	{
		if (!d->tokens.isEmpty())
		{
			QString recipient = d->tokens.takeFirst();
			if (!IRCGlobal::isChannelName(recipient))
			{
				IRCUserInfo userInfo(recipient, d->adapter->network());
				emit query(userInfo.cleanNickname());
			}
		}
	}
	else if (d->message == "ME")
	{
		QString recipient = d->adapter->recipient();
		if (recipient.isEmpty())
		{
			return ErrorChatWindowOnly;
		}
		QString content = d->tokens.join(" ");
		parse(d->adapter, QString("/PRIVMSG %1 %2ACTION %3%2").arg(recipient, QChar(0x1), content));
	}
	else
	{
		d->output = QString("%1 %2").arg(d->message, d->tokens.join(" "));
	}

	return Ok;
}

//------------------------------------------------------------------------------
// ircnetworkadapter.cpp
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
#include "ircnetworkadapter.h"
#include "irc/ircchanneladapter.h"
#include "irc/ircglobal.h"
#include "irc/ircglobalmessages.h"
#include "irc/ircmessageclass.h"
#include "irc/ircprivadapter.h"
#include "irc/ircrequestparser.h"
#include "irc/ircuserinfo.h"
#include "irc/ircuserlist.h"
#include "log.h"

IRCNetworkAdapter::IRCNetworkAdapter()
{
	pIrcSocketSignalsAdapter = new IRCSocketSignalsAdapter(this);
	ircClient.connectSocketSignals(pIrcSocketSignalsAdapter);
	
	QObject::connect(&ircClient, SIGNAL( ircServerResponse(const QString&) ), 
		this, SLOT( ircServerResponse(const QString&) ) );
		
	// Request parser
	QObject::connect(&ircRequestParser, SIGNAL( echoPrivmsg(const QString&, const QString&) ), 
		this, SLOT( echoPrivmsg(const QString&, const QString&) ) );
		
	QObject::connect(&ircRequestParser, SIGNAL( query(const QString&) ), 
		this, SLOT( openNewAdapter(const QString&) ) );

	// Response parser begins here.
	QObject::connect(&ircResponseParser, SIGNAL( kick(const QString&, const QString&, const QString&, const QString&) ), 
		this, SLOT( kick(const QString&, const QString&, const QString&, const QString&) ) );
		
	QObject::connect(&ircResponseParser, SIGNAL( kill(const QString&, const QString&) ), 
		this, SLOT( kill(const QString&, const QString&) ) );

	QObject::connect(&ircResponseParser, SIGNAL( modeInfo(const QString&, const QString&, const QString&) ), 
		this, SLOT( modeInfo(const QString&, const QString&, const QString&) ) );

	QObject::connect(&ircResponseParser, SIGNAL( namesListReceived(const QString&, const QStringList&) ), 
		this, SLOT( namesListReceived(const QString&, const QStringList&) ) );

	QObject::connect(&ircResponseParser, SIGNAL( namesListEndReceived(const QString&) ), 
		this, SLOT( namesListEndReceived(const QString&) ) );
		
	QObject::connect(&ircResponseParser, SIGNAL( noSuchNickname(const QString&) ), 
		this, SLOT( noSuchNickname(const QString&) ) );

	QObject::connect(&ircResponseParser, SIGNAL ( parseError(const QString&) ), 
		this, SLOT( parseError(const QString&) ) );

	QObject::connect(&ircResponseParser, SIGNAL ( privMsgReceived(const QString&, const QString&, const QString&) ), 
		this, SLOT( privMsgReceived(const QString&, const QString&, const QString&) ) );

	QObject::connect(&ircResponseParser, SIGNAL ( sendPongMessage(const QString&) ), 
		this, SLOT( sendPong(const QString&) ) );

	QObject::connect(&ircResponseParser, SIGNAL ( userChangesNickname(const QString&, const QString&) ), 
		this, SLOT( userChangesNickname(const QString&, const QString&) ) );

	QObject::connect(&ircResponseParser, SIGNAL ( userJoinsChannel(const QString&, const QString&, const QString&) ), 
		this, SLOT( userJoinsChannel(const QString&, const QString&, const QString&) ) );
		
	QObject::connect(&ircResponseParser, SIGNAL ( userModeChanged(const QString&, const QString&, unsigned, unsigned) ), 
		this, SLOT( userModeChanged(const QString&, const QString&, unsigned, unsigned) ) );		

	QObject::connect(&ircResponseParser, SIGNAL ( userPartsChannel(const QString&, const QString&, const QString&) ), 
		this, SLOT( userPartsChannel(const QString&, const QString&, const QString&) ) );

	QObject::connect(&ircResponseParser, SIGNAL ( userQuitsNetwork(const QString&, const QString&) ), 
		this, SLOT( userQuitsNetwork(const QString&, const QString&) ) );
		
	QObject::connect(&ircResponseParser, SIGNAL ( whoIsUser(const QString&, const QString&, const QString&, const QString&) ), 
		this, SLOT( whoIsUser(const QString&, const QString&, const QString&, const QString&) ) );
}

IRCNetworkAdapter::~IRCNetworkAdapter()
{
	disconnect();

	killAllChatWindows();
	delete this->pIrcSocketSignalsAdapter;
}

void IRCNetworkAdapter::banUser(const QString& nickname, const QString& reason, const QString& channel)
{
	QString cleanNickname = IRCUserInfo(nickname).cleanNickname();

	IRCDelayedOperation operation(IRCDelayedOperation::Ban, cleanNickname, channel);
	operation.setAttribute("reason", reason);
	delayedOperations << operation;
	
	this->sendMessage(QString("/whois %1").arg(cleanNickname));
}

void IRCNetworkAdapter::connect(const IRCNetworkConnectionInfo& connectionInfo)
{
	this->connectionInfo = connectionInfo;
	emit titleChange();
	ircClient.connect(connectionInfo.networkEntity.address, connectionInfo.networkEntity.port);
}

void IRCNetworkAdapter::detachChatWindow(const IRCChatAdapter* pAdapter)
{
	chatWindows.remove(pAdapter->recipient().toLower());
}

void IRCNetworkAdapter::disconnect(const QString& farewellMessage)
{
	sendMessage("/quit " + farewellMessage);
	ircClient.disconnect();	
}

void IRCNetworkAdapter::doSendMessage(const QString& message, IRCAdapterBase* pOrigin)
{
	if (pOrigin == NULL)
	{
		pOrigin = this;
	}
	
	if (!ircClient.isConnected())
	{
		pOrigin->emitError(tr("You are not connected to the network."));
		return;
	}

	QString parsedMessage;
	IRCRequestParser::IRCRequestParseResult result = ircRequestParser.parse(pOrigin, message, parsedMessage);
	
	switch (result)
	{
		case IRCRequestParser::ErrorInputInsufficientParameters:
			pOrigin->emitError(tr("Insufficient parameters."));
			break;
		
		case IRCRequestParser::ErrorInputNotPrependedWithSlash:
			emit error(tr("This is a server window. All commands must be prepended with a '/' character."));
			break;
			
		case IRCRequestParser::ErrorMessageEmpty:
			pOrigin->emitError(tr("Attempted to send empty message"));
			break;
		
		case IRCRequestParser::ErrorMessageTooLong:
			pOrigin->emitError(tr("Command is too long"));
			break;
			
		case IRCRequestParser::Ok:
			ircClient.sendMessage(parsedMessage);
			break;
			
		case IRCRequestParser::QuitCommand:
			ircClient.sendMessage(parsedMessage);
			emit messageWithClass(tr("Quit"), IRCMessageClass::NetworkAction);
			break;
	}
}

void IRCNetworkAdapter::echoPrivmsg(const QString& recipient, const QString& content)
{
	// We will echo only chat messages for recipients for whom
	// we have windows open.
	if (hasRecipient(recipient))
	{
		const QString& sender = this->myNickname();	
		privMsgReceived(recipient, sender, content);
	}
}

IRCChatAdapter* IRCNetworkAdapter::getChatAdapter(const QString& recipient)
{
	IRCChatAdapter* pAdapter = NULL;
	
	if (recipient.isEmpty())
	{
		emit error("Doomseeker error: getChatAdapter() received empty recipient.");
		return NULL;
	}
	
	QString recipientLowercase = recipient.toLower();
	if (hasRecipient(recipientLowercase))
	{
		return chatWindows[recipientLowercase];
	}
	
	return NULL;
}

const IRCChatAdapter* IRCNetworkAdapter::getChatAdapter(const QString& recipient) const
{
	const IRCChatAdapter* pAdapter = NULL;
	
	if (recipient.isEmpty())
	{
		return NULL;
	}
	
	QString recipientLowercase = recipient.toLower();
	if (hasRecipient(recipientLowercase))
	{
		return chatWindows[recipientLowercase];
	}
	
	return NULL;
}

IRCChatAdapter* IRCNetworkAdapter::getOrCreateNewChatAdapter(const QString& recipient)
{
	IRCChatAdapter* pAdapter = NULL;

	if (recipient.isEmpty())
	{
		emit error("Doomseeker error: getOrCreateNewChatAdapter() received empty recipient.");
		return NULL;
	}

	QString recipientLowercase = recipient.toLower();
	
	if (hasRecipient(recipientLowercase))
	{
		return chatWindows[recipientLowercase];
	}

#ifdef _DEBUG
	Log::instance << QString("IRCNetworkAdapter::getOrCreateNewChatAdapter() Creating new adapter for recipient: %1").arg(recipientLowercase);
#endif
	
	if (IRCGlobal::isChannelName(recipient))
	{
		pAdapter = new IRCChannelAdapter(this, recipient);
	}
	else
	{
		pAdapter = new IRCPrivAdapter(this, recipient);
	}

	chatWindows.insert(recipientLowercase, pAdapter);
	emit newChatWindowIsOpened(pAdapter);

	return pAdapter;
}

bool IRCNetworkAdapter::hasRecipient(const QString& recipient) const
{
	QString recipientLowercase = recipient.toLower();

	return (chatWindows.find(recipientLowercase) != chatWindows.end());
}

void IRCNetworkAdapter::ircServerResponse(const QString& message)
{
	ircResponseParser.parse(message);

	emit this->message(message.trimmed());
}

bool IRCNetworkAdapter::isMyNickname(const QString& nickname) const
{
	IRCUserInfo myUserInfo(this->connectionInfo.nick);

	return (myUserInfo == nickname);
}

bool IRCNetworkAdapter::isOperator(const QString& nickname, const QString& channel) const
{
	if (IRCGlobal::isChannelName(channel))
	{
		const IRCChannelAdapter* pChannelAdapter = (const IRCChannelAdapter*) this->getChatAdapter(channel);
		if (pChannelAdapter != NULL)
		{
			return pChannelAdapter->isOperator(nickname);
		}
	}
	
	return false;
}

void IRCNetworkAdapter::kick(const QString& channel, const QString& byWhom, const QString& whoIsKicked, const QString& reason)
{
	if (hasRecipient(channel))
	{
		IRCChannelAdapter* pAdapter = (IRCChannelAdapter*) this->getOrCreateNewChatAdapter(channel);
		
		if (isMyNickname(whoIsKicked))
		{
			this->emitMessageWithClass(tr("You have been kicked from channel %1 by %2 (Reason: %3)").arg(channel, byWhom, reason), IRCMessageClass::ChannelAction);
			killChatWindow(channel);
		}
		else
		{
			pAdapter->emitMessageWithClass(tr("%1 was kicked by %2 (%3)").arg(whoIsKicked, byWhom, reason), IRCMessageClass::ChannelAction);
			pAdapter->removeNameFromCachedList(whoIsKicked);
		}
	}
}

void IRCNetworkAdapter::kill(const QString& victim, const QString& comment)
{
	emit message(QString("Connection for user %1 was killed. (%2)").arg(victim, comment));

	// We need to iterate through EVERY adapter and notify them
	// about this quit.
	// Implementation of each adapter should recognize if this quit actually
	// has anything to do with that adapter.
	QList<IRCChatAdapter*> adaptersList = chatWindows.values();
	foreach (IRCChatAdapter* pAdapter, adaptersList)
	{
		pAdapter->userLeaves(victim, comment, IRCChatAdapter::NetworkKill);
	}
}

void IRCNetworkAdapter::killAllChatWindows()
{	
	QList<IRCChatAdapter*> pWindows = chatWindows.values();
	foreach (IRCChatAdapter* pAdapter, pWindows)
	{
		// Make sure that the adapter destructor won't call the
		// detachChatWindow() method or the program will be shot to oblivion.
		pAdapter->setNetwork(NULL);
		delete pAdapter;
	}

	chatWindows.clear();
}

void IRCNetworkAdapter::killChatWindow(const QString& recipient)
{
	if (hasRecipient(recipient))
	{
		IRCChatAdapter* pAdapter = getChatAdapter(recipient);
		chatWindows.remove(recipient.toLower());
		
		// Make sure that the adapter destructor won't call the
		// detachChatWindow() method or the program will be shot to oblivion.
		pAdapter->setNetwork(NULL);
		delete pAdapter;
	}
}

void IRCNetworkAdapter::modeInfo(const QString& channel, const QString& whoSetThis, const QString& modeParams)
{
	IRCChannelAdapter* pAdapter = (IRCChannelAdapter*) this->getOrCreateNewChatAdapter(channel);
	pAdapter->emitMessageWithClass(tr("%1 sets mode: %2").arg(whoSetThis, modeParams), IRCMessageClass::ChannelAction);
}

void IRCNetworkAdapter::namesListReceived(const QString& channel, const QStringList& names)
{
	if (this->hasRecipient(channel))
	{
		IRCChannelAdapter* pAdapter = (IRCChannelAdapter*) this->getOrCreateNewChatAdapter(channel);
		pAdapter->appendNamesToCachedList(names);
	}
}

void IRCNetworkAdapter::namesListEndReceived(const QString& channel)
{
	if (this->hasRecipient(channel))
	{
		IRCChannelAdapter* pAdapter = (IRCChannelAdapter*) this->getOrCreateNewChatAdapter(channel);
		pAdapter->emitCachedNameListUpdated();
	}
}

void IRCNetworkAdapter::noSuchNickname(const QString& nickname)
{
	IRCGlobalMessages::instance().emitError(tr("User %1 is not logged in.").arg(nickname), this);
}

void IRCNetworkAdapter::openNewAdapter(const QString& recipientName)
{
	if (!isConnected() || recipientName.isEmpty())
	{
		return;
	}
	
	bool bStandardRoutine = !IRCGlobal::isChannelName(recipientName)
		|| hasRecipient(recipientName);

	if (bStandardRoutine)
	{
		IRCChatAdapter* pAdapter = this->getOrCreateNewChatAdapter(recipientName);
		pAdapter->emitFocusRequest();
	}
	else if (IRCGlobal::isChannelName(recipientName))
	{
		this->sendMessage("/join " + recipientName);
	}
}

void IRCNetworkAdapter::parseError(const QString& error)
{
	emit this->error(tr("IRC Parse error: %1").arg(error));
}

void IRCNetworkAdapter::privMsgReceived(const QString& recipient, const QString& sender, const QString& content)
{
	IRCChatAdapter* pAdapter = this->getOrCreateNewChatAdapter(recipient);
	pAdapter->emitChatMessage(sender, content);
}

void IRCNetworkAdapter::sendPong(const QString& toWhom)
{
	QString message = QString("/PONG %1").arg(toWhom);
	sendMessage(message);
}

void IRCNetworkAdapter::setChannelMode(const QString& channel, const QString& nickname, const QString& flag, bool bSet)
{
	QString cleanNickname = IRCUserInfo(nickname).cleanNickname();
	
	QString flagPrefixed;
	if (bSet)
	{
		flagPrefixed = "+" + flag.trimmed();
	}
	else
	{
		flagPrefixed = "-" + flag.trimmed();
	}
	
	QString message = QString("/mode %1 %2 %3").arg(channel, flagPrefixed, cleanNickname);
	this->sendMessage(message);
}

QString IRCNetworkAdapter::title() const
{
	return connectionInfo.networkEntity.description + " ( " + myNickname() + " )";
}

void IRCNetworkAdapter::userChangesNickname(const QString& oldNickname, const QString& newNickname)
{
	emit message(QString("User changes nickname: %1 to %2").arg(oldNickname, newNickname));

	if (isMyNickname(oldNickname))
	{
		emit message("Updating own nickname");
		connectionInfo.nick = newNickname;
		
		emit titleChange();
	}
	
	QList<IRCChatAdapter*> adaptersList = chatWindows.values();
	foreach (IRCChatAdapter* pAdapter, adaptersList)
	{
		pAdapter->userChangesNickname(oldNickname, newNickname);
	}
	
	// MAKE SURE TO SEE IF WE HAVE A CHAT WINDOW OPEN WITH THIS 
	// USER AND FIX THE KEY IN THE MAP!!!
	if (hasRecipient(oldNickname))
	{
		QString oldNicknameLowercase = oldNickname.toLower();
		QString newNicknameLowercase = newNickname.toLower();

		IRCChatAdapter* pAdapter = getChatAdapter(oldNickname);
		chatWindows.remove(oldNicknameLowercase);
		chatWindows.insert(newNicknameLowercase, pAdapter);
	}
}

void IRCNetworkAdapter::userJoinsChannel(const QString& channel, const QString& nickname, const QString& fullSignature)
{
	if (!isMyNickname(nickname))
	{
		if (hasRecipient(channel))
		{
			emit messageWithClass(tr("User %1 joins channel %2").arg(nickname, channel), IRCMessageClass::ChannelAction);

			IRCChannelAdapter* pChannel = (IRCChannelAdapter*)this->getOrCreateNewChatAdapter(channel);
			pChannel->userJoins(nickname, fullSignature);
		}
	}
	else
	{
		IRCChannelAdapter* pChannel = (IRCChannelAdapter*)this->getOrCreateNewChatAdapter(channel);
		pChannel->emitFocusRequest();
	}
}

void IRCNetworkAdapter::userModeChanged(const QString& channel, const QString& nickname, unsigned flagsAdded, unsigned flagsRemoved)
{
	if (hasRecipient(channel))
	{
		IRCChatAdapter* pAdapter = this->getOrCreateNewChatAdapter(channel);
		pAdapter->userModeChanges(nickname, flagsAdded, flagsRemoved);
	}
}

void IRCNetworkAdapter::userPartsChannel(const QString& channel, const QString& nickname, const QString& farewellMessage)
{
	if (hasRecipient(channel))
	{
		IRCChannelAdapter* pChannel = (IRCChannelAdapter*)getChatAdapter(channel);
			
		if (isMyNickname(nickname))
		{
			emit messageWithClass(tr("You left channel %1").arg(channel), IRCMessageClass::ChannelAction);
			killChatWindow(channel);
		}
		else
		{
			emit messageWithClass(tr("User %1 parts channel %2").arg(nickname, channel), IRCMessageClass::ChannelAction);
			pChannel->userLeaves(nickname, farewellMessage, IRCChatAdapter::ChannelPart);
		}
	}
	
	
}

void IRCNetworkAdapter::userQuitsNetwork(const QString& nickname, const QString& farewellMessage)
{
	emit messageWithClass(QString("User %1 quits network.").arg(nickname), IRCMessageClass::ChannelAction);
	
	// We need to iterate through EVERY adapter and notify them
	// about this quit.
	// Implementation of each adapter should recognize if this quit actually
	// has anything to do with that adapter.
	QList<IRCChatAdapter*> adaptersList = chatWindows.values();
	foreach (IRCChatAdapter* pAdapter, adaptersList)
	{
		pAdapter->userLeaves(nickname, farewellMessage, IRCChatAdapter::NetworkQuit);
	}
}

void IRCNetworkAdapter::whoIsUser(const QString& nickname, const QString& user, const QString& hostName, const QString& realName)
{
	// Deliver pending bans.
	while(true)
	{
		const IRCDelayedOperation* pBanOperation = delayedOperations.operationForNickname(IRCDelayedOperation::Ban, nickname);
		if (pBanOperation == NULL)
		{
			break;
		}
		
		QString banString = "*!*@" + hostName;
		QString reason = pBanOperation->attribute("reason");
		this->sendMessage(QString("/mode %1 +b %2").arg(pBanOperation->channel(), banString));
		this->sendMessage(QString("/kick %1 %2 %3").arg(pBanOperation->channel(), nickname, reason));
		
		delayedOperations.remove(pBanOperation);
	}
}

////////////////////////////////////////////////////////////////////////////////

void IRCSocketSignalsAdapter::connected()
{
	emit pParent->message(tr("Connected. Sending registration messages."));
	
	IRCNetworkConnectionInfo& connectionInfo = pParent->connectionInfo;
	
	QString messagePass = "/PASS %1";
	QString messageNick = "/NICK %1 %2";
	QString messageUser = "/USER %1 %2 %3 :%4";
	
	IRCNetworkEntity& network = connectionInfo.networkEntity;
	
	if (!network.password.isEmpty())
	{
		pParent->sendMessage(messagePass.arg(network.password));
	}
	
	pParent->sendMessage(messageNick.arg(connectionInfo.nick).arg(connectionInfo.alternateNick));
	pParent->sendMessage(messageUser.arg(connectionInfo.nick).arg(connectionInfo.nick).arg(connectionInfo.nick).arg(connectionInfo.realName));
	pParent->sendMessage(messageUser.arg(connectionInfo.nick));
	
	if (!network.nickservPassword.isEmpty())
	{
		QString messageNickserv = network.nickservCommand;
		messageNickserv = messageNickserv.arg(network.nickservPassword);
		
		pParent->sendMessage(messageNickserv);
	}
}

void IRCSocketSignalsAdapter::disconnected()
{
	pParent->killAllChatWindows();
	gLog << tr("IRC: Disconnected from network %1").arg(pParent->connectionInfo.networkEntity.description);
	emit pParent->message("Disconnected");
}

void IRCSocketSignalsAdapter::errorReceived(QAbstractSocket::SocketError error)
{
	emit pParent->error(pSocket->errorString());
}

void IRCSocketSignalsAdapter::infoMessage(const QString& message)
{
	gLog << message;
	emit pParent->message(message);
}

void IRCSocketSignalsAdapter::hostLookupError(QHostInfo::HostInfoError errorValue)
{
	switch (errorValue)
	{
		case QHostInfo::HostNotFound:
			emit pParent->error("Host lookup error: host not found.");
			break;
			
		default:
			emit pParent->error("Unknown host lookup error.");
			break;
	}
}

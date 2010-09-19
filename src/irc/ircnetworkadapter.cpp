//------------------------------------------------------------------------------
// ircnetworkadapter.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ircnetworkadapter.h"
#include "irc/ircchatadapter.h"

IRCNetworkAdapter::IRCNetworkAdapter()
{
	pIrcSocketSignalsAdapter = new IRCSocketSignalsAdapter(this);
	ircClient.connectSocketSignals(pIrcSocketSignalsAdapter);
	
	QObject::connect(&ircClient, SIGNAL( ircServerResponse(const QString&) ), this, SLOT( ircServerResponse(const QString&) ) );
	
	QObject::connect(&ircResponseParser, SIGNAL ( privMsgReceived(const QString&, const QString&, const QString&) ), 
		this, SLOT( privMsgReceived(const QString&, const QString&, const QString&) ) );
	QObject::connect(&ircResponseParser, SIGNAL ( sendPongMessage(const QString&) ), this, SLOT( sendPong(const QString&) ) );
	QObject::connect(&ircResponseParser, SIGNAL ( userChangesNickname(const QString&, const QString&) ), 
		this, SLOT( userChangesNickname(const QString&, const QString&) ) );
	QObject::connect(&ircResponseParser, SIGNAL ( userJoinsChannel(const QString&, const QString&, const QString&) ), 
		this, SLOT( userJoinsChannel(const QString&, const QString&, const QString&) ) );
}

IRCNetworkAdapter::~IRCNetworkAdapter()
{
	killAllChatWindows();
	delete pIrcSocketSignalsAdapter;
}

void IRCNetworkAdapter::connect(const IRCNetworkConnectionInfo& connectionInfo)
{
	this->connectionInfo = connectionInfo;

	emit titleChange();

	QHostAddress address(connectionInfo.serverAddress);
	ircClient.connect(address, connectionInfo.serverPort);
}

IRCChatAdapter* IRCNetworkAdapter::createNewChatAdapter(const QString& recipient)
{
	IRCChatAdapter* pAdapter = NULL;
	
	if (hasRecipient(recipient))
	{
		return chatWindows[recipient];
	}
	
	pAdapter = new IRCChatAdapter(this, recipient);
	chatWindows.insert(recipient, pAdapter);
	emit newChatWindowIsOpened(pAdapter);

	return pAdapter;
}

void IRCNetworkAdapter::detachChatWindow(const IRCChatAdapter* pAdapter)
{
	chatWindows.remove(pAdapter->recipient());
}

void IRCNetworkAdapter::doSendMessage(const QString& message, IRCAdapterBase* pOrigin)
{
	if (pOrigin == NULL)
	{
		pOrigin = this;
	}

	// First of all trim whitespaces.
	QString formattedMessage = message.trimmed();
	if (!formattedMessage.startsWith('/'))
	{
		emit error(tr("This is a server window. All commands must be prepended with a '/' character."));
	}
	else
	{
		formattedMessage = formattedMessage.remove(0, 1);
		
		if (formattedMessage.size() > IRCClient::MAX_MESSAGE_LENGTH)
		{
			pOrigin->emitError(tr("Message is too long."));
		}
		else
		{
			ircClient.sendMessage(formattedMessage);
		}
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

bool IRCNetworkAdapter::hasRecipient(const QString& recipient) const
{
	return (chatWindows.find(recipient) != chatWindows.end());
}

void IRCNetworkAdapter::privMsgReceived(const QString& recipient, const QString& sender, const QString& content)
{
	IRCChatAdapter* pAdapter = NULL;

	if (hasRecipient(recipient))
	{
		pAdapter = chatWindows[recipient];
	}
	else
	{
		pAdapter = this->createNewChatAdapter(recipient);
	}

	pAdapter->emitMessage(QString("<%1>: %2").arg(sender, content));
}

void IRCNetworkAdapter::ircServerResponse(const QString& message)
{
	ircResponseParser.parse(message);

	emit this->message(message.trimmed());
}

bool IRCNetworkAdapter::isMyNickname(const QString& nickname) const
{
	return (connectionInfo.nick.compare(nickname) == 0);
}

void IRCNetworkAdapter::sendPong(const QString& toWhom)
{
	QString message = QString("/PONG %1").arg(toWhom);
	sendMessage(message);
}

QString IRCNetworkAdapter::title() const
{
	return connectionInfo.serverAddress;
}

void IRCNetworkAdapter::userChangesNickname(const QString& oldNickname, const QString& newNickname)
{
	emit message(QString("User changes nickname: %1 to %2").arg(oldNickname, newNickname));

	if (isMyNickname(oldNickname))
	{
		emit message("Updating own nickname");
		connectionInfo.nick = newNickname;
	}
	
	// TODO Update user roster.
}

void IRCNetworkAdapter::userJoinsChannel(const QString& channel, const QString& nickname, const QString& fullSignature)
{
	if (isMyNickname(nickname))
	{
		// We should now open a new channel window.
		IRCChatAdapter* pNewChatWindow = createNewChatAdapter(channel);
	}

	// TODO Update user roster.
}

////////////////////////////////////////////////////////////////////////////////

void IRCSocketSignalsAdapter::connected()
{
	emit pParent->message(tr("Connected. Sending registration messages."));
	
	IRCNetworkConnectionInfo& connectionInfo = pParent->connectionInfo;
	
	QString messagePass = "/PASS %1";
	QString messageNick = "/NICK %1 %2";
	QString messageUser = "/USER %1 %2 %3 :%4";
	
	if (!connectionInfo.serverPassword.isEmpty())
	{
		pParent->sendMessage(messagePass.arg(connectionInfo.serverPassword));
	}
	
	pParent->sendMessage(messageNick.arg(connectionInfo.nick).arg(connectionInfo.alternateNick));
	pParent->sendMessage(messageUser.arg(connectionInfo.nick).arg(connectionInfo.nick).arg(connectionInfo.nick).arg(connectionInfo.realName));
}

void IRCSocketSignalsAdapter::disconnected()
{
	pParent->killAllChatWindows();
	emit pParent->message("Disconnected");
}

void IRCSocketSignalsAdapter::errorReceived(QAbstractSocket::SocketError error)
{
	emit pParent->error(pSocket->errorString());
}

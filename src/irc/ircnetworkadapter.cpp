//------------------------------------------------------------------------------
// ircnetworkadapter.cpp
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "ircnetworkadapter.h"

IRCNetworkAdapter::IRCNetworkAdapter()
{
	pIrcSocketSignalsAdapter = new IRCSocketSignalsAdapter(this);
	ircClient.connectSocketSignals(pIrcSocketSignalsAdapter);
	
	QObject::connect(&ircClient, SIGNAL( ircServerResponse(const QString&) ), this, SLOT( ircServerResponse(const QString&) ) );
	
	QObject::connect(&ircResponseParser, SIGNAL ( sendPongMessage(const QString&) ), this, SLOT( sendPong(const QString&) ) );
}

IRCNetworkAdapter::~IRCNetworkAdapter()
{
	delete pIrcSocketSignalsAdapter;
}

void IRCNetworkAdapter::connect(const IRCNetworkConnectionInfo& connectionInfo)
{
	this->connectionInfo = connectionInfo;

	QHostAddress address(connectionInfo.serverAddress);
	ircClient.connect(address, connectionInfo.serverPort);
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
			pOrigin->emitError(tr("Message is too large."));
		}
		else
		{
			ircClient.sendMessage(formattedMessage);
		}
	}
}

void IRCNetworkAdapter::ircServerResponse(const QString& message)
{
	ircResponseParser.parse(message);

	emit this->message(message.trimmed());
}

void IRCNetworkAdapter::sendPong(const QString& toWhom)
{
	QString message = QString("/PONG %1").arg(toWhom);
	sendMessage(message);
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
	emit pParent->message("Disconnected");
}

void IRCSocketSignalsAdapter::errorReceived(QAbstractSocket::SocketError error)
{
	emit pParent->error(pSocket->errorString());
}

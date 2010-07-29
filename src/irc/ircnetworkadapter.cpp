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

void IRCNetworkAdapter::ircServerResponse(const QString& message)
{
	emit this->message(message.trimmed());
}

void IRCNetworkAdapter::sendMessage(const QString& message)
{
	// First of all trim whitespaces.
	QString formattedMessage = message.trimmed();
	if (!formattedMessage.startsWith('/'))
	{
		emit error(tr("This is a server window. All commands must be prepended with a '/' character."));
	}
	else
	{
		formattedMessage = formattedMessage.remove(0, 1);
		ircClient.sendMessage(formattedMessage);
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

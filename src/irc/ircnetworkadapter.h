//------------------------------------------------------------------------------
// ircnetworkadapter.h
//
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __IRCNETWORKADAPTER_H__
#define __IRCNETWORKADAPTER_H__

#include "irc/ircadapterbase.h"
#include "irc/ircclient.h"
#include "irc/ircnetworkconnectioninfo.h"
#include "socketsignalsadapter.h"

class IRCSocketSignalsAdapter;

class IRCNetworkAdapter : public IRCAdapterBase
{
	Q_OBJECT
	
	friend class IRCSocketSignalsAdapter;

	public:
		IRCNetworkAdapter();
		~IRCNetworkAdapter();
	
		void						connect(const IRCNetworkConnectionInfo& connectionInfo);
		
	public slots:
		void						sendMessage(const QString& message);
		
	protected:
		IRCClient					ircClient;
		IRCSocketSignalsAdapter*	pIrcSocketSignalsAdapter;
};

class IRCSocketSignalsAdapter : public SocketSignalsAdapter
{
	Q_OBJECT
	
	public:
		IRCNetworkAdapter*		pParent;
	
		IRCSocketSignalsAdapter(IRCNetworkAdapter* pParent)
		{
			this->pParent = pParent;
		}

	public slots:
		void					connected();
		void					disconnected();
		void					errorReceived(QAbstractSocket::SocketError error);
};

#endif

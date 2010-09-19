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
#include "irc/ircresponseparser.h"
#include "socketsignalsadapter.h"

#include <QHash>

class IRCChatAdapter;
class IRCSocketSignalsAdapter;

/**
 *	@brief Interprets communication between the client and the IRC server.
 */
class IRCNetworkAdapter : public IRCAdapterBase
{
	Q_OBJECT
	
	friend class IRCSocketSignalsAdapter;

	public:
		IRCNetworkAdapter();
		~IRCNetworkAdapter();

		AdapterType							adapterType() const { return NetworkAdapter; }
	
		void								connect(const IRCNetworkConnectionInfo& connectionInfo);

		/**
		 *	@brief Detaches the IRCChatAdapter instance from this network
		 *  @b without deleting the instance.
		 */
		void								detachChatWindow(const IRCChatAdapter* pAdapter);
		
		/**
		 *	@brief Implemented to support direct communication between client 
		 *	and server.
		 *
		 *	All messages that do not begin with '/' character will be ignored 
		 *	here. In fact if the '/' character is missing an error() signal
		 *	will be emitted to notify the user of this fact.
		 *
		 *	Programmers must remember that although IRC protocol itself
		 *	doesn't require clients to prepend the messages with a slash this
		 *	class always does - the slash character is stripped, then the 
		 *	remainder of the message is sent 'as is'.
		 *
		 *	@param pOrigin
		 *		If this is not null the IRCNetworkAdapter will attempt to pass
		 *		some message and error signals through this pOrigin. Otherwise
		 *		these signals will be sent directly.
		 */
		void								doSendMessage(const QString& message, IRCAdapterBase* pOrigin);				

		bool								hasRecipient(const QString& recipient) const;
		bool								isMyNickname(const QString& nickname) const;

		void								killAllChatWindows();

		QString								title() const;

	signals:
		void								newChatWindowIsOpened(IRCChatAdapter* pWindow);
		
	protected:
		QHash<QString, IRCChatAdapter*>		chatWindows;
		IRCNetworkConnectionInfo			connectionInfo;
		IRCClient							ircClient;
		IRCResponseParser					ircResponseParser;
		IRCSocketSignalsAdapter*			pIrcSocketSignalsAdapter;

		/**
		 *	@brief Creates the new IRCChatAdapter object and immedaitelly 
		 *	adds it to the chatWindows hashmap.
		 *
		 *	@b Note: If such recipient is already registered no new object 
		 *	is created.
		 *
		 *	@return Pointer to the adapter object.
		 */
		IRCChatAdapter*						createNewChatAdapter(const QString& recipient);

	protected slots:
		void								ircServerResponse(const QString& message);
		void								privMsgReceived(const QString& recipient, const QString& sender, const QString& content);
		void								sendPong(const QString& toWhom);
		void								userChangesNickname(const QString& oldNickname, const QString& newNickname);
		void								userJoinsChannel(const QString& channel, const QString& nickname, const QString& fullSignature);

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

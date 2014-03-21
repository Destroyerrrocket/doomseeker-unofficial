//------------------------------------------------------------------------------
// masterclientsignalproxy.h
//
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __MASTERCLIENTSIGNALPROXY_H__
#define __MASTERCLIENTSIGNALPROXY_H__

#include <QList>
#include <QObject>

class MasterClient;
class Message;
class Server;

/**
 *	@brief Designed to preserve the information about the MasterClient
 *	instance that is sending the signal.
 */
class MasterClientSignalProxy : public QObject
{
	Q_OBJECT

	public:
		MasterClient* pMaster;

		MasterClientSignalProxy(MasterClient* pMaster);

	signals:
		void listUpdated(MasterClient* pSender);
		void message(MasterClient* pSender, const QString& title, const QString& content, bool isError);
		void messageImportant(MasterClient* pSender, const Message& objMessage);

	protected slots:
		void listUpdatedSlot()
		{
			emit listUpdated(pMaster);
		}

		void readMasterMessage(const QString& title, const QString& content, bool isError)
		{
			emit message(pMaster, title, content, isError);
		}

		void readMasterMessageImportant(const Message& objMessage)
		{
			emit messageImportant(pMaster, objMessage);
		}
};

#endif

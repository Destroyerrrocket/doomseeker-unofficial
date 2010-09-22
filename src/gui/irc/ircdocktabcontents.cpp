//------------------------------------------------------------------------------
// ircdocktabcontents.cpp
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
#include "ircdocktabcontents.h"
#include "gui/commonGUI.h"
#include "irc/ircchanneladapter.h"
#include "irc/ircdock.h"
#include "irc/ircglobal.h"
#include "irc/ircnetworkadapter.h"
#include "irc/ircuserinfo.h"
#include "irc/ircuserlist.h"
#include "log.h"
#include <QStandardItemModel>

IRCDockTabContents::IRCDockTabContents(IRCDock* pParentIRCDock)
{
	setupUi(this);
	
	this->bIsDestroying = false;

	this->pParentIRCDock = pParentIRCDock;
	this->lvUserList->setModel(new QStandardItemModel(this->lvUserList));

	// There is only one case in which we want this to be visible:
	// if we are in a channel.
	this->lvUserList->setVisible(false);

	connect(btnSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ));
	connect(leCommandLine, SIGNAL( returnPressed() ), this, SLOT( sendMessage() ));
}

IRCDockTabContents::~IRCDockTabContents()
{
	this->bIsDestroying = true;

	if (pIrcAdapter != NULL)
	{
		disconnect(pIrcAdapter, 0, 0, 0);
		IRCAdapterBase* pTmpAdapter = pIrcAdapter;
		pIrcAdapter = NULL;
		delete pTmpAdapter;	
	}
}

void IRCDockTabContents::adapterTerminating()
{
	if (pIrcAdapter != NULL && !this->bIsDestroying)
	{
		// Disconnect the adapter from this tab.
		disconnect(pIrcAdapter, 0, 0, 0);
		pIrcAdapter = NULL;
		
		emit chatWindowCloseRequest(this);
	}
}

QStandardItem* IRCDockTabContents::findUserListItem(const QString& nickname)
{
	QStandardItemModel* pModel = (QStandardItemModel*)this->lvUserList->model();
	IRCUserInfo userInfo(nickname);
	
	for (int i = 0; i < pModel->rowCount(); ++i)
	{
		QStandardItem* pItem = pModel->item(i);
		if (userInfo == pItem->text())
		{
			return pItem;
		}
	}
	
	return NULL;
}

void IRCDockTabContents::nameAdded(const IRCUserInfo& userInfo)
{
	QString nickname = userInfo.prefixedName();
	printf("IRCDockTabContents Name added! %s\n", nickname.toAscii().constData());

	QStandardItemModel* pModel = (QStandardItemModel*)this->lvUserList->model();
	QStandardItem* pItem = new QStandardItem(nickname);

	// Try to append the nickname at the proper place in the list.
	for (int i = 0; i < pModel->rowCount(); ++i)
	{
		QStandardItem* pExistingItem = pModel->item(i);
		QString existingNickname = pExistingItem->text();

		if (userInfo <= existingNickname)
		{
			pModel->insertRow(i, pItem);
			return;
		}
	}

	// If above code didn't return then
	// this nickname should be appended to the end of the list.
	pModel->appendRow(pItem);
}

void IRCDockTabContents::nameListUpdated(const IRCUserList& userList)
{
	for (unsigned i = 0; i < userList.size(); ++i)
	{
		nameAdded(*userList[i]);
	}
}

void IRCDockTabContents::nameRemoved(const IRCUserInfo& userInfo)
{
	QStandardItemModel* pModel = (QStandardItemModel*)this->lvUserList->model();
	for (int i = 0; i < pModel->rowCount(); ++i)
	{
		QStandardItem* pItem = pModel->item(i);
		if (userInfo == pItem->text())
		{
			printf("IRCDockTabContents Name removed! %s\n", userInfo.prefixedName().toAscii().constData());

			pModel->removeRow(i);
			break;
		}
	}
}

void IRCDockTabContents::nameUpdated(const IRCUserInfo& userInfo)
{
	nameRemoved(userInfo);
	nameAdded(userInfo);
}

void IRCDockTabContents::newChatWindowIsOpened(IRCChatAdapter* pAdapter)
{
	this->lvUserList->setModel(new QStandardItemModel(this));

	pParentIRCDock->addIRCAdapter(pAdapter);
}

void IRCDockTabContents::receiveError(const QString& error)
{
	receiveMessageColored(tr("Error: %1").arg(error), "#ff0000");
}

void IRCDockTabContents::receiveMessage(const QString& message)
{
	txtOutputWidget->moveCursor(QTextCursor::End);
	txtOutputWidget->insertPlainText(message + "\n");
}

void IRCDockTabContents::receiveMessageColored(const QString& message, const QString& htmlColor)
{
	txtOutputWidget->moveCursor(QTextCursor::End);
	txtOutputWidget->insertHtml("<font color='" + htmlColor + "'>" + message + "</font><br />");
}

void IRCDockTabContents::sendMessage()
{
	QString message = leCommandLine->text();
	leCommandLine->setText("");
	
	pIrcAdapter->sendMessage(message);
}

void IRCDockTabContents::setIRCAdapter(IRCAdapterBase* pAdapter)
{
	pIrcAdapter = pAdapter;
	connect(pIrcAdapter, SIGNAL( error(const QString&) ), SLOT( receiveError(const QString& ) ));
	connect(pIrcAdapter, SIGNAL( message(const QString&) ), SLOT( receiveMessage(const QString& ) ));
	connect(pIrcAdapter, SIGNAL( messageColored(const QString&, const QString&) ), SLOT( receiveMessageColored(const QString&, const QString&) ));
	connect(pIrcAdapter, SIGNAL( terminating() ), SLOT( adapterTerminating() ) );
	connect(pIrcAdapter, SIGNAL( titleChange() ), SLOT( adapterTitleChange() ) );

	switch (pIrcAdapter->adapterType())
	{
		case IRCAdapterBase::NetworkAdapter:
		{
			IRCNetworkAdapter* pNetworkAdapter = (IRCNetworkAdapter*)pAdapter;
			connect(pNetworkAdapter, SIGNAL( newChatWindowIsOpened(IRCChatAdapter*) ), SLOT( newChatWindowIsOpened(IRCChatAdapter*) ) );
			break;
		}
		
		case IRCAdapterBase::ChannelAdapter:
		{
			IRCChannelAdapter* pChannelAdapter = (IRCChannelAdapter*)pAdapter;
			connect(pChannelAdapter, SIGNAL( nameAdded(const IRCUserInfo&) ), SLOT( nameAdded(const IRCUserInfo&) ) );
			connect(pChannelAdapter, SIGNAL( nameListUpdated(const IRCUserList&) ), SLOT( nameListUpdated(const IRCUserList&) ) );
			connect(pChannelAdapter, SIGNAL( nameRemoved(const IRCUserInfo&) ), SLOT( nameRemoved(const IRCUserInfo&) ) );
			connect(pChannelAdapter, SIGNAL( nameUpdated(const IRCUserInfo&) ), SLOT( nameUpdated(const IRCUserInfo&) ) );

			this->lvUserList->setVisible(true);
			break;
		}

		case IRCAdapterBase::PrivAdapter:
		{
			break;
		}

		default:
		{
			receiveError("Doomseeker error: Unknown IRCAdapterBase*");
			break;
		}
	}
}

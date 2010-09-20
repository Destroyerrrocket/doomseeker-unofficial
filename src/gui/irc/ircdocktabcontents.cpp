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
#include "irc/ircnetworkadapter.h"
#include <QStandardItemModel>

IRCDockTabContents::IRCDockTabContents(IRCDock* pParentIRCDock)
: QWidget(pParentIRCDock)
{
	setupUi(this);

	this->pParentIRCDock = pParentIRCDock;
	this->lvUserList->setModel(new QStandardItemModel(this->lvUserList));

	// There is only one case in which we want this to be visible:
	// if we are in a channel.
	this->lvUserList->setVisible(false);

	connect(btnSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ));
	connect(leCommandLine, SIGNAL( returnPressed() ), this, SLOT( sendMessage() ));
}

void IRCDockTabContents::nameAdded(const QString& nickname)
{
	printf("IRCDockTabContents Name added! %s\n", nickname.toAscii().constData());

	QStandardItemModel* pModel = (QStandardItemModel*)this->lvUserList->model();
	QStandardItem* pItem = new QStandardItem(nickname);

	// Try to append the nickname at the proper place in the list.
	QString nicknameLower = nickname.toLower();
	for (int i = 0; i < pModel->rowCount(); ++i)
	{
		QStandardItem* pExistingItem = pModel->item(i);
		QString existingNicknameLower = pExistingItem->text().toLower();

		if (nicknameLower <= existingNicknameLower)
		{
			pModel->insertRow(i, pItem);
			return;
		}
	}

	// If above code didn't return
	// this nickname should be appended to the end of the list.
	pModel->appendRow(pItem);
}

void IRCDockTabContents::nameListUpdated(const QStringList& names)
{
	foreach(const QString& p, names)
	{
		printf("%s\n", p.toAscii().constData());
	}
	CommonGUI::stringListToStandardItemsListView(this->lvUserList, names);
}

void IRCDockTabContents::nameRemoved(const QString& nickname)
{
	QStandardItemModel* pModel = (QStandardItemModel*)this->lvUserList->model();
	for (int i = 0; i < pModel->rowCount(); ++i)
	{
		QStandardItem* pItem = pModel->item(i);
		if (pItem->text().compare(nickname, Qt::CaseInsensitive) == 0)
		{
			printf("IRCDockTabContents Name removed! %s\n", nickname.toAscii().constData());

			pModel->removeRow(i);
			break;
		}
	}
}

void IRCDockTabContents::newChatWindowIsOpened(IRCChatAdapter* pAdapter)
{
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
			connect(pChannelAdapter, SIGNAL( nameAdded(const QString&) ), SLOT( nameAdded(const QString&) ) );
			connect(pChannelAdapter, SIGNAL( nameListUpdated(const QStringList&) ), SLOT( nameListUpdated(const QStringList&) ) );
			connect(pChannelAdapter, SIGNAL( nameRemoved(const QString&) ), SLOT( nameRemoved(const QString&) ) );

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

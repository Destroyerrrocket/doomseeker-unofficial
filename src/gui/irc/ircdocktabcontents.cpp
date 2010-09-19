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
#include "irc/ircchatadapter.h"
#include "irc/ircdock.h"
#include "irc/ircnetworkadapter.h"

IRCDockTabContents::IRCDockTabContents(IRCDock* pParentIRCDock)
: QWidget(pParentIRCDock)
{
	setupUi(this);

	this->pParentIRCDock = pParentIRCDock;

	connect(btnSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ));
	connect(leCommandLine, SIGNAL( returnPressed() ), this, SLOT( sendMessage() ));
}

void IRCDockTabContents::newChatWindowIsOpened(IRCChatAdapter* pAdapter)
{
	pParentIRCDock->addIRCAdapter(pAdapter);
}

void IRCDockTabContents::receiveError(const QString& error)
{
	txtOutputWidget->moveCursor(QTextCursor::End);
	txtOutputWidget->insertHtml("<font color='#ff0000'>Error: " + error + "</font><br />");
}

void IRCDockTabContents::receiveMessage(const QString& message)
{
	txtOutputWidget->moveCursor(QTextCursor::End);
	txtOutputWidget->insertPlainText(message + "\n");
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
	connect(pIrcAdapter, SIGNAL( titleChange() ), SLOT( adapterTitleChange() ) );

	if (pIrcAdapter->adapterType() == IRCAdapterBase::NetworkAdapter)
	{
		IRCNetworkAdapter* pNetworkAdapter = (IRCNetworkAdapter*)pAdapter;
		connect(pNetworkAdapter, SIGNAL( newChatWindowIsOpened(IRCChatAdapter*) ), SLOT( newChatWindowIsOpened(IRCChatAdapter*) ) );
	}
	
}

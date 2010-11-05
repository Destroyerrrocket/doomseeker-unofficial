//------------------------------------------------------------------------------
// ircdock.cpp
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
#include "ircdock.h"
#include "gui/irc/ircdocktabcontents.h"
#include "gui/irc/ircnetworkselectionbox.h"
#include "irc/configuration/ircconfig.h"
#include "irc/ircglobalmessages.h"
#include "irc/ircnetworkadapter.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QToolBar>

IRCDock::IRCDock(QWidget* parent)
: QDockWidget(parent)
{
	setupUi(this);
	this->toggleViewAction()->setIcon(QIcon(":/icons/irc.png"));
	
	setupToolbar();
	
	IRCGlobalMessages& ircGlobalMessages = IRCGlobalMessages::instance();
	
	connect(tabWidget, SIGNAL( currentChanged(int) ), 
		SLOT( tabCurrentChanged(int) ));
	
	connect(tabWidget, SIGNAL( tabCloseRequested(int) ), 
		SLOT( tabCloseRequestedSlot(int) ));
	
	connect(&ircGlobalMessages, SIGNAL( message(const QString&, IRCAdapterBase*) ), 
		SLOT( globalMessage(const QString&, IRCAdapterBase*) ) );
	
	connect(&ircGlobalMessages, SIGNAL( messageWithClass(const QString&, const IRCMessageClass&, IRCAdapterBase*) ), 
		SLOT( globalMessageWithClass(const QString&, const IRCMessageClass&, IRCAdapterBase*) ) );
}

IRCDockTabContents* IRCDock::addIRCAdapter(IRCAdapterBase* pIRCAdapter)
{
	IRCDockTabContents* pNewAdapterWidget = new IRCDockTabContents(this);

	connect(pNewAdapterWidget, SIGNAL( chatWindowCloseRequest(IRCDockTabContents*) ), SLOT( chatWindowCloseRequestSlot(IRCDockTabContents*) ) );
	connect(pNewAdapterWidget, SIGNAL( focusRequest(IRCDockTabContents*) ), SLOT( tabFocusRequest(IRCDockTabContents*) ) );
	connect(pNewAdapterWidget, SIGNAL( titleChange(IRCDockTabContents*) ), SLOT( titleChange(IRCDockTabContents*) ) );

	pNewAdapterWidget->setIRCAdapter(pIRCAdapter);
	tabWidget->addTab(pNewAdapterWidget, pNewAdapterWidget->icon(), pNewAdapterWidget->title());
	this->titleChange(pNewAdapterWidget);
	
	return pNewAdapterWidget;
}

void IRCDock::applyAppearanceSettings()
{
	for (int i = 0; i < tabWidget->count(); ++i)
	{
		IRCDockTabContents* pWidget = (IRCDockTabContents*)tabWidget->widget(i);
		pWidget->applyAppearanceSettings();
	}
}

void IRCDock::chatWindowCloseRequestSlot(IRCDockTabContents* pCaller)
{
	int tabIndex = tabWidget->indexOf(pCaller);
	if (tabIndex >= 0)
	{
		tabCloseRequestedSlot(tabIndex);
	}
}

void IRCDock::globalMessage(const QString& message, IRCAdapterBase* pMessageSender)
{
	IRCDockTabContents* pWidget = (IRCDockTabContents*)tabWidget->currentWidget();
	if (pWidget != NULL)
	{
		QString prefixedMessage = prefixMessage(pWidget->ircAdapter(), pMessageSender, message);
		pWidget->receiveMessage(prefixedMessage);
	}
}

void IRCDock::globalMessageWithClass(const QString& message, const IRCMessageClass& messageClass, IRCAdapterBase* pMessageSender)
{
	IRCDockTabContents* pWidget =  (IRCDockTabContents*)tabWidget->currentWidget();
	
	bool bIsAdapterRelated = pWidget != NULL
		&& pWidget->ircAdapter()->network()->isAdapterRelated(pMessageSender);
	
	if (bIsAdapterRelated || pMessageSender == NULL)
	{
		QString prefixedMessage = prefixMessage(pWidget->ircAdapter(), pMessageSender, message);
		pWidget->receiveMessageWithClass(prefixedMessage, messageClass);
	}
	else
	{
		pMessageSender->emitMessageWithClass(message, messageClass);
	}
}

bool IRCDock::hasTabFocus(const IRCDockTabContents* pTab) const
{
	return (this->tabWidget->currentWidget() == pTab);
}

IRCNetworkAdapter* IRCDock::networkWithUiFocus()
{
	IRCDockTabContents* pWidget = (IRCDockTabContents*)tabWidget->currentWidget();
	if (pWidget == NULL)
	{
		return NULL;
	}
	
	return pWidget->ircAdapter()->network();
}

void IRCDock::performNetworkAutojoins()
{
	IRCNetworkConnectionInfo connectionInfo;
	connectionInfo.alternateNick = gIRCConfig.personal.alternativeNickname;
	connectionInfo.nick = gIRCConfig.personal.nickname;
	connectionInfo.realName = gIRCConfig.personal.fullName;
	
	connectionInfo.fillInMissingFields();

	QVector<IRCNetworkEntity> autojoinNetworks = gIRCConfig.networks.autojoinNetworks();
	foreach (const IRCNetworkEntity& network, autojoinNetworks)
	{
		connectionInfo.networkEntity = network;
		
		IRCNetworkAdapter* pIRCNetworkAdapter = new IRCNetworkAdapter();

		// Setup the UI tab for the new network.
		addIRCAdapter(pIRCNetworkAdapter);
				
		// Connect to the network.
		pIRCNetworkAdapter->connect(connectionInfo);
	}
}

QString IRCDock::prefixMessage(IRCAdapterBase* pTargetChatWindow, IRCAdapterBase* pMessageSender, const QString& message)
{
	if (pMessageSender != NULL)
	{
		IRCNetworkAdapter* pTargetNetwork = pTargetChatWindow->network();
		if (pTargetNetwork !=  pMessageSender)
		{
			return QString("%1: %2").arg(pMessageSender->title(), message);
		}
	}
	
	return message;
}

void IRCDock::setupToolbar()
{
	QToolBar* pToolBar = new QToolBar(this);
	pToolBar->setMovable(false);

	toolBarConnect = new QAction(QIcon(":/icons/network-connect-3.png"), tr("Connect"), pToolBar);
	toolBarOpenChatWindow = new QAction(QIcon(":/icons/irc_channel.png"), tr("Open chat window"), pToolBar);

	pToolBar->addAction(toolBarConnect);
	pToolBar->addAction(toolBarOpenChatWindow);

	verticalLayout->insertWidget(0, pToolBar);
	connect(pToolBar, SIGNAL( actionTriggered(QAction*) ), this, SLOT( toolBarAction(QAction*) ) );
}

void IRCDock::tabCloseRequestedSlot(int index)
{
	QWidget* pPageWidget = tabWidget->widget(index);
	tabWidget->removeTab(index);
	
	delete pPageWidget;
}

void IRCDock::tabCurrentChanged(int index)
{
	tabWidget->tabBarPublic()->setTabTextColor(index, "");
	IRCDockTabContents* pTab = (IRCDockTabContents*) tabWidget->widget(index);
	pTab->grabFocus();
}

void IRCDock::tabFocusRequest(IRCDockTabContents* pCaller)
{
	tabWidget->setCurrentWidget(pCaller);
}

void IRCDock::titleChange(IRCDockTabContents* pCaller)
{
	int tabIndex = tabWidget->indexOf(pCaller);
	if (tabIndex >= 0)
	{
		QString newTitle = pCaller->title();
		tabWidget->setTabText(tabIndex, newTitle);
		tabWidget->tabBarPublic()->setTabTextColor(tabIndex, pCaller->titleColor());
	}
}

void IRCDock::toolBarAction(QAction* pAction)
{
	if (pAction == toolBarConnect)
	{
		IRCNetworkSelectionBox networkSelection(this);
		if (networkSelection.exec() == QDialog::Accepted)
		{
			IRCNetworkConnectionInfo connectionInfo = networkSelection.networkConnectionInfo();
			gIRCConfig.networks.lastUsedNetwork = connectionInfo.networkEntity;
			
			// We will attempt to remember user credentials for further use.
			gIRCConfig.personal.alternativeNickname = connectionInfo.alternateNick;
			gIRCConfig.personal.nickname = connectionInfo.nick;
			gIRCConfig.personal.fullName = connectionInfo.realName;		
			
			connectionInfo.fillInMissingFields();
			
			IRCNetworkAdapter* pIRCNetworkAdapter = new IRCNetworkAdapter();

			// Setup the UI tab for the new network.
			IRCDockTabContents* pTab = addIRCAdapter(pIRCNetworkAdapter);
			
			// Connect to the network.
			pIRCNetworkAdapter->connect(connectionInfo);
			
			tabFocusRequest(pTab);
		}
	}
	else if (pAction == toolBarOpenChatWindow)
	{
		IRCNetworkAdapter* pNetwork = networkWithUiFocus();
		if (pNetwork == NULL)
		{
			QMessageBox::warning(NULL, tr("Doomseeker IRC - Open chat window"), tr("Cannot obtain network connection adapter."));
		}
		else if (!pNetwork->isConnected())
		{
			QMessageBox::warning(NULL, tr("Doomseeker IRC - Open chat window"), tr("You are not connected to this network."));
		}
		else
		{
			QString recipientName = QInputDialog::getText(NULL, tr("Open chat window"), tr("Specify a channel or user name:"));
			if (!recipientName.isEmpty())
			{
				pNetwork->openNewAdapter(recipientName);
			}
		}
	}
}

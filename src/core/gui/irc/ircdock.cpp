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
#include "gui/irc/ircsounds.h"
#include "irc/configuration/chatnetworkscfg.h"
#include "irc/configuration/ircconfig.h"
#include "irc/ircnetworkadapter.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QToolBar>

IRCDock::IRCDock(QWidget* parent)
: QDockWidget(parent)
{
	setupUi(this);
	this->toggleViewAction()->setIcon(QIcon(":/icons/chat.png"));

	pSounds = new IRCSounds();
	pSounds->loadFromConfig();

	setupToolbar();

	connect(tabWidget, SIGNAL( currentChanged(int) ),
		SLOT( tabCurrentChanged(int) ));

	connect(tabWidget, SIGNAL( tabCloseRequested(int) ),
		SLOT( tabCloseRequestedSlot(int) ));
}

IRCDock::~IRCDock()
{
	delete pSounds;
}

IRCDockTabContents* IRCDock::addIRCAdapter(IRCAdapterBase* pIRCAdapter)
{
	IRCDockTabContents* pNewAdapterWidget = new IRCDockTabContents(this);

	connect(pNewAdapterWidget, SIGNAL( chatWindowCloseRequest(IRCDockTabContents*) ), SLOT( chatWindowCloseRequestSlot(IRCDockTabContents*) ) );
	connect(pNewAdapterWidget, SIGNAL( focusRequest(IRCDockTabContents*) ), SLOT( tabFocusRequest(IRCDockTabContents*) ) );
	connect(pNewAdapterWidget, SIGNAL( titleChange(IRCDockTabContents*) ), SLOT( titleChange(IRCDockTabContents*) ) );
	connect(pNewAdapterWidget, SIGNAL(newMessagePrinted()),
		SLOT(titleChangeWithColorOfSenderIfNotFocused()));
	connect(pNewAdapterWidget, SIGNAL(titleBlinkRequested()),
		SLOT(titleChangeWithColorOfSenderIfNotFocused()));

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

void IRCDock::connectToNewNetwork(IRCNetworkConnectionInfo& connectionInfo, bool bFocusOnNewTab)
{
	IRCNetworkAdapter* pIRCNetworkAdapter = new IRCNetworkAdapter(connectionInfo);

	// Switch this to true only for debug.
	pIRCNetworkAdapter->setEmitAllIRCMessagesEnabled(false);

	// Setup the UI tab for the new network.
	IRCDockTabContents* pTab = addIRCAdapter(pIRCNetworkAdapter);

	// Connect to the network.
	pIRCNetworkAdapter->connect();

	if (bFocusOnNewTab)
	{
		tabFocusRequest(pTab);
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

	QList<IRCNetworkEntity> autojoinNetworks = ChatNetworksCfg().autoJoinNetworks();
	foreach (const IRCNetworkEntity& network, autojoinNetworks)
	{
		connectionInfo.networkEntity = network;

		connectToNewNetwork(connectionInfo, false);
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
	pToolBar->setOrientation(Qt::Vertical);

	toolBarConnect = new QAction(QIcon(":/icons/network-connect-3.png"), tr("Connect"), pToolBar);
	toolBarOpenChatWindow = new QAction(QIcon(":/icons/irc_channel.png"), tr("Open chat window"), pToolBar);

	pToolBar->addAction(toolBarConnect);
	pToolBar->addAction(toolBarOpenChatWindow);

	horizontalLayout->insertWidget(0, pToolBar);
	connect(pToolBar, SIGNAL( actionTriggered(QAction*) ), this, SLOT( toolBarAction(QAction*) ) );
}

IRCSounds& IRCDock::sounds()
{
	return *pSounds;
}

void IRCDock::tabCloseRequestedSlot(int index)
{
	QWidget* pPageWidget = tabWidget->widget(index);
	tabWidget->removeTab(index);

	delete pPageWidget;
}

void IRCDock::tabCurrentChanged(int index)
{
	if (index >= 0)
	{
		tabWidget->tabBarPublic()->setTabTextColor(index, "");
		IRCDockTabContents* pTab = (IRCDockTabContents*) tabWidget->widget(index);
		pTab->grabFocus();
	}
}

void IRCDock::tabFocusRequest(IRCDockTabContents* pCaller)
{
	tabWidget->setCurrentWidget(pCaller);
}

IRCDockTabContents *IRCDock::tabWithFocus()
{
	if (tabWidget->currentWidget() != NULL)
	{
		return static_cast<IRCDockTabContents*>(tabWidget->currentWidget());
	}
	return NULL;
}

void IRCDock::titleChange(IRCDockTabContents* caller)
{
	int tabIndex = tabWidget->indexOf(caller);
	if (tabIndex >= 0)
	{
		tabWidget->setTabText(tabIndex, caller->title());
	}
}

void IRCDock::titleChangeWithColorOfSenderIfNotFocused()
{
	IRCDockTabContents* caller = static_cast<IRCDockTabContents*>(sender());
	int tabIndex = tabWidget->indexOf(caller);
	if (tabIndex >= 0)
	{
		tabWidget->setTabText(tabIndex, caller->title());
		tabWidget->tabBarPublic()->setTabTextColor(tabIndex, caller->titleColor());
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
			ChatNetworksCfg().setLastUsedNetwork(connectionInfo.networkEntity);

			// We will attempt to remember user credentials for further use.
			gIRCConfig.personal.alternativeNickname = connectionInfo.alternateNick;
			gIRCConfig.personal.nickname = connectionInfo.nick;
			gIRCConfig.personal.fullName = connectionInfo.realName;

			connectionInfo.fillInMissingFields();

			connectToNewNetwork(connectionInfo, true);
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

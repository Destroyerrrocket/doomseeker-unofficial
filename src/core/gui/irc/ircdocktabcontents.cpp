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
#include "gui/irc/ircsounds.h"
#include "gui/commonGUI.h"
#include "irc/configuration/ircconfig.h"
#include "irc/ircchanneladapter.h"
#include "irc/ircdock.h"
#include "irc/ircglobal.h"
#include "irc/ircmessageclass.h"
#include "irc/ircnetworkadapter.h"
#include "irc/ircuserinfo.h"
#include "irc/ircuserlist.h"
#include "log.h"
#include <QScrollBar>
#include <QStandardItemModel>

const int IRCDockTabContents::BLINK_TIMER_DELAY_MS = 650;

IRCDockTabContents::IRCDockTabContents(IRCDock* pParentIRCDock)
{
	setupUi(this);
	
	this->bBlinkTitle = false;
	this->bIsDestroying = false;
	this->lastMessageClass = NULL;
	this->userListContextMenu = NULL;

	this->pParentIRCDock = pParentIRCDock;
	this->lvUserList->setModel(new QStandardItemModel(this->lvUserList));

	// There is only one case in which we want this to be visible:
	// if we are in a channel.
	this->lvUserList->setVisible(false);

	connect(btnSend, SIGNAL( clicked() ), this, SLOT( sendMessage() ));
	connect(leCommandLine, SIGNAL( returnPressed() ), this, SLOT( sendMessage() ));
	
	applyAppearanceSettings();
	
	blinkTimer.setSingleShot(false);
	this->connect(&blinkTimer, SIGNAL( timeout() ),
		SLOT( blinkTimerSlot() ) );
	
	// Performance check line, keep commented for non-testing builds:
	//receiveMessage(Strings::createRandomAlphaNumericStringWithNewLines(80, 5000));
}

IRCDockTabContents::~IRCDockTabContents()
{
	this->bIsDestroying = true;
	
	if (this->lastMessageClass != NULL)
	{
		delete this->lastMessageClass;
	}

	if (this->userListContextMenu != NULL)
	{
		delete this->userListContextMenu;
	}

	if (pIrcAdapter != NULL)
	{
		disconnect(pIrcAdapter, 0, 0, 0);
		IRCAdapterBase* pTmpAdapter = pIrcAdapter;
		pIrcAdapter = NULL;
		delete pTmpAdapter;	
	}
}

void IRCDockTabContents::adapterFocusRequest()
{
	emit focusRequest(this);
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

void IRCDockTabContents::applyAppearanceSettings()
{
	const static QString STYLE_SHEET_BASE_TEMPLATE = 
		"QListView, QTextEdit, QLineEdit { background: %1; color: %2; } ";
		
	const IRCConfig::AppearanceCfg& appearance = gIRCConfig.appearance;
		
	QString qtStyleSheet = STYLE_SHEET_BASE_TEMPLATE
		.arg(appearance.backgroundColor)
		.arg(appearance.defaultTextColor);
		
	QColor colorSelectedText("#cbcb0f");
	QColor colorSelectedBackground("#B74600");
	
	QColor colorHoverText = colorSelectedText.lighter();
	QColor colorHoverBackground = colorSelectedBackground.lighter();
		
	qtStyleSheet += QString("QListView::item:selected { color: %1; background: %2; } ").arg(colorSelectedText.name(), colorSelectedBackground.name());
	qtStyleSheet += QString("QListView::item:hover  { color: %1; background: %2; } ").arg(colorHoverText.name(), colorHoverBackground.name());
		
	QString channelActionClassName = IRCMessageClass::toStyleSheetClassName(IRCMessageClass::ChannelAction);
	QString errorClassName = IRCMessageClass::toStyleSheetClassName(IRCMessageClass::Error);
	QString networkActionClassName = IRCMessageClass::toStyleSheetClassName(IRCMessageClass::NetworkAction);
	
	QString htmlStyleSheetMessageArea = "";
	htmlStyleSheetMessageArea += "span { white-space: pre; }";
	htmlStyleSheetMessageArea += QString("a { color: %1; white-space: pre; } ").arg(appearance.urlColor);
	htmlStyleSheetMessageArea += QString("." + channelActionClassName + " { color: %1; } ").arg(appearance.channelActionColor);
	htmlStyleSheetMessageArea += QString("." + errorClassName + " { color: %1; } ").arg(appearance.errorColor);
	htmlStyleSheetMessageArea += QString("." + networkActionClassName + " { color: %1; } ").arg(appearance.networkActionColor);
		 
	this->lvUserList->setStyleSheet(qtStyleSheet);
	this->lvUserList->setFont(appearance.userListFont);
	
	this->leCommandLine->setStyleSheet(qtStyleSheet);
	this->leCommandLine->setFont(appearance.mainFont);
	
	this->txtOutputWidget->setStyleSheet(qtStyleSheet);
	this->txtOutputWidget->setFont(appearance.mainFont);
	
	this->txtOutputWidget->document()->setDefaultStyleSheet(htmlStyleSheetMessageArea);
	this->txtOutputWidget->clear();
	this->txtOutputWidget->insertHtml(this->textOutputContents.join(""));
	this->txtOutputWidget->moveCursor(QTextCursor::End);
}

void IRCDockTabContents::blinkTimerSlot()
{
	setBlinkTitle(!bBlinkTitle);
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

IRCDockTabContents::UserListMenu& IRCDockTabContents::getUserListContextMenu()
{
	if (this->userListContextMenu == NULL)
	{
		this->userListContextMenu = new UserListMenu();
	}
	
	return *this->userListContextMenu;
}

void IRCDockTabContents::grabFocus()
{
	// Make sure the tab title is not "blinked out" anymore.
	blinkTimer.stop();
	setBlinkTitle(false);

	this->leCommandLine->setFocus();
}

bool IRCDockTabContents::hasTabFocus() const
{
	return this->pParentIRCDock->hasTabFocus(this);
}

QIcon IRCDockTabContents::icon() const
{
	if (pIrcAdapter == NULL)
	{
		return QIcon();
	}
	
	switch (pIrcAdapter->adapterType())
	{
		case IRCAdapterBase::ChannelAdapter:
			return QIcon(":/icons/irc_channel.png");
			
		case IRCAdapterBase::NetworkAdapter:
			return QIcon(":/flags/lan-small");
			
		case IRCAdapterBase::PrivAdapter:
			return QIcon(":/icons/person.png");
			
		default:
			return QIcon();
	}
}

void IRCDockTabContents::insertMessage(const IRCMessageClass& messageClass, const QString& htmlString)
{
	if (this->lastMessageClass == NULL)
	{
		this->lastMessageClass = new IRCMessageClass();
	}
	*this->lastMessageClass = messageClass;
	
	this->textOutputContents << htmlString;

	// Text insertion must be done this way to allow proper
	// handling of "Pause" button. Note that the cursor
	// in the widget is not affected as textCursor() creates a copy
	// of cursor object.	
	QTextCursor cursor = this->txtOutputWidget->textCursor();
	cursor.movePosition(QTextCursor::End);
	cursor.insertHtml(htmlString);
	
	if (!btnPauseTextArea->isChecked())
	{
		this->txtOutputWidget->moveCursor(QTextCursor::End);
	}
	
	if (!this->hasTabFocus())
	{
		emit titleChange(this);
	}
}

void IRCDockTabContents::myNicknameUsedSlot()
{
	pParentIRCDock->sounds().playIfAvailable(IRCSounds::NicknameUsed);
	if (!hasTabFocus())
	{
		blinkTimer.start(BLINK_TIMER_DELAY_MS);
	}
}

void IRCDockTabContents::nameAdded(const IRCUserInfo& userInfo)
{
	QString nickname = userInfo.prefixedName();

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
	this->lvUserList->setModel(new QStandardItemModel(this->lvUserList));

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
	// Once a new chat adapter is opened we need to add it to the master
	// dock widget.
	pParentIRCDock->addIRCAdapter(pAdapter);
}

void IRCDockTabContents::receiveError(const QString& error)
{
	receiveMessageWithClass(tr("Error: %1").arg(error), IRCMessageClass::Error);
}

void IRCDockTabContents::receiveMessage(const QString& message)
{
	receiveMessageWithClass(message, IRCMessageClass::Normal);
}

void IRCDockTabContents::receiveMessageWithClass(const QString& message, const IRCMessageClass& messageClass)
{
	QString className = messageClass.toStyleSheetClassName();
	
	QString messageHtmlEscaped = message;
	
	// As the new-line character is stripped by the lower levels we should
	// assume that each message ends with a new-line char, as specified
	// by RFC 1459.
	messageHtmlEscaped += "\n";
	
	messageHtmlEscaped.replace("<", "&lt;").replace(">", "&gt;");
	
	messageHtmlEscaped = Strings::wrapUrlsWithHtmlATags(messageHtmlEscaped);
	
	if (gIRCConfig.appearance.timestamps)
	{
		messageHtmlEscaped = Strings::timestamp("[hh:mm:ss] ") + messageHtmlEscaped;
	}
	
	if (className.isEmpty())
	{
		messageHtmlEscaped = "<span>" + messageHtmlEscaped + "</span>";
	}
	else
	{
		messageHtmlEscaped = ("<span class='" + className + "'>" + messageHtmlEscaped + "</span>");
	}
	
	// Play sound if this is Priv adapter.
	if (pIrcAdapter->adapterType() == IRCAdapterBase::PrivAdapter)
	{
		pParentIRCDock->sounds().playIfAvailable(IRCSounds::PrivateMessageReceived);
		
		// If this tab doesn't have focus, also start blinking the title.
		if (!hasTabFocus())
		{
			blinkTimer.start(BLINK_TIMER_DELAY_MS);
		}
	}
	
	this->insertMessage(messageClass, messageHtmlEscaped);
}

QString IRCDockTabContents::selectedNickname()
{
	QModelIndexList selectedIndexes = this->lvUserList->selectionModel()->selectedRows();
	// There can be only one.
	if (!selectedIndexes.isEmpty())
	{
		int row = selectedIndexes[0].row();
		QStandardItemModel* pModel = (QStandardItemModel*)this->lvUserList->model();
		QStandardItem* pItem = pModel->item(row);
	
		return pItem->text();
	}
	
	return "";
}

void IRCDockTabContents::sendMessage()
{
	QString message = leCommandLine->text();
	leCommandLine->setText("");
	
	if (!message.trimmed().isEmpty())
	{
		pIrcAdapter->sendMessage(message);
	}
}

void IRCDockTabContents::setBlinkTitle(bool b)
{
	bool bEmit = false;
	if (bBlinkTitle != b)
	{
		// Delay signal emit until after we change the variable.
		bEmit = true;
	}
	
	this->bBlinkTitle = b;
	
	if (bEmit)
	{
		emit titleChange(this);
	}
}

void IRCDockTabContents::setIRCAdapter(IRCAdapterBase* pAdapter)
{
	pIrcAdapter = pAdapter;
	connect(pIrcAdapter, SIGNAL( error(const QString&) ), SLOT( receiveError(const QString& ) ));
	connect(pIrcAdapter, SIGNAL( focusRequest() ), SLOT( adapterFocusRequest() ));
	connect(pIrcAdapter, SIGNAL( message(const QString&) ), SLOT( receiveMessage(const QString& ) ));
	connect(pIrcAdapter, SIGNAL( messageWithClass(const QString&, const IRCMessageClass&) ), SLOT( receiveMessageWithClass(const QString&, const IRCMessageClass&) ));
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
			connect(pChannelAdapter, SIGNAL( myNicknameUsed() ), SLOT( myNicknameUsedSlot() ) );
			connect(pChannelAdapter, SIGNAL( nameAdded(const IRCUserInfo&) ), SLOT( nameAdded(const IRCUserInfo&) ) );
			connect(pChannelAdapter, SIGNAL( nameListUpdated(const IRCUserList&) ), SLOT( nameListUpdated(const IRCUserList&) ) );
			connect(pChannelAdapter, SIGNAL( nameRemoved(const IRCUserInfo&) ), SLOT( nameRemoved(const IRCUserInfo&) ) );
			connect(pChannelAdapter, SIGNAL( nameUpdated(const IRCUserInfo&) ), SLOT( nameUpdated(const IRCUserInfo&) ) );

			this->lvUserList->setVisible(true);
			connect(this->lvUserList, SIGNAL( customContextMenuRequested(const QPoint&) ), 
				SLOT( userListCustomContextMenuRequested(const QPoint&) ) );
				
			connect(this->lvUserList, SIGNAL( doubleClicked(const QModelIndex&) ),
				SLOT( userListDoubleClicked(const QModelIndex&) ) ); 
			
			this->lvUserList->setContextMenuPolicy(Qt::CustomContextMenu);
			
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

QString IRCDockTabContents::title() const
{
	return pIrcAdapter->title();
}

QString IRCDockTabContents::titleColor() const
{
	if (this->lastMessageClass != NULL && !this->hasTabFocus())
	{
		QString color;
	
		if (*this->lastMessageClass == IRCMessageClass::Normal)
		{
			color = "#ff0000";
		}
		else
		{
			color = this->lastMessageClass->colorFromConfig();
		}
		
		if (bBlinkTitle)
		{
			QColor c(color);
			
			int rInverted = 0xff - c.red();
			int gInverted = 0xff - c.green();
			int bInverted = 0xff - c.blue();
			
			QColor inverted(rInverted, gInverted, bInverted);
			
			return inverted.name();
		}
		else
		{
			return color;
		}
	}
	
	return "";
}

void IRCDockTabContents::userListCustomContextMenuRequested(const QPoint& pos)
{
	if (this->pIrcAdapter->adapterType() != IRCAdapterBase::ChannelAdapter)
	{
		// Prevent illegal calls.
		return;
	}
	
	QString nickname = this->selectedNickname();	
	if (nickname.isEmpty())
	{
		// Prevent calls if there is no one selected.
		return;
	}
	QString cleanNickname = IRCUserInfo(nickname).cleanNickname();
	
	IRCChannelAdapter* pAdapter = (IRCChannelAdapter*) this->pIrcAdapter;	
	const QString& channel = pAdapter->recipient();

	UserListMenu& menu = this->getUserListContextMenu();
	menu.setIsOperator(pAdapter->amIOperator());
	menu.applyTargetUserFlags(nickname);
	QPoint posGlobal = this->lvUserList->mapToGlobal(pos);
	
	QAction* pAction = menu.exec(posGlobal);
	
	if (pAction == NULL)
	{
		return;
	}
	
	if (pAction == menu.ban)
	{
		bool bOk = false;
	
		QString reason = CommonGUI::askString(tr("Ban user"), tr("Input reason for banning user %1 from channel %2").arg(nickname, channel), &bOk);
		if (bOk)
		{
			pAdapter->banUser(cleanNickname, reason);
		}
	}
	else if (pAction == menu.deop)
	{
		pAdapter->setOp(cleanNickname, false);
	}
	else if (pAction == menu.devoice)
	{
		pAdapter->setVoiced(cleanNickname, false);
	}
	else if (pAction == menu.kick)
	{
		bool bOk = false;
	
		QString reason = CommonGUI::askString(tr("Kick user"), tr("Input reason for kicking user %1 from channel %2").arg(nickname, channel), &bOk);
		if (bOk)
		{
			pAdapter->kickUser(cleanNickname, reason);
		}
	}
	else if (pAction == menu.op)
	{
		pAdapter->setOp(cleanNickname, true);
	}
	else if (pAction == menu.openChatWindow)
	{
		pAdapter->network()->openNewAdapter(cleanNickname);
	}
	else if (pAction == menu.voice)
	{
		pAdapter->setVoiced(cleanNickname, true);
	}
}

void IRCDockTabContents::userListDoubleClicked(const QModelIndex& index)
{
	if (this->pIrcAdapter->adapterType() != IRCAdapterBase::ChannelAdapter)
	{
		// Prevent illegal calls.
		return;
	}

	QString nickname = this->selectedNickname();	
	if (nickname.isEmpty())
	{
		// Prevent calls if there is no one selected.
		return;
	}
	QString cleanNickname = IRCUserInfo(nickname).cleanNickname();
	
	this->pIrcAdapter->network()->openNewAdapter(cleanNickname);
}

//////////////////////////////////////////////////////////////////////////////
IRCDockTabContents::UserListMenu::UserListMenu()
{
	this->openChatWindow = this->addAction(tr("Open chat window"));
	this->addSeparator();
	this->op = this->addAction(tr("Op"));
	this->deop = this->addAction(tr("Deop"));
	this->voice = this->addAction(tr("Voice"));
	this->devoice = this->addAction(tr("Devoice"));
	this->addSeparator();
	this->kick = this->addAction(tr("Kick"));
	this->ban = this->addAction(tr("Ban"));
	
	this->bIsOperator = false;
}

void IRCDockTabContents::UserListMenu::applyTargetUserFlags(const IRCUserInfo& userInfo)
{
	if (this->bIsOperator)
	{
		if (userInfo.isOp())
		{
			this->devoice->setEnabled(false);
			this->op->setEnabled(false);
			this->voice->setEnabled(false);
		}
		else
		{
			this->devoice->setEnabled(userInfo.isVoiced());
			this->deop->setEnabled(false);
			this->voice->setEnabled(!userInfo.isVoiced());
			
		}
	}
}

void IRCDockTabContents::UserListMenu::setIsOperator(bool bOperator)
{
	this->bIsOperator = bOperator;

	this->op->setEnabled(bOperator);
	this->deop->setEnabled(bOperator);
	this->voice->setEnabled(bOperator);
	this->devoice->setEnabled(bOperator);
	this->kick->setEnabled(bOperator);
	this->ban->setEnabled(bOperator);
}
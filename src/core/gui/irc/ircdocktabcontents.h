//------------------------------------------------------------------------------
// ircdocktabcontents.h
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
#ifndef __IRCDOCK_TAB_CONTENTS_H_
#define __IRCDOCK_TAB_CONTENTS_H_

#include "irc/ircadapterbase.h"
#include "irc/ircmessageclass.h"

#include "ui_ircdocktabcontents.h"
#include <QAction>
#include <QStandardItem>
#include <QMenu>
#include <QTimer>
#include <QWidget>

class IRCChatAdapter;
class IRCDock;
class IRCNicknameCompleter;
class IRCUserInfo;
class IRCUserList;

/**
 *	@brief Dockable widget designed for IRC communication.
 */
class IRCDockTabContents : public QWidget, private Ui::IRCDockTabContents
{
	Q_OBJECT;

	public:
		IRCDockTabContents(IRCDock* pParentIRCDock);
		~IRCDockTabContents();
		
		/**
		 *	@brief Applies current appearance settings from the IRC config.
		 */
		void applyAppearanceSettings();
		
		/** 
		 *	@brief Called when tab becomes active.
		 *
		 *	Informs the tab that it should grab keyboard focus.
		 *	Text input widget will be selected.
		 */
		void grabFocus();
		bool hasTabFocus() const;
		
		QIcon icon() const;

		IRCAdapterBase* ircAdapter() const { return pIrcAdapter; }
		
		/**
		 *	@brief Calling this multiple times on the same object will cause
		 *	memory leaks.
		 */
		void setIRCAdapter(IRCAdapterBase* pAdapter);
		
		QString title() const;
		QString titleColor() const;
		
	public slots:
		void receiveMessage(const QString& message);
		void receiveMessageWithClass(const QString& message, const IRCMessageClass& messageClass);

	signals:
		/**
		 *	@brief Emitted when the IRCAdapterBase that is associated
		 *	with this widget is no longer valid - possibly even deleted.
		 *
		 *	Capture this to close this widget.
		 */
		void chatWindowCloseRequest(IRCDockTabContents*);
		
		/**
		 *	@brief Emitted when the variable returned by 
		 *	IRCAdapterBase::title() might have changed and the 
		 *	application should be notified of this fact.
		 */
		void titleChange(IRCDockTabContents* pCaller);
		
		/**
		 *	@brief Emitted when network adapter for this dock emits
		 *	its focusRequest() signal.
		 */
		void focusRequest(IRCDockTabContents* pCaller);

	protected slots:
		void adapterFocusRequest();
		void adapterTerminating();
	
		void adapterTitleChange()
		{
			emit titleChange(this);
		}

		void nameAdded(const IRCUserInfo& userInfo);
		void nameListUpdated(const IRCUserList& userList);
		void nameRemoved(const IRCUserInfo& userInfo);
		void nameUpdated(const IRCUserInfo& userInfo);
		
		/**
		 *	@brief Captures signals from IRC Networks which indicate that a new
		 *	chat window is being opened.
		 */
		void newChatWindowIsOpened(IRCChatAdapter* pAdapter);
		
		void myNicknameUsedSlot();
		
		void receiveError(const QString& error);
		void sendMessage();
		void userListCustomContextMenuRequested(const QPoint& pos);
		void userListDoubleClicked(const QModelIndex& index);

	protected:
		IRCAdapterBase* pIrcAdapter;
		IRCDock* pParentIRCDock;

		bool eventFilter(QObject *watched, QEvent *event);

	private:
		class UserListMenu : public QMenu
		{
			public:
				UserListMenu();

				QAction* ban;
				QAction *ctcpTime;
				QAction *ctcpPing;
				QAction *ctcpVersion;
				QAction* dehalfOp;
				QAction* deop;
				QAction* devoice;
				QAction* halfOp;
				QAction* kick;
				QAction* op;
				QAction* openChatWindow;
				QAction* voice;

			private:
				bool bIsOperator;
			
		};

		class PrivChatMenu;

		static const int BLINK_TIMER_DELAY_MS;
	
		/**
		 *	@brief Holds blinkTimer state.
		 *
		 *	Either text shows in usual color (false) or inverted one (true).
		 *	Change to this variable should be accompanied by emitting
		 *	titleChange() signal.
		 */
		bool bBlinkTitle;
		bool bIsDestroying;
		
		QTimer blinkTimer;
		
		IRCMessageClass* lastMessageClass;
		IRCNicknameCompleter *nicknameCompleter;
		/**
		 *	@brief This is required to properly refresh colors when
		 *	appearance is changed.
		 */
		QStringList textOutputContents;		
		UserListMenu* userListContextMenu;

		void completeNickname();
		QStandardItem* findUserListItem(const QString& nickname);
		UserListMenu& getUserListContextMenu();
		IRCNetworkAdapter* network();
		void insertMessage(const IRCMessageClass& messageClass, const QString& htmlString);
		QString selectedNickname();

		void sendCtcpPing(const QString &nickname);
		void sendCtcpTime(const QString &nickname);
		void sendCtcpVersion(const QString &nickname);

		/**
		 *	Sets bBlinkTitle to specified value and emits
		 *	titleChange() signal if new value was different than the
		 *	previous one.
		 */
		void setBlinkTitle(bool b);
		/**
		 * @brief Deletes current model, applies a new, empty one.
		 */
		void setupNewUserListModel();
		void showPrivChatContextMenu();

	private slots:
		void blinkTimerSlot();
		void resetNicknameCompletion();
		void showChatContextMenu();
};

#endif

//------------------------------------------------------------------------------
// serverfilterdock.h
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
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __SERVERFILTERDOCK_H__
#define __SERVERFILTERDOCK_H__

#include <QDockWidget>

#include "ui_serverfilterdock.h"

class ServerListFilterInfo;

class ServerFilterDock : public QDockWidget, private Ui::ServerFilterDock
{
	Q_OBJECT

	public:
		ServerFilterDock(QWidget* pParent = NULL);

		void					addGameModeToComboBox(const QString& gameMode);

		/**
 		* @brief Creates and/or returns an instance of widget that is located
 		* in the MainWindow toolbar.
 		*/
		QLineEdit				*createQuickSearch();

		/**
		 *	@brief Removes all cached items from Game Modes combo box.
		 *
		 *	A new empty item will be added by default.
		 *
		 *	Emits filterUpdated() signal.
		 */
		void					clearGameModes();

		ServerListFilterInfo	filterInfo() const;

		/**
		 *	@brief Sets widgets to new filter info.
		 *
		 *	Emits filterUpdated() signal.
		 */
		void					setFilterInfo(const ServerListFilterInfo& filterInfo);

	signals:
		void					filterUpdated(const ServerListFilterInfo& filterInfo);

	private:
		/**
 		* @brief Quick Search widget that is actually located outside the
 		* ServerFilterDock.
 		*
 		* ServerFilterDock needs to keep track of this widget in order to
 		* update the changing value appropriately.
 		*/
		QLineEdit*              leQuickSearch;

		void					doConnections();

	private slots:
		void					btnClearClicked();
		void					emitUpdated();
		void					emitUpdated(const QString& dummy);
		void					thisVisibilityChanged(bool bVisible);
};

#endif

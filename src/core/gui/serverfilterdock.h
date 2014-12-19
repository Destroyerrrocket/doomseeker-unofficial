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

class ServerListFilterInfo;
class QLineEdit;
class QComboBox;

class ServerFilterDock : public QDockWidget
{
	Q_OBJECT

	public:
		ServerFilterDock(QWidget* pParent = NULL);
		~ServerFilterDock();

		void addGameModeToComboBox(const QString& gameMode);

		/**
 		* @brief Creates and/or returns an instance of widget that is located
 		* in the MainWindow toolbar.
 		*/
		QLineEdit *createQuickSearch();

		ServerListFilterInfo filterInfo() const;

	public slots:
		/**
		 * @brief Sets widgets to new filter info.
		 *
		 * Emits filterUpdated() signal.
		 */
		void setFilterInfo(const ServerListFilterInfo& filterInfo);

	signals:
		void filterUpdated(const ServerListFilterInfo& filterInfo);
		void nonEmptyServerGroupingAtTopToggled(bool b);

	private:
		class PrivData;
		PrivData *d;

		void addSortedNonDuplicate(QComboBox* comboBox, const QString& text);

	private slots:
		void clear();
		void emitUpdated();
		void onServerGroupingChange();
};

#endif

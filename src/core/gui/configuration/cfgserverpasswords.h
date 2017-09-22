//------------------------------------------------------------------------------
// cfgserverpasswords.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef idEB6A779F_CFBD_4900_AFC803D7BF80EC31
#define idEB6A779F_CFBD_4900_AFC803D7BF80EC31

#include "gui/configuration/configpage.h"
#include "dptr.h"
#include <QIcon>

class ServerPassword;
class QTableWidget;
class QTableWidgetItem;

class CFGServerPasswords : public ConfigPage
{
	Q_OBJECT

	public:
		CFGServerPasswords(QWidget* parent=NULL);
		~CFGServerPasswords();

		QIcon icon() const { return QIcon(":/icons/padlock.png"); }
		QString name() const { return tr("Server Passwords"); }

		void readSettings();

	protected:
		void saveSettings();

	private:
		DPtr<CFGServerPasswords> d;

		void addServerPasswordToTable(const ServerPassword& password);
		void clearTable(QTableWidget* table);
		int findPassphraseInTable(const QString& phrase);
		void hidePasswords();
		bool isPassphraseInTable(const QString& phrase);
		void revealPasswords();
		ServerPassword serverPasswordFromRow(int row);
		void setPasswordInRow(int row, const ServerPassword& password);
		void setServersInTable(const ServerPassword& password);
		QTableWidgetItem* toolTipItem(const QString& contents);
		void updatePassword(const ServerPassword& password);

	private slots:
		void addPasswordFromLineEdit();
		void onPasswordTableCellChange(int currentRow, int currentColumn,
			int previousRow, int previousColumn);
		void removeSelectedPasswords();
		void removeSelectedServers();
		void showServerLossWarningIfNecessary();
		void toggleRevealHide();
};

#endif

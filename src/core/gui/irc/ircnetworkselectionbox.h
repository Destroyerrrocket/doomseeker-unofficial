//------------------------------------------------------------------------------
// ircnetworkselectionbox.h
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
#ifndef __IRCNETWORKSELECTIONBOX_H__
#define __IRCNETWORKSELECTIONBOX_H__

#include "ui_ircnetworkselectionbox.h"

#include "irc/entities/ircnetworkentity.h"
#include "irc/ircnetworkconnectioninfo.h"
#include <QDialog>

class IRCNetworkSelectionBox : public QDialog, private Ui::IRCNetworkSelectionBox
{
	Q_OBJECT

	public:
		IRCNetworkSelectionBox(QWidget* parent = NULL);

		void						accept();

		/**
		 *	@brief Extracts network specified in this dialog.
		 *
		 *	This will take all information from the entity selected
		 *	from the combo box. Some values that are directly editable
		 *	through this dialog will be substituted (if changed).
		 */
		IRCNetworkEntity			network() const;

		IRCNetworkConnectionInfo	networkConnectionInfo() const;

	private:
		QVector<IRCNetworkEntity>	networksArray;

		void						fetchNetworks();
		void						initWidgets();

		void						addNetworkToComboBox(const IRCNetworkEntity& network, bool bLastUsed = false);
		/**
		 *	@brief Extracts selected network from combo box.
		 */
		IRCNetworkEntity			networkComboBox() const;
		void						updateNetworkInfo();
		bool						validate();

	private slots:
		void						btnNewNetworkClicked();
		void						networkChanged(int index);
};

#endif

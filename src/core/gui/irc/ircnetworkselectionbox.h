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

#include "dptr.h"

#include <QDialog>

class IRCNetworkConnectionInfo;
class IRCNetworkEntity;

class IRCNetworkSelectionBox : public QDialog
{
	Q_OBJECT

	public:
		IRCNetworkSelectionBox(QWidget* parent = NULL);
		~IRCNetworkSelectionBox();

		void accept();

		/**
		 *	@brief Extracts network specified in this dialog.
		 *
		 *	This will take all information from the entity selected
		 *	from the combo box. Some values that are directly editable
		 *	through this dialog will be substituted (if changed).
		 */
		IRCNetworkEntity network() const;

		IRCNetworkConnectionInfo networkConnectionInfo() const;

	private:
		void fetchNetworks();
		void initWidgets();

		void addNetworkToComboBox(const IRCNetworkEntity& network);
		QString buildTitle(const IRCNetworkEntity &network) const;
		/**
		 *	@brief Extracts selected network from combo box.
		 */
		IRCNetworkEntity networkCurrent() const;
		IRCNetworkEntity networkAtRow(int row) const;
		bool replaceNetworkInConfig(const IRCNetworkEntity &oldNetwork, const IRCNetworkEntity &newNetwork);
		void setNetworkMatchingDescriptionAsCurrent(const QString &description);
		void updateCurrentNetwork(const IRCNetworkEntity &network);
		void updateNetworkInfo();
		bool validate();

		DPtr<IRCNetworkSelectionBox> d;
	private slots:
		void createNewNetwork();
		void editCurrentNetwork();
		void networkChanged(int index);
};

#endif

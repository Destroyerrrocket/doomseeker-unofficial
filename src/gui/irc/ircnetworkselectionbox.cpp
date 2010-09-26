//------------------------------------------------------------------------------
// ircnetworkselectionbox.cpp
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
#include "ircnetworkselectionbox.h"
#include "gui/configuration/irc/cfgircdefinenetworkdialog.h"
#include "irc/configuration/ircconfig.h"
#include "qtmetapointer.h"

// TODO: Add validators.

IRCNetworkSelectionBox::IRCNetworkSelectionBox(QWidget* parent)
: QDialog(parent)
{
	setupUi(this);
	
	connect(btnNewNetwork, SIGNAL( clicked() ), SLOT( btnNewNetworkClicked() ) );
	connect(cboNetwork, SIGNAL( currentIndexChanged(int) ), SLOT( networkChanged(int) ) );
	
	initWidgets();
}

void IRCNetworkSelectionBox::btnNewNetworkClicked()
{
	CFGIRCDefineNetworkDialog dialog(this);
	if (dialog.exec() == QDialog::Accepted)
	{
		gIRCConfig.networks.networks << dialog.getNetworkEntity();
		fetchNetworks();
	}
}

void IRCNetworkSelectionBox::fetchNetworks()
{
	gIRCConfig.networks.networksSortedByDescription(networksArray);
	cboNetwork->clear();
	
	foreach (const IRCNetworkEntity& network, networksArray)
	{
		QString title = QString("%1 [%2:%3]").arg(network.description).arg(network.address).arg(network.port);
		QtMetaPointer metaPointer = (void*)&network;
		QVariant variantPointer = qVariantFromValue(metaPointer);
			
		cboNetwork->addItem(title, variantPointer);
	}
	
	updateNetworkInfo();	
}

void IRCNetworkSelectionBox::initWidgets()
{
	leAlternateNick->setText(gIRCConfig.personal.alternativeNickname);
	leNick->setText(gIRCConfig.personal.nickname);
	leRealName->setText(gIRCConfig.personal.fullName);
	
	fetchNetworks();
}

IRCNetworkEntity IRCNetworkSelectionBox::network() const
{
	IRCNetworkEntity networkEntity = this->networkComboBox();
	
	networkEntity.address = leServerAddress->text();
	networkEntity.password = lePassword->text();
	networkEntity.port = spinPort->value();

	return networkEntity;	
}

void IRCNetworkSelectionBox::networkChanged(int index)
{
	if (index > 0)
	{
		updateNetworkInfo();
	}
}

IRCNetworkEntity IRCNetworkSelectionBox::networkComboBox() const
{
	int index = cboNetwork->currentIndex();
	if (index < 0)
	{
		return IRCNetworkEntity();
	}
	
	QtMetaPointer metaPointer = qVariantValue<QtMetaPointer>(cboNetwork->itemData(index));
	void* pointer = metaPointer;
	IRCNetworkEntity* pNetwork = (IRCNetworkEntity*)pointer;
	
	return *pNetwork;
}

IRCNetworkConnectionInfo IRCNetworkSelectionBox::networkConnectionInfo() const
{
	IRCNetworkConnectionInfo outInfo;

	outInfo.alternateNick = leAlternateNick->text();
	outInfo.nick = leNick->text();
	outInfo.realName = leRealName->text();

	outInfo.networkEntity = this->network();
	
	return outInfo;
}

void IRCNetworkSelectionBox::updateNetworkInfo()
{
	IRCNetworkEntity network = this->networkComboBox();
		
	leServerAddress->setText(network.address);
	spinPort->setValue(network.port);
	lePassword->setText(network.password);
}

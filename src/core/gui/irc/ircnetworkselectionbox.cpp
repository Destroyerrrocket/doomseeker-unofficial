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
#include "irc/configuration/chatnetworkscfg.h"
#include "irc/configuration/ircconfig.h"
#include "qtmetapointer.h"
#include <QMessageBox>

IRCNetworkSelectionBox::IRCNetworkSelectionBox(QWidget* parent)
: QDialog(parent)
{
	setupUi(this);

	connect(btnNewNetwork, SIGNAL( clicked() ), SLOT( btnNewNetworkClicked() ) );
	connect(cboNetwork, SIGNAL( currentIndexChanged(int) ), SLOT( networkChanged(int) ) );

	initWidgets();
}

void IRCNetworkSelectionBox::accept()
{
	if (validate())
	{
		QDialog::accept();
	}
}

void IRCNetworkSelectionBox::addNetworkToComboBox(const IRCNetworkEntity& network)
{
	cboNetwork->addItem(buildTitle(network), network.serializeQVariant());
}

QString IRCNetworkSelectionBox::buildTitle(const IRCNetworkEntity &network) const
{
	return QString("%1 [%2:%3]").arg(network.description()).arg(network.address()).arg(network.port());
}

void IRCNetworkSelectionBox::btnNewNetworkClicked()
{
	CFGIRCDefineNetworkDialog dialog(this);
	if (dialog.exec() == QDialog::Accepted)
	{
		ChatNetworksCfg cfg;
		QList<IRCNetworkEntity> networks = cfg.networks();
		networks << dialog.getNetworkEntity();
		cfg.setNetworks(networks);

		fetchNetworks();
	}
}

void IRCNetworkSelectionBox::editCurrentNetwork()
{
	IRCNetworkEntity network = networkCurrent();
	if (!network.isValid())
	{
		QMessageBox::critical(this, tr("Doomseeker - edit network"),
			tr("Cannot edit as no valid network is selected."));
		return;
	}
	CFGIRCDefineNetworkDialog dialog(network, this);
	if (dialog.exec() == QDialog::Accepted)
	{
		IRCNetworkEntity editedNetwork = dialog.getNetworkEntity();
		if (replaceNetworkInConfig(network, editedNetwork))
		{
			updateCurrentNetwork(editedNetwork);
		}
	}
}

void IRCNetworkSelectionBox::fetchNetworks()
{
	ChatNetworksCfg cfg;
	QList<IRCNetworkEntity> networks = cfg.networks();
	qSort(networks);
	cboNetwork->clear();

	foreach (const IRCNetworkEntity& network, networks)
	{
		addNetworkToComboBox(network);
	}

	IRCNetworkEntity lastUsedNetwork = cfg.lastUsedNetwork();
	if (lastUsedNetwork.isValid())
	{
		setNetworkMatchingDescriptionAsCurrent(lastUsedNetwork.description());
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
	IRCNetworkEntity networkEntity = networkCurrent();
	networkEntity.setPassword(lePassword->text());
	return networkEntity;
}

void IRCNetworkSelectionBox::networkChanged(int index)
{
	if (index >= 0)
	{
		updateNetworkInfo();
	}
}

IRCNetworkEntity IRCNetworkSelectionBox::networkCurrent() const
{
	return networkAtRow(cboNetwork->currentIndex());
}

IRCNetworkEntity IRCNetworkSelectionBox::networkAtRow(int row) const
{
	if (row < 0 || row >= cboNetwork->count())
	{
		return IRCNetworkEntity();
	}
	return IRCNetworkEntity::deserializeQVariant(cboNetwork->itemData(row));
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

void IRCNetworkSelectionBox::setNetworkMatchingDescriptionAsCurrent(const QString &description)
{
	for (int row = 0; row < cboNetwork->count(); ++row)
	{
		IRCNetworkEntity candidate = networkAtRow(row);
		if (candidate.description() == description)
		{
			cboNetwork->setCurrentIndex(row);
			break;
		}
	}
}

void IRCNetworkSelectionBox::updateCurrentNetwork(const IRCNetworkEntity &network)
{
	cboNetwork->setItemText(cboNetwork->currentIndex(), buildTitle(network));
	cboNetwork->setItemData(cboNetwork->currentIndex(), network.serializeQVariant());
	updateNetworkInfo();
}

bool IRCNetworkSelectionBox::replaceNetworkInConfig(const IRCNetworkEntity &oldNetwork, const IRCNetworkEntity &newNetwork)
{
	ChatNetworksCfg cfg;
	return cfg.replaceNetwork(oldNetwork.description(), newNetwork, this);
}

void IRCNetworkSelectionBox::updateNetworkInfo()
{
	IRCNetworkEntity network = networkCurrent();

	leServerAddress->setText(network.address());
	spinPort->setValue(network.port());
	lePassword->setText(network.password());
}

bool IRCNetworkSelectionBox::validate()
{
	const static QString ERROR_TITLE = tr("IRC connection error");
	IRCNetworkConnectionInfo connectionInfo = this->networkConnectionInfo();

	if (connectionInfo.nick.isEmpty())
	{
		QMessageBox::warning(NULL, ERROR_TITLE, tr("You must specify a nick."));
		return false;
	}

	if (connectionInfo.networkEntity.address().isEmpty())
	{
		QMessageBox::warning(NULL, ERROR_TITLE, tr("You must specify a network address."));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
// cfgircdefinenetworkdialog.cpp
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
#include "cfgircdefinenetworkdialog.h"

CFGIRCDefineNetworkDialog::CFGIRCDefineNetworkDialog(const IRCNetworkEntity& initValuesEntity, QWidget* parent)
: QDialog(parent)
{
	construct();

	initFrom(initValuesEntity);
}

CFGIRCDefineNetworkDialog::CFGIRCDefineNetworkDialog(QWidget* parent)
: QDialog(parent)
{
	construct();
}

void CFGIRCDefineNetworkDialog::buttonClicked(QAbstractButton* button)
{
	QPushButton* pButton = (QPushButton*)button;
	if (pButton == buttonBox->button(QDialogButtonBox::Ok))
	{
		this->accept();
	}
	else
	{
		this->reject();
	}	
}

void CFGIRCDefineNetworkDialog::construct()
{
	setupUi(this);
	
	connect(buttonBox, SIGNAL( clicked(QAbstractButton*) ), SLOT( buttonClicked(QAbstractButton*) ) );
}
		
IRCNetworkEntity CFGIRCDefineNetworkDialog::getNetworkEntity() const
{
	IRCNetworkEntity entity;
	
	QString autojoinChannels = this->teAutojoinChannels->toPlainText();
	autojoinChannels.remove('\r').replace('\n', ' ');
	
	entity.address = this->leAddress->text().trimmed();
	entity.autojoinChannels = autojoinChannels.split(" ", QString::SkipEmptyParts);
	entity.description = this->leDescription->text().trimmed();
	entity.nickservCommand = this->leNickservCommand->text().trimmed();
	entity.nickservPassword = this->leNickservPassword->text();
	entity.password = this->leServerPassword->text();
	entity.port = this->spinPort->value();
	
	return entity;
}

void CFGIRCDefineNetworkDialog::initFrom(const IRCNetworkEntity& networkEntity)
{
	this->leAddress->setText(networkEntity.address);
	this->teAutojoinChannels->setPlainText(networkEntity.autojoinChannels.join(" "));
	this->leDescription->setText(networkEntity.description);
	this->leNickservCommand->setText(networkEntity.nickservCommand);
	this->leNickservPassword->setText(networkEntity.nickservPassword);
	this->leServerPassword->setText(networkEntity.password);
	this->spinPort->setValue(networkEntity.port);
}

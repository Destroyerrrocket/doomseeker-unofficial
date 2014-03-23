//------------------------------------------------------------------------------
// cfgircappearance.cpp
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
#include "cfgircappearance.h"
#include "irc/configuration/ircconfig.h"

CFGIRCAppearance::CFGIRCAppearance(QWidget* parent)
: ConfigurationBaseBox(parent)
{
	setupUi(this);
}

void CFGIRCAppearance::readSettings()
{
	// Colors
	this->btnBackgroundColor->setColorHtml(gIRCConfig.appearance.backgroundColor);
	this->btnChannelActionColor->setColorHtml(gIRCConfig.appearance.channelActionColor);
	this->btnCtcpColor->setColorHtml(gIRCConfig.appearance.ctcpColor);
	this->btnDefaultTextColor->setColorHtml(gIRCConfig.appearance.defaultTextColor);
	this->btnErrorColor->setColorHtml(gIRCConfig.appearance.errorColor);
	this->btnNetworkActionColor->setColorHtml(gIRCConfig.appearance.networkActionColor);
	this->btnUrlColor->setColorHtml(gIRCConfig.appearance.urlColor);
	this->btnUserListSelectedTextColor->setColorHtml(gIRCConfig.appearance.userListSelectedTextColor);
	this->btnUserListSelectedBackgroundColor->setColorHtml(gIRCConfig.appearance.userListSelectedBackgroundColor);

	// Fonts
	this->btnMainFont->setSelectedFont(gIRCConfig.appearance.mainFont);
	this->btnUserListFont->setSelectedFont(gIRCConfig.appearance.userListFont);
	
	// Others
	this->cbTimestamps->setChecked(gIRCConfig.appearance.timestamps);
}
		
void CFGIRCAppearance::saveSettings()
{
	// Colors
	gIRCConfig.appearance.backgroundColor = this->btnBackgroundColor->colorHtml();
	gIRCConfig.appearance.channelActionColor = this->btnChannelActionColor->colorHtml();
	gIRCConfig.appearance.ctcpColor = this->btnCtcpColor->colorHtml();
	gIRCConfig.appearance.defaultTextColor = this->btnDefaultTextColor->colorHtml();
	gIRCConfig.appearance.errorColor = this->btnErrorColor->colorHtml();
	gIRCConfig.appearance.networkActionColor = this->btnNetworkActionColor->colorHtml();
	gIRCConfig.appearance.urlColor = this->btnUrlColor->colorHtml();
	gIRCConfig.appearance.userListSelectedTextColor = this->btnUserListSelectedTextColor->colorHtml();
	gIRCConfig.appearance.userListSelectedBackgroundColor = this->btnUserListSelectedBackgroundColor->colorHtml();

	// Fonts
	gIRCConfig.appearance.mainFont = this->btnMainFont->selectedFont();
	gIRCConfig.appearance.userListFont = this->btnUserListFont->selectedFont();
	
	// Others
	gIRCConfig.appearance.timestamps = this->cbTimestamps->isChecked();
}



//------------------------------------------------------------------------------
// wadseekerconfigappearance.cpp
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
#include "wadseekerconfigappearance.h"

WadseekerAppearanceConfigBox::WadseekerAppearanceConfigBox(IniSection& cfg, QWidget* parent) 
: ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);
}

ConfigurationBoxInfo* WadseekerAppearanceConfigBox::createStructure(IniSection& cfg, QWidget* parent)
{
	ConfigurationBoxInfo* cfgBoxInfo = new ConfigurationBoxInfo();
	cfgBoxInfo->confBox = new WadseekerAppearanceConfigBox(cfg, parent);
	cfgBoxInfo->boxName = tr("Appearance");
	cfgBoxInfo->icon = QIcon(":/icons/color-fill.png");
	return cfgBoxInfo;
}

void WadseekerAppearanceConfigBox::readSettings()
{
	btnNoticeMessageColor->setColorHtml(config["ColorMessageNotice"]);
	btnErrorMessageColor->setColorHtml(config["ColorMessageError"]);
	btnCriticalErrorMessageColor->setColorHtml(config["ColorMessageCriticalError"]);
}

void WadseekerAppearanceConfigBox::saveSettings()
{
	config["ColorMessageNotice"] = btnNoticeMessageColor->colorHtml();
	config["ColorMessageError"] = btnErrorMessageColor->colorHtml();
	config["ColorMessageCriticalError"] = btnCriticalErrorMessageColor->colorHtml();
}

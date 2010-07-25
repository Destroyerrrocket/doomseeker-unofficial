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

WadseekerAppearanceConfigBox::WadseekerAppearanceConfigBox(Config* cfg, QWidget* parent) 
: ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);
}

ConfigurationBoxInfo* WadseekerAppearanceConfigBox::createStructure(Config* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* cfgBoxInfo = new ConfigurationBoxInfo();
	cfgBoxInfo->confBox = new WadseekerAppearanceConfigBox(cfg, parent);
	cfgBoxInfo->boxName = tr("Appearance");
	cfgBoxInfo->icon = QIcon(":/icons/color-fill.png");
	return cfgBoxInfo;
}

void WadseekerAppearanceConfigBox::readSettings()
{
	SettingsData* setting;

	setting = config->setting("WadseekerColorMessageNotice");
	btnNoticeMessageColor->setColorHtml(setting->string());
	
	setting = config->setting("WadseekerColorMessageError");
	btnErrorMessageColor->setColorHtml(setting->string());
	
	setting = config->setting("WadseekerColorMessageCriticalError");
	btnCriticalErrorMessageColor->setColorHtml(setting->string());
}

void WadseekerAppearanceConfigBox::saveSettings()
{
	SettingsData* setting;

	setting = config->setting("WadseekerColorMessageNotice");
	setting->setValue(btnNoticeMessageColor->colorHtml());
	
	setting = config->setting("WadseekerColorMessageError");
	setting->setValue(btnErrorMessageColor->colorHtml());
	
	setting = config->setting("WadseekerColorMessageCriticalError");
	setting->setValue(btnCriticalErrorMessageColor->colorHtml());
}

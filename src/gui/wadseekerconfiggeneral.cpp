//------------------------------------------------------------------------------
// wadseekerconfiggeneral.cpp
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

#include "gui/wadseekerconfiggeneral.h"
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
#include <QMessageBox>

WadseekerGeneralConfigBox::WadseekerGeneralConfigBox(Config* cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	cbTargetDirectory->setCompleter(new QCompleter(new QDirModel()));
}

ConfigurationBoxInfo* WadseekerGeneralConfigBox::createStructure(Config* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* cfgBoxInfo = new ConfigurationBoxInfo();
	cfgBoxInfo->confBox = new WadseekerGeneralConfigBox(cfg, parent);
	cfgBoxInfo->boxName = tr("General");
	return cfgBoxInfo;
}

void WadseekerGeneralConfigBox::fillTargetDirectoryComboBox()
{
	SettingsData* setting = config->setting("WadPaths");
	cbTargetDirectory->clear();
	cbTargetDirectory->addItems(setting->string().split(";", QString::SkipEmptyParts));
}

void WadseekerGeneralConfigBox::readSettings()
{
	SettingsData* setting;

	fillTargetDirectoryComboBox();

	setting = config->setting("WadseekerTargetDirectory");
	cbTargetDirectory->setEditText(setting->string());

	setting = config->setting("WadseekerConnectTimeoutSeconds");
	spinConnectTimeout->setValue(setting->integer());

	setting = config->setting("WadseekerDownloadTimeoutSeconds");
	spinDownloadTimeout->setValue(setting->integer());
}

void WadseekerGeneralConfigBox::saveSettings()
{
	SettingsData* setting;

	setting = config->setting("WadseekerTargetDirectory");
	setting->setValue(cbTargetDirectory->currentText());
	QFileInfo targetDirectoryInfo(cbTargetDirectory->currentText());
	if(!targetDirectoryInfo.isWritable())
	{
		QMessageBox::warning(this, tr("Unwritable Target"), tr("The target directory you selected for Wadseeker can not be written to."));
	}

	setting = config->setting("WadseekerConnectTimeoutSeconds");
	setting->setValue(spinConnectTimeout->value());

	setting = config->setting("WadseekerDownloadTimeoutSeconds");
	setting->setValue(spinDownloadTimeout->value());
}

//------------------------------------------------------------------------------
// wadseekerconfig.cpp
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include "gui/wadseekerconfig.h"
#include <QFileDialog>

WadseekerConfigBox::WadseekerConfigBox(Config* cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	connect(btnBrowseTargetDirectory, SIGNAL( clicked() ), this, SLOT( btnBrowseTargetDirectoryClicked() ) );
}

ConfigurationBoxInfo* WadseekerConfigBox::createStructure(Config* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* ec = new ConfigurationBoxInfo();
	ec->confBox = new WadseekerConfigBox(cfg, parent);
	ec->boxName = tr("Wadseeker");
	return ec;
}

void WadseekerConfigBox::readSettings()
{
	SettingsData* setting;

	setting = config->setting("WadseekerTargetDirectory");
	leTargetDirectory->setText(setting->string());
}

void WadseekerConfigBox::saveSettings()
{
	SettingsData* setting;

	setting = config->setting("WadseekerTargetDirectory");
	setting->setValue(leTargetDirectory->text());
}

void WadseekerConfigBox::btnBrowseTargetDirectoryClicked()
{
	QString filter;
	QString strFilepath = QFileDialog::getExistingDirectory(this, tr("Wadseeker target directory"));

	if(!strFilepath.isEmpty()) // don't update if nothing was selected.
		leTargetDirectory->setText(strFilepath);
}


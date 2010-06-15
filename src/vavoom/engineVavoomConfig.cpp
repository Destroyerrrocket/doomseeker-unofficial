//------------------------------------------------------------------------------
// engineVavoomConfig.cpp
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

#include "vavoom/engineVavoomConfig.h"
#include <QFileDialog>

const // clear warnings
#include "vavoom/vavoom.xpm"

EngineVavoomConfigBox::EngineVavoomConfigBox(Config* cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	connect(btnBrowseClientBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseClientBinaryClicked() ));
	connect(btnBrowseServerBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseServerBinaryClicked() ));
}

void EngineVavoomConfigBox::btnBrowseClientBinaryClicked()
{
	QString filter;
#if defined(Q_OS_WIN32)
	filter = tr("Binary files (*.exe);;Any files (*)");
#else
	// Other platforms do not have an extension for their binary files.
	filter = tr("Any files(*)");
#endif
	QString strFilepath = QFileDialog::getOpenFileName(this, tr("Doomseeker - choose Odamex client binary"), QString(), filter);
	if(!strFilepath.isEmpty()) // don't update if nothing was selected.
		leClientBinaryPath->setText(strFilepath);
}

void EngineVavoomConfigBox::btnBrowseServerBinaryClicked()
{
	QString filter;
#if defined(Q_OS_WIN32)
	filter = tr("Binary files (*.exe);;Any files (*)");
#else
	// Other platforms do not have an extension for their binary files.
	filter = tr("Any files(*)");
#endif
	QString strFilepath = QFileDialog::getOpenFileName(this, tr("Doomseeker - choose Odamex server binary"), QString(), filter);
	if(!strFilepath.isEmpty()) // don't update if nothing was selected.
		leServerBinaryPath->setText(strFilepath);
}

ConfigurationBoxInfo* EngineVavoomConfigBox::createStructure(Config* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* pConfigurationBoxInfo = new ConfigurationBoxInfo();
	pConfigurationBoxInfo->boxName = tr("Vavoom");
	pConfigurationBoxInfo->confBox = new EngineVavoomConfigBox(cfg, parent);
	pConfigurationBoxInfo->icon = QPixmap(vavoom_xpm);
	return pConfigurationBoxInfo;
}

void EngineVavoomConfigBox::readSettings()
{
	QString str;
	SettingsData* setting;

	setting = config->setting("VavoomBinaryPath");
	leClientBinaryPath->setText(setting->string());

	setting = config->setting("VavoomCustomParameters");
	leCustomParameters->setText(setting->string());

	setting = config->setting("VavoomMasterserver");
	leMasterserverAddress->setText(setting->string());

	setting = config->setting("VavoomServerBinaryPath");
	leServerBinaryPath->setText(setting->string());
}

void EngineVavoomConfigBox::saveSettings()
{
	QString strVal;
	SettingsData* setting;

	strVal = leClientBinaryPath->text();
	setting = config->setting("VavoomBinaryPath");
	setting->setValue(strVal);

	strVal = leCustomParameters->text();
	setting = config->setting("VavoomCustomParameters");
	setting->setValue(strVal);

	strVal = leMasterserverAddress->text();
	setting = config->setting("VavoomMasterserver");
	setting->setValue(strVal);

	strVal = leServerBinaryPath->text();
	setting = config->setting("VavoomServerBinaryPath");
	setting->setValue(strVal);
}

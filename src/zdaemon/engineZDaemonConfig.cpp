//------------------------------------------------------------------------------
// engineZDaemonConfig.cpp
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "zdaemon/engineZDaemonConfig.h"
#include <QFileDialog>

const // clear warnings
#include "zdaemon/zdaemon.xpm"

EngineZDaemonConfigBox::EngineZDaemonConfigBox(Config* cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	connect(btnBrowseClientBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseClientBinaryClicked() ));
	connect(btnBrowseServerBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseServerBinaryClicked() ));
}

void EngineZDaemonConfigBox::btnBrowseClientBinaryClicked()
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

void EngineZDaemonConfigBox::btnBrowseServerBinaryClicked()
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

ConfigurationBoxInfo* EngineZDaemonConfigBox::createStructure(Config* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* pConfigurationBoxInfo = new ConfigurationBoxInfo();
	pConfigurationBoxInfo->boxName = tr("ZDaemon");
	pConfigurationBoxInfo->confBox = new EngineZDaemonConfigBox(cfg, parent);
	pConfigurationBoxInfo->icon = QPixmap(zdaemon_xpm);
	return pConfigurationBoxInfo;
}

void EngineZDaemonConfigBox::readSettings()
{
	QString str;
	SettingsData* setting;

	setting = config->setting("ZDaemonBinaryPath");
	leClientBinaryPath->setText(setting->string());

	setting = config->setting("ZDaemonCustomParameters");
	leCustomParameters->setText(setting->string());

	setting = config->setting("ZDaemonMasterserver");
	leMasterserverAddress->setText(setting->string());

	setting = config->setting("ZDaemonServerBinaryPath");
	leServerBinaryPath->setText(setting->string());
}

void EngineZDaemonConfigBox::saveSettings()
{
	QString strVal;
	SettingsData* setting;

	strVal = leClientBinaryPath->text();
	setting = config->setting("ZDaemonBinaryPath");
	setting->setValue(strVal);

	strVal = leCustomParameters->text();
	setting = config->setting("ZDaemonCustomParameters");
	setting->setValue(strVal);

	strVal = leMasterserverAddress->text();
	setting = config->setting("ZDaemonMasterserver");
	setting->setValue(strVal);

	strVal = leServerBinaryPath->text();
	setting = config->setting("ZDaemonServerBinaryPath");
	setting->setValue(strVal);
}

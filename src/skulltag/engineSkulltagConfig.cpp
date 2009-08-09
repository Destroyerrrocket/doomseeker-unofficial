//------------------------------------------------------------------------------
// engineSkulltagConfig.cpp
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

#include "skulltag/engineSkulltagConfig.h"
#include <QFileDialog>

const // clear warnings
#include "skulltag/skulltag.xpm"

EngineSkulltagConfigBox::EngineSkulltagConfigBox(Config* cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	#ifdef Q_OS_WIN32
		lblClientBinary->setText(tr("Path to executable:"));
		frameServerBinary->hide();
	#endif

	connect(btnBrowseClientBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseClientBinaryClicked() ));
	connect(btnBrowseServerBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseServerBinaryClicked() ));
	connect(btnBrowseTestingPath, SIGNAL( clicked() ), this, SLOT ( btnBrowseTestingPathClicked() ));
}

void EngineSkulltagConfigBox::btnBrowseClientBinaryClicked()
{
	QString filter;
#if defined(Q_OS_WIN32)
	filter = tr("Binary files (*.exe);;Any files (*)");
#else
	// Other platforms do not have an extension for their binary files.
	filter = tr("Any files(*)");
#endif
	QString strFilepath = QFileDialog::getOpenFileName(this, tr("Doomseeker - choose Skulltag binary"), QString(), filter);
	if(!strFilepath.isEmpty()) // don't update if nothing was selected.
		leClientBinaryPath->setText(strFilepath);
}

void EngineSkulltagConfigBox::btnBrowseServerBinaryClicked()
{
	QString filter;
	filter = tr("Any files(*)");

	QString strFilepath = QFileDialog::getOpenFileName(this, tr("Doomseeker - choose Skulltag server binary"), QString(), filter);
	if(!strFilepath.isEmpty()) // don't update if nothing was selected.
		leServerBinaryPath->setText(strFilepath);
}

void EngineSkulltagConfigBox::btnBrowseTestingPathClicked()
{
	QString strDirpath = QFileDialog::getExistingDirectory(this, tr("Doomseeker - choose Skulltag testing directory"));
	if (!strDirpath.isEmpty())
		leTestingPath->setText(strDirpath);
}

ConfigurationBoxInfo* EngineSkulltagConfigBox::createStructure(Config* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* ec = new ConfigurationBoxInfo();
	ec->boxName = tr("Skulltag");
	ec->confBox = new EngineSkulltagConfigBox(cfg, parent);
	ec->icon = QPixmap(skulltag_xpm);
	return ec;
}

void EngineSkulltagConfigBox::readSettings()
{
	QString str;
	SettingsData* setting;

	setting = config->setting("SkulltagBinaryPath");
	leClientBinaryPath->setText(setting->string());

	setting = config->setting("SkulltagServerBinaryPath");
	leServerBinaryPath->setText(setting->string());

	setting = config->setting("SkulltagTestingPath");
	leTestingPath->setText(setting->string());

	setting = config->setting("SkulltagCustomParameters");
	leCustomParameters->setText(setting->string());

	setting = config->setting("SkulltagMasterserver");
	leMasterserverAddress->setText(setting->string());
}

void EngineSkulltagConfigBox::saveSettings()
{
	QString strVal;
	SettingsData* setting;

	strVal = leClientBinaryPath->text();
	setting = config->setting("SkulltagBinaryPath");
	setting->setValue(strVal);

	strVal = leServerBinaryPath->text();
	setting = config->setting("SkulltagServerBinaryPath");
	setting->setValue(strVal);

	strVal = leTestingPath->text();
	setting = config->setting("SkulltagTestingPath");
	setting->setValue(strVal);

	strVal = leCustomParameters->text();
	setting = config->setting("SkulltagCustomParameters");
	setting->setValue(strVal);

	strVal = leMasterserverAddress->text();
	setting = config->setting("SkulltagMasterserver");
	setting->setValue(strVal);
}

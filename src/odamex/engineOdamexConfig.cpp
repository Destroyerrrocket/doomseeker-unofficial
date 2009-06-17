//------------------------------------------------------------------------------
// engineOdamexConfig.cpp
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

#include "odamex/engineOdamexConfig.h"
#include <QFileDialog>

const // clear warnings
#include "odamex/odamex.xpm"

EngineOdamexConfigBox::EngineOdamexConfigBox(Config* cfg, QWidget* parent) : ConfigurationBaseBox(cfg, parent)
{
	setupUi(this);

	connect(btnBrowseBinary, SIGNAL( clicked() ), this, SLOT ( btnBrowseBinaryClicked() ));
}
///////////////////////////////////////////////////
ConfigurationBoxInfo* EngineOdamexConfigBox::createStructure(Config* cfg, QWidget* parent)
{
	ConfigurationBoxInfo* ec = new ConfigurationBoxInfo();
	ec->boxName = tr("Odamex");
	ec->confBox = new EngineOdamexConfigBox(cfg, parent);
	ec->icon = QPixmap(odamex_xpm);
	return ec;
}
///////////////////////////////////////////////////
void EngineOdamexConfigBox::readSettings()
{
	QString str;
	SettingsData* setting;

	setting = config->setting("OdamexBinaryPath");
	leBinaryPath->setText(setting->string());

	setting = config->setting("OdamexCustomParameters");
	leCustomParameters->setText(setting->string());

	setting = config->setting("OdamexMasterserver");
	leMasterserverAddress->setText(setting->string());
}

void EngineOdamexConfigBox::saveSettings()
{
	QString strVal;
	SettingsData* setting;

	strVal = leBinaryPath->text();
	setting = config->setting("OdamexBinaryPath");
	setting->setValue(strVal);

	strVal = leCustomParameters->text();
	setting = config->setting("OdamexCustomParameters");
	setting->setValue(strVal);

	strVal = leMasterserverAddress->text();
	setting = config->setting("OdamexMasterserver");
	setting->setValue(strVal);
}
////////////////////////////////////////////////////
void EngineOdamexConfigBox::btnBrowseBinaryClicked()
{
	QString filter;
#if defined(Q_WS_WIN)
	filter = tr("Binary files (*.exe);;Any files (*)");
#else
	// Other platforms do not have an extension for their binary files.
	filter = tr("Any files(*)");
#endif
	QString strFilepath = QFileDialog::getOpenFileName(this, tr("Doomseeker - choose Odamex binary"), QString(), filter);
	if(!strFilepath.isEmpty()) // don't update if nothing was selected.
		leBinaryPath->setText(strFilepath);
}

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

#include "engineSkulltagConfig.h"
#include "skulltagmain.h"
#include "plugins/engineplugin.h"

#include <QFileDialog>

EngineSkulltagConfigBox::EngineSkulltagConfigBox(const EnginePlugin* plugin, IniSection& cfg, QWidget* parent) 
: EngineConfigurationBaseBox(plugin, cfg, parent)
{
	#ifdef Q_OS_WIN32
		makeClientOnly();
	#endif

	// Create the testing box, we might as well do this in code.
	groupTesting = new QGroupBox();
	groupTesting->setTitle(tr("Testing Releases"));
	groupTesting->setCheckable(true);
	groupTesting->setLayout(new QVBoxLayout());
	groupTesting->layout()->addWidget(new QLabel(tr("Directory for testing releases:")));
	addWidget(groupTesting);

	QWidget *releasePathLayout = new QWidget();
	releasePathLayout->setLayout(new QHBoxLayout());
	leTestingPath = new QLineEdit();
	btnBrowseTestingPath = new QPushButton("...");
	releasePathLayout->layout()->addWidget(leTestingPath);
	releasePathLayout->layout()->addWidget(btnBrowseTestingPath);
	groupTesting->layout()->addWidget(releasePathLayout);

	connect(btnBrowseTestingPath, SIGNAL( clicked() ), this, SLOT ( btnBrowseTestingPathClicked() ));
}

void EngineSkulltagConfigBox::btnBrowseTestingPathClicked()
{
	QString strDirpath = QFileDialog::getExistingDirectory(this, tr("Doomseeker - choose Skulltag testing directory"));
	if (!strDirpath.isEmpty())
		leTestingPath->setText(strDirpath);
}

void EngineSkulltagConfigBox::readSettings()
{
	EngineConfigurationBaseBox::readSettings();
	
	IniSection& config = *SkulltagMain::get()->data()->pConfig;

	groupTesting->setChecked(config["EnableTesting"]);
	leTestingPath->setText(config["TestingPath"]);
}

void EngineSkulltagConfigBox::saveSettings()
{
	EngineConfigurationBaseBox::saveSettings();

	QString strVal;
	
	IniSection& config = *SkulltagMain::get()->data()->pConfig;

	config["EnableTesting"] = groupTesting->isChecked();

	strVal = leTestingPath->text();
	config["TestingPath"] = strVal;
}

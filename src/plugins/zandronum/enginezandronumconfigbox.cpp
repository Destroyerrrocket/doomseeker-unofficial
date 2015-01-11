//------------------------------------------------------------------------------
// enginezandronumconfigbox.cpp
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#include "enginezandronumconfigbox.h"
#include "zandronumengineplugin.h"
#include "ini/ini.h"
#include "plugins/engineplugin.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

EngineZandronumConfigBox::EngineZandronumConfigBox(const EnginePlugin* plugin, IniSection& cfg, QWidget* parent)
: EngineConfigurationBaseBox(plugin, cfg, parent)
{
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

	// Allow clients to "ask politely" not to display the country
	// of the client. User modified servers can just ignore this and
	// display the country anyway.
	// Default: do not allow.
	cbAllowDisplayCountry = new QCheckBox(this);
	cbAllowDisplayCountry->setText(tr("Allow servers to display my country"));
	cbAllowDisplayCountry->setChecked(false);
	addWidget(cbAllowDisplayCountry);

	connect(btnBrowseTestingPath, SIGNAL( clicked() ), this, SLOT ( btnBrowseTestingPathClicked() ));
}

void EngineZandronumConfigBox::btnBrowseTestingPathClicked()
{
	QString strDirpath = QFileDialog::getExistingDirectory(this, tr("Doomseeker - choose Zandronum testing directory"));
	if (!strDirpath.isEmpty())
		leTestingPath->setText(strDirpath);
}

void EngineZandronumConfigBox::readSettings()
{
	EngineConfigurationBaseBox::readSettings();

	IniSection& config = *ZandronumEnginePlugin::staticInstance()->data()->pConfig;

	groupTesting->setChecked(config["EnableTesting"]);
	leTestingPath->setText(config["TestingPath"]);
	cbAllowDisplayCountry->setChecked(config["AllowServersToDisplayMyCountry"]);
}

void EngineZandronumConfigBox::saveSettings()
{
	EngineConfigurationBaseBox::saveSettings();

	QString strVal;

	IniSection& config = *ZandronumEnginePlugin::staticInstance()->data()->pConfig;

	config["EnableTesting"] = groupTesting->isChecked();

	strVal = leTestingPath->text();
	config["TestingPath"] = strVal;
	config["AllowServersToDisplayMyCountry"] = cbAllowDisplayCountry->isChecked();
}

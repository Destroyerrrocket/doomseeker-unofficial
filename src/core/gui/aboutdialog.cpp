//------------------------------------------------------------------------------
// aboutdialog.cpp
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#include "aboutdialog.h"
#include "main.h"
#include "plugins/engineplugin.h"
#include "wadseeker/wadseekerversioninfo.h"
#include "version.h"
#include <QPixmap>

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	connect(buttonBox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT( close() ));

	// Doomseeker
	versionChangeset->setText(Version::changeset());
	versionNumber->setText(Version::versionRevision());
	lblRevision->setText(QString::number(Version::revisionNumber()));
	logo->setPixmap(QPixmap(":/logo.png"));

	// Wadseeker
	wadseekerAuthor->setText(WadseekerVersionInfo::author());
	wadseekerDescription->setText(WadseekerVersionInfo::description());
	wadseekerVersion->setText(WadseekerVersionInfo::version());
	wadseekerYearSpan->setText(WadseekerVersionInfo::yearSpan());

	// Populate plugins dialog
	for(unsigned i = 0; i < Main::enginePlugins->numPlugins(); ++i)
	{
		pluginBox->addItem( (*Main::enginePlugins)[i]->info->data()->name);
	}
	connect(pluginBox, SIGNAL( currentIndexChanged(int) ), this, SLOT( changePlugin(int) ));
	changePlugin(0);

	adjustSize();
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::changePlugin(int pluginIndex)
{
	if(static_cast<unsigned> (pluginIndex) >= Main::enginePlugins->numPlugins())
		return; // Invalid plugin.

	const EnginePlugin* plug = (*Main::enginePlugins)[pluginIndex]->info;

	pluginAuthor->setText(plug->data()->author);
	pluginVersion->setText(QString("Version: %1").arg(plug->data()->version));
}

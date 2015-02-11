//------------------------------------------------------------------------------
// cfgautoupdates.cpp
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
#include "cfgautoupdates.h"
#include "ui_cfgautoupdates.h"

#include "configuration/doomseekerconfig.h"
#include "updater/updatechannel.h"
#include "log.h"
#include <cassert>

DClass<CFGAutoUpdates> : public Ui::CFGAutoUpdates
{
};

DPointered(CFGAutoUpdates)

CFGAutoUpdates::CFGAutoUpdates(QWidget *parent)
: ConfigurationBaseBox(parent)
{
	d->setupUi(this);
}

CFGAutoUpdates::~CFGAutoUpdates()
{
}

void CFGAutoUpdates::initUpdateChannels()
{
	QList<UpdateChannel> channels = UpdateChannel::allChannels();
	foreach (const UpdateChannel& channel, channels)
	{
		d->cboUpdateChannel->addItem(channel.translatedName(),
			channel.name());
	}
}

void CFGAutoUpdates::onUpdateChannelChange(int index)
{
	// Update description field.
	QString name = d->cboUpdateChannel->itemData(index).toString();
	UpdateChannel channel = UpdateChannel::fromName(name);
	d->pteChannelDescription->setPlainText(channel.translatedDescription());
}

void CFGAutoUpdates::readSettings()
{
	initUpdateChannels();

	switch (gConfig.autoUpdates.updateMode)
	{
		case DoomseekerConfig::AutoUpdates::UM_Disabled:
			d->rbDisabled->setChecked(true);
			break;
		default:
		case DoomseekerConfig::AutoUpdates::UM_NotifyOnly:
			d->rbNotifyButDontInstall->setChecked(true);
			break;
		case DoomseekerConfig::AutoUpdates::UM_FullAuto:
			d->rbInstallAutomatically->setChecked(true);
			break;
	}
	QString channelName = gConfig.autoUpdates.updateChannelName;
	int channelIdx = d->cboUpdateChannel->findData(channelName);
	if (channelIdx < 0)
	{
		// Default to "stable" if user tampered with the INI file.
		channelIdx = d->cboUpdateChannel->findData(UpdateChannel::mkStable().name());
	}
	d->cboUpdateChannel->setCurrentIndex(channelIdx);
}

void CFGAutoUpdates::saveSettings()
{
	if (d->rbDisabled->isChecked())
	{
		gConfig.autoUpdates.updateMode = DoomseekerConfig::AutoUpdates::UM_Disabled;
	}
	else if (d->rbNotifyButDontInstall->isChecked())
	{
		gConfig.autoUpdates.updateMode = DoomseekerConfig::AutoUpdates::UM_NotifyOnly;
	}
	else if (d->rbInstallAutomatically->isChecked())
	{
		gConfig.autoUpdates.updateMode = DoomseekerConfig::AutoUpdates::UM_FullAuto;
	}
	else
	{
		assert(false && "CFGAutoUpdates::saveSettings() - No radio button is checked.");
	}
	gConfig.autoUpdates.updateChannelName = d->cboUpdateChannel->itemData(
		d->cboUpdateChannel->currentIndex()).toString();
}

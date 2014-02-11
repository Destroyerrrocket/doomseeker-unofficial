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

#include "configuration/doomseekerconfig.h"
#include "updater/updatechannel.h"
#include "log.h"
#include <cassert>

CFGAutoUpdates::CFGAutoUpdates(QWidget *parent) 
: ConfigurationBaseBox(parent)
{
	setupUi(this);
}

void CFGAutoUpdates::initUpdateChannels()
{
	QList<UpdateChannel> channels = UpdateChannel::allChannels();
	foreach (const UpdateChannel& channel, channels)
	{
		cboUpdateChannel->addItem(channel.translatedName(),
			channel.name());
	}
}

void CFGAutoUpdates::onUpdateChannelChange(int index)
{
	// Update description field.
	QString name = cboUpdateChannel->itemData(index).toString();
	UpdateChannel channel = UpdateChannel::fromName(name);
	pteChannelDescription->setPlainText(channel.translatedDescription());
}

void CFGAutoUpdates::readSettings()
{
	initUpdateChannels();

	switch (gConfig.autoUpdates.updateMode)
	{
		case DoomseekerConfig::AutoUpdates::UM_Disabled:
			rbDisabled->setChecked(true);
			break;
		default:
		case DoomseekerConfig::AutoUpdates::UM_NotifyOnly:
			rbNotifyButDontInstall->setChecked(true);
			break;
		case DoomseekerConfig::AutoUpdates::UM_FullAuto:
			rbInstallAutomatically->setChecked(true);
			break;
	}
	QString channelName = gConfig.autoUpdates.updateChannelName;
	int channelIdx = cboUpdateChannel->findData(channelName);
	if (channelIdx < 0)
	{
		// Default to "stable" if user tampered with the INI file.
		channelIdx = cboUpdateChannel->findData(UpdateChannel::mkStable().name());
	}
	cboUpdateChannel->setCurrentIndex(channelIdx);
}

void CFGAutoUpdates::saveSettings()
{
	if (rbDisabled->isChecked())
	{
		gConfig.autoUpdates.updateMode = DoomseekerConfig::AutoUpdates::UM_Disabled;
	}
	else if (rbNotifyButDontInstall->isChecked())
	{
		gConfig.autoUpdates.updateMode = DoomseekerConfig::AutoUpdates::UM_NotifyOnly;
	}
	else if (rbInstallAutomatically->isChecked())
	{
		gConfig.autoUpdates.updateMode = DoomseekerConfig::AutoUpdates::UM_FullAuto;
	}
	else
	{
		assert(false && "CFGAutoUpdates::saveSettings() - No radio button is checked.");
	}
	gConfig.autoUpdates.updateChannelName = cboUpdateChannel->itemData(
		cboUpdateChannel->currentIndex()).toString();
}

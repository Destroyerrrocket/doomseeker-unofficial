//------------------------------------------------------------------------------
// updatechannel.h
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
#include "updatechannel.h"

#include <cassert>

const QString CHANNEL_BETA = "beta";
const QString CHANNEL_STABLE = "stable";

UpdateChannel::UpdateChannel(const QString& name)
{
	this->channelName = name;
}

UpdateChannel::UpdateChannel(const UpdateChannel& other)
{
	this->channelName = other.channelName;
}

QList<UpdateChannel> UpdateChannel::allChannels()
{
	QList<UpdateChannel> list;
	list << mkStable();
	list << mkBeta();
	return list;
}

UpdateChannel UpdateChannel::mkBeta()
{
	return UpdateChannel(CHANNEL_BETA);
}

UpdateChannel UpdateChannel::mkStable()
{
	return UpdateChannel(CHANNEL_STABLE);
}

bool UpdateChannel::operator==(const UpdateChannel& other) const
{
	return this->channelName == other.channelName;
}

bool UpdateChannel::isNull() const
{
	return this->channelName.isNull();
}

QString UpdateChannel::name() const
{
	assert(!isNull() && "UpdateChannel::name() on a null object");
	return this->channelName;
}

QString UpdateChannel::translatedDescription() const
{
	assert(!isNull() && "UpdateChannel::translatedDescription() on a null object");
	if (channelName == CHANNEL_BETA)
	{
		return UpdateChannelTr::tr(
			"Beta versions have newer features but they "
			"are untested. Releases on this update channel "
			"are more often and are suggested for users "
			"who want newest functionalities and minor bug fixes "
			"as soon as they become implemented and available."
		);
	}
	else if (channelName == CHANNEL_STABLE)
	{
		return UpdateChannelTr::tr(
			"Stable versions are released rarely. They cover "
			"many changes at once and these changes are more certain "
			"to work correctly. Critical bug fixes are also provided "
			"through this channel."
		);
	}
	else
	{
		return channelName;
	}
}

QString UpdateChannel::translatedName() const
{
	assert(!isNull() && "UpdateChannel::translatedName() on a null object");
	if (channelName == CHANNEL_BETA)
	{
		return UpdateChannelTr::tr("Beta");
	}
	else if (channelName == CHANNEL_STABLE)
	{
		return UpdateChannelTr::tr("Stable");
	}
	else
	{
		return channelName;
	}
}


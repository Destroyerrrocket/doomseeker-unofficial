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
#ifndef DOOMSEEKER_UPDATER_UPDATECHANNEL_H
#define DOOMSEEKER_UPDATER_UPDATECHANNEL_H

#include <QObject>
#include <QString>

/**
 * This class is for translation purposes so the
 * strings won't appear as assigned to QObject in the
 * Qt Linguist.
 *
 * UpdateChannel is designed to be copied and copying
 * a QObject leads to a disaster.
 */
class UpdateChannelTr : public QObject
{
	private:
		UpdateChannelTr() {}
};

class UpdateChannel
{
	public:
		static QList<UpdateChannel> allChannels();
		static UpdateChannel mkBeta();
		static UpdateChannel mkStable();

		/**
		 * @brief Creates a null object.
		 */
		UpdateChannel() {};
		UpdateChannel(const UpdateChannel& other);

		bool operator==(const UpdateChannel& other) const;
		bool operator!=(const UpdateChannel& other) const
		{
			return !(*this == other);
		}

		bool isNull() const;

		QString name() const;
		QString translatedDescription() const;
		QString translatedName() const;

	private:
		UpdateChannel(const QString& name);

		QString channelName;
};

#endif



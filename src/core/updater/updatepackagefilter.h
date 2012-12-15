//------------------------------------------------------------------------------
// updatepackagefilter.h
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
#ifndef DOOMSEEKER_UPDATER_UPDATERPACKAGEFILTER_H
#define DOOMSEEKER_UPDATER_UPDATERPACKAGEFILTER_H

#include "updater/updatechannel.h"
#include "updater/updatepackage.h"
#include <QList>
#include <QMap>
#include <QString>

/**
 * @brief Filters UpdatePackage information basing on what is requested
 *        by the program.
 *
 * These operations are performed:
 * - Packages are filtered by specified channel name: setChannel().
 * - Packages which have the same revision number as the ones already
 *   installed are discarded. This is hardcoded into the class.
 */
class UpdatePackageFilter
{
	public:
		UpdatePackageFilter();
		~UpdatePackageFilter();

		QList<UpdatePackage> filter(const QList<UpdatePackage>& packages);
		void setChannel(const UpdateChannel& channel);

	private:
		class PluginInfo;
		class PrivData;

		PrivData* d;

		QMap<QString, PluginInfo> collectPluginInfo();
};

#endif

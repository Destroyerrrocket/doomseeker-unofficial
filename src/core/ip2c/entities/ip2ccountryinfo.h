//------------------------------------------------------------------------------
// ip2ccountryinfo.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2011 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __IP2CCOUNTRYINFO_H__
#define __IP2CCOUNTRYINFO_H__

#include "global.h"

#include <QPixmap>
#include <QString>

/**
 *	@brief Flag and name of the country.
 */
class IP2CCountryInfo
{
	public:
		const QPixmap* flag;
		QString name;

		IP2CCountryInfo();
		IP2CCountryInfo(const QPixmap* flag, QString name);

		bool isValid() const;
};

#endif

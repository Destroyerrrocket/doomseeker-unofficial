//------------------------------------------------------------------------------
// ip2ccountryinfo.cpp
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
#include "ip2ccountryinfo.h"

IP2CCountryInfo::IP2CCountryInfo()
{
	flag = NULL;
}

IP2CCountryInfo::IP2CCountryInfo(const QPixmap* flag, QString name)
{
	this->flag = flag;
	this->name = name;
}

bool IP2CCountryInfo::isValid() const
{
	return flag != NULL
		&& !flag->isNull()
		&& !name.isEmpty();
}

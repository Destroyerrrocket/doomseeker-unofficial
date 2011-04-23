//------------------------------------------------------------------------------
// message.cpp
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "message.h"

QString Message::getStringBasingOnType(unsigned type)
{
	// Currently nothing to return.
	switch (type)
	{
        case Types::BANNED_FROM_MASTERSERVER:
            return QObject::tr("You have been banned from master server.");

        default:
            return QString("%1 IS NOT A VALID ERROR MESSAGE! FIX THIS!").arg(type);
	}
}

void Message::construct()
{
    qRegisterMetaType<Message>("Message");

    this->_type = Types::IGNORE_TYPE;

    // Seconds since the epoch.
    this->_timestamp = QDateTime::currentDateTime().toTime_t();
}

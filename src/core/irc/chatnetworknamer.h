//------------------------------------------------------------------------------
// chatnetworknamer.h
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef idb4ab209b_3f61_4ac4_a23f_e2fd3b519c8e
#define idb4ab209b_3f61_4ac4_a23f_e2fd3b519c8e

#include <QString>

class ChatNetworkNamer
{
public:
	static QString additionalAllowedChars();
	static QString convertToValidName(const QString &name);
	static bool isValidCharacter(const QChar &c);
	static bool isValidName(const QString &name);
};

#endif

//------------------------------------------------------------------------------
// freedoominfoparser.h
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id86e35712_abf9_449b_be61_20011facc647
#define id86e35712_abf9_449b_be61_20011facc647

#include "dptr.h"
#include <QByteArray>

class ModSet;

class FreedoomInfoParser
{
public:
	FreedoomInfoParser(const QByteArray &contents);
	~FreedoomInfoParser();

	ModSet parse();

private:
	DPtr<FreedoomInfoParser> d;
};

#endif

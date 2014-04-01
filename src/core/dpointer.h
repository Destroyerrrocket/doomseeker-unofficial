//------------------------------------------------------------------------------
// dpointer.h
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id91044119_93C0_4719_829DE83410DDE660
#define id91044119_93C0_4719_829DE83410DDE660

#define COPYABLE_D_POINTERED_DECLARE(type) \
	type(const type &other); \
	type& operator=(const type &other);

#define COPYABLE_D_POINTERED_DEFINE(type) \
	type::type(const type &other) \
	{ \
		d = new PrivData(); \
		*d = *other.d; \
	} \
	\
	type& type::operator=(const type &other) \
	{ \
		if (this != &other) \
		{ \
			*d = *other.d; \
		} \
		return *this; \
	}

#define COPYABLE_D_POINTERED_INNER_DEFINE(type, constructorname) \
	type::constructorname(const type &other) \
	{ \
		d = new PrivData(); \
		*d = *other.d; \
	} \
	\
	type& type::operator=(const type &other) \
	{ \
		if (this != &other) \
		{ \
			*d = *other.d; \
		} \
		return *this; \
	}

#endif

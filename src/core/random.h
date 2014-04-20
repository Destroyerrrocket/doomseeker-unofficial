//------------------------------------------------------------------------------
// random.h
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
#ifndef __RANDOM_H__
#define __RANDOM_H__

#include "global.h"

/**
 * @ingroup group_pluginapi
 * @brief Random values generator.
 *
 * Each call to a method that generates a value performs
 * a bIsInit == true check first. If this check fails a builtInInit() is
 * called. This ensures that Random number generator is always initialized
 * before the rand() call is made.
 *
 * Internally this uses srand() and rand() functions from C Standard Library.
 */
class MAIN_EXPORT Random
{
	public:
		/**
		 *	@brief Inits or re-inits the Random class with specified seed.
		 */
		static void init(int seed);

		/**
		 *	@brief Generates a new random unsigned short.
		 *
		 *	@param max
		 *		Maximum value for the generated number. This is exclusive,
		 *		ie. generated value is in range 0 <= x < max
		 */
		static unsigned short nextUShort(unsigned short max);

	private:
		static bool bIsInit;

		static void builtInInit();
};

#endif

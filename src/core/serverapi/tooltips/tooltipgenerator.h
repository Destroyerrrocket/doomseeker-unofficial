//------------------------------------------------------------------------------
// tooltipgenerator.h
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
#ifndef __TOOLTIP_GENERATOR_H_
#define __TOOLTIP_GENERATOR_H_

#include "serverapi/serverptr.h"
#include "global.h"
#include <QString>
#include <QObject>

/**
 * @ingroup group_pluginapi
 */
class MAIN_EXPORT TooltipGenerator : public QObject
{
	public:
		TooltipGenerator(const ServerCPtr &server);
		virtual ~TooltipGenerator();

		/**
		 * @brief DMFLAGS listing for the current server.
		 */
		virtual QString dmflagsHTML();

		/**
		 * @brief General info about current game (fraglimit, team scores, etc.)
		 */
		virtual QString gameInfoTableHTML();

		/**
		 * @brief General info about server, like server name, version,
		 *        email, etc.
		 */
		virtual QString generalInfoHTML();

		/**
		 * @brief Player table that is created when cursor
		 *        hovers over players column.
		 */
		virtual QString playerTableHTML();

	protected:
		ServerCPtr server() const;

	private:
		class PrivData;
		PrivData *d;
};

#endif

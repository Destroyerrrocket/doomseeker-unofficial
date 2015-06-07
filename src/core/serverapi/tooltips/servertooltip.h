//------------------------------------------------------------------------------
// servertooltip.h
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id86d749c6_00f5_4612_ade3_caf9613c33dd
#define id86d749c6_00f5_4612_ade3_caf9613c33dd

#include <QString>
#include "serverapi/serverptr.h"

class PWad;

namespace ServerTooltip
{
	class L10n : public QObject
	{
		Q_OBJECT;
	};

	QString createIwadToolTip(ServerPtr server);
	QString createPlayersToolTip(ServerCPtr server);
	QString createPortToolTip(ServerCPtr server);
	QString createPwadsToolTip(ServerPtr server);
	QString createPwadToolTipInfo(const PWad& pwad, const ServerPtr &server);
	QString createServerNameToolTip(ServerCPtr server);
};

#endif

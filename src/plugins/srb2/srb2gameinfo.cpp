//------------------------------------------------------------------------------
// srb2gameinfo.cpp
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
// Copyright (C) 2016 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "srb2gameinfo.h"

QList<GameMode> Srb2GameInfo::gameModes()
{
	QList<GameMode> modes;
	modes << GameMode::mkCooperative();
	modes << GameMode::ffaGame(1, tr("Comptetition"));
	modes << GameMode::ffaGame(2, tr("Race"));
	modes << GameMode::mkDeathmatch();
	modes << GameMode::mkTeamDeathmatch();
	modes << GameMode::ffaGame(5, tr("Tag"));
	modes << GameMode::ffaGame(6, tr("Hide and Seek"));
	modes << GameMode::mkCaptureTheFlag();
	return modes;
}

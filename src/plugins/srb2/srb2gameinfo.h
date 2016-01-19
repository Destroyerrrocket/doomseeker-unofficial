//------------------------------------------------------------------------------
// srb2gameinfo.h
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
#ifndef idbb44c6c8_a578_436c_adc7_dcba557f22ff
#define idbb44c6c8_a578_436c_adc7_dcba557f22ff

#include "serverapi/serverstructs.h"
#include <QObject>

class Srb2GameInfo : public QObject
{
	Q_OBJECT

public:
	static QList<GameMode> gameModes();
};

class Srb2DifficultyProvider : GameCVarProvider
{
public:
	QList<GameCVar> get(const QVariant &context)
	{
		return QList<GameCVar>();
	}
};

#endif

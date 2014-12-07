//------------------------------------------------------------------------------
// customparamspanel.h
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
#ifndef idb5bd7932_a986_44a8_8c28_935cc89c64f2
#define idb5bd7932_a986_44a8_8c28_935cc89c64f2

#include "ui_customparamspanel.h"
#include <QWidget>

class GameCreateParams;
class Ini;

class CustomParamsPanel : public QWidget, private Ui::CustomParamsPanel
{
Q_OBJECT

public:
	CustomParamsPanel(QWidget *parent);

	void fillInParams(GameCreateParams &params);
	void loadConfig(Ini &config);
	void saveConfig(Ini &config);
};

#endif

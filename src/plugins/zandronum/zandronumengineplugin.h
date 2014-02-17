//------------------------------------------------------------------------------
// zandronumengineplugin.h
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef DOOMSEEKER_PLUGIN_ZANDRONUM_ENGINEPLUGIN_H
#define DOOMSEEKER_PLUGIN_ZANDRONUM_ENGINEPLUGIN_H

#include "plugins/engineplugin.h"

class ZandronumEnginePlugin : public EnginePlugin
{
	DECLARE_PLUGIN(ZandronumEnginePlugin)
	public:
		ZandronumEnginePlugin();

		QList<CreateServerDialogPage*> createServerDialogPages(
			CreateServerDialog* pDialog);

		void setupConfig(IniSection &config) const;

		ConfigurationBaseBox *configuration(QWidget *parent) const;
		GameHost* gameHost();
		QList<GameCVar>	limits(const GameMode& gm) const;

		MasterClient				*masterClient() const;

		ServerPtr server(const QHostAddress &address, unsigned short port) const;
};

#endif

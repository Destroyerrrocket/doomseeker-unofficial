//------------------------------------------------------------------------------
// skulltaggamerunner.h
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
#ifndef __SKULLTAG_GAME_RUNNER_H_
#define __SKULLTAG_GAME_RUNNER_H_

#include "serverapi/gamerunner.h"
#include "skulltagmain.h"

class SkulltagServer;

class SkulltagGameRunner : public GameRunner
{
	public:
		SkulltagGameRunner(const SkulltagServer* server);

		const PluginInfo*			plugin() const { return SkulltagMain::get(); }
		
	protected:
		QString						argForConnectPassword() const { return "+cl_password"; }	
		QString						argForServerLaunch() const { return "-host"; }
		
		void						hostDMFlags(QStringList& args, const DMFlags& dmFlags) const;
		void						hostProperties(QStringList& args) const;
		
		
};

#endif
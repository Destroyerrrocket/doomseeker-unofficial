//------------------------------------------------------------------------------
// zandronumgamerunner.h
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
#ifndef DOOMSEEKER_PLUGIN_ZANDRONUMGAMERUNNER_H
#define DOOMSEEKER_PLUGIN_ZANDRONUMGAMERUNNER_H

#include "serverapi/gamerunner.h"
#include "zandronumengineplugin.h"

class ZandronumServer;

class ZandronumGameRunner : public GameRunner
{
	public:
		ZandronumGameRunner(const ZandronumServer* server);

		bool connectParameters(QStringList &args, PathFinder &pf, bool &iwadFound,
			const QString &connectPassword, const QString &wadTargetDirectory);

		const EnginePlugin*			plugin() const { return ZandronumEnginePlugin::staticInstance(); }

	private:
		const ZandronumServer* server;
};

#endif

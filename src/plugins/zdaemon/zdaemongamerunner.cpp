//------------------------------------------------------------------------------
// zdaemongamerunner.cpp
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#include "main.h"
#include "zdaemongamerunner.h"
#include "zdaemongameinfo.h"
#include "zdaemonmain.h"
#include "zdaemonserver.h"

ZDaemonGameRunner::ZDaemonGameRunner(const ZDaemonServer* server)
: GameRunner(server)
{
}

void ZDaemonGameRunner::hostDMFlags(QStringList& args, const DMFlags& dmFlags) const
{
	const QString argNames[] = { "+dmflags", "+dmflags2" };
	for (int i = 0; i < qMin(dmFlags.size(), 2); ++i)
	{
		unsigned flagsValue = 0;
		const DMFlagsSection* section = dmFlags[i];

		for (int j = 0; j < section->flags.count(); ++j)
		{
			flagsValue |= 1 << section->flags[j].value;
		}

		args << argNames[i] << QString::number(flagsValue);
	}
}

const EnginePlugin* ZDaemonGameRunner::plugin() const
{
	return ZDaemonMain::get();
}

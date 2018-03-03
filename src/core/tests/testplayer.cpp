//------------------------------------------------------------------------------
// testplayer.cpp
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2018 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "testplayer.h"

#include "serverapi/player.h"

static Player mkPlayer(QString name)
{
	return Player(name, 0, 0);
}

bool TestPlayerNameColorStrip::executeTest()
{
	{
		// Plain.
		Player player = mkPlayer("Simple Joe '89");
		if (player.nameColorTagsStripped() != "Simple Joe '89")
		{
			testLog << "Plain player name changed after strip: " << player.nameColorTagsStripped();
			return false;
		}
	}

	{
		// Predefined color.
		Player player = mkPlayer("[PRO] \034gHunter \034fKiller [PRO]");
		if (player.nameColorTagsStripped() != "[PRO] Hunter Killer [PRO]")
		{
			testLog << "Failed to strip predefined color codes: " << player.nameColorTagsStripped();
			return false;
		}
	}

	{
		// Predefined color at end.
		Player player = mkPlayer("Haxxor\034");
		if (player.nameColorTagsStripped() != "Haxxor")
		{
			testLog << "Failed to handle invalid color code at end: " << player.nameColorTagsStripped();
			return false;
		}
	}

	{
		// PWAD colors.
		Player player = mkPlayer("\034[e]Emmet \034[Brown]McFly\034[aqua]");
		if (player.nameColorTagsStripped() != "Emmet McFly")
		{
			testLog << "Failed to strip PWAD color codes: " << player.nameColorTagsStripped();
			return false;
		}
	}

	{
		// PWAD color unclosed.
		Player player = mkPlayer("\034[eEmmet");
		if (player.nameColorTagsStripped() != "eEmmet")
		{
			testLog << "Unclosed PWAD color mishandled: " << player.nameColorTagsStripped();
			return false;
		}
	}

	{
		// PWAD color unclosed at end.
		Player player = mkPlayer("McFly \034[");
		if (player.nameColorTagsStripped() != "McFly ")
		{
			testLog << "PWAD color unclosed at end mishandled: " << player.nameColorTagsStripped();
			return false;
		}
	}

	{
		// PWAD color empty.
		Player player = mkPlayer("Mar\034[]ty");
		if (player.nameColorTagsStripped() != "Marty")
		{
			testLog << "PWAD color empty mishandled: " << player.nameColorTagsStripped();
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
// version.cpp
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
#include "version.h"
#include "svnrevision.h"

QString Version::name()
{
	return "Doomseeker";
}

QString Version::revision()
{
	return SVN_REVISION_STRING;
}

int Version::revisionNumber()
{
	return SVN_REVISION_NUMBER;
}

QString Version::userAgent()
{
	return "Doomseeker/" + versionRevision();
}

QString Version::version()
{
	return "0.9 Beta";
}

QString Version::versionRevision()
{
	if (revision().isEmpty())
	{
		return version();
	}
	else
	{
		return version() + "-" + revision();
	}
}


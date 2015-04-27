//------------------------------------------------------------------------------
// versiondefs.h
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
#ifndef DOOMSEEKER_VERSIONDEFS_H
#define DOOMSEEKER_VERSIONDEFS_H

// This is a centralized location to store all version information. Make sure
// to update this file before making a new tag or a new release.
//
// NOTE:
// This file defines the version data but it SHOULD NOT be used to access this
// data. To prevent unnecessary recompilations of huge amount of files
// everytime the version changes, all information should be accessed
// using the class defined in 'version.h' file.
//
// This file should only by included by following files:
// - version.cpp
// - windows.rc

#include "svnrevision.h"

// Info used by version.cpp
#define VERSION_STRING "1.1~beta"

// Info used by windows.rc
#define FILEVERSION_DEF 1,1,0,SVN_REVISION_NUMBER
#define PRODUCTVERSION_DEF 1,1,0,SVN_REVISION_NUMBER
#define FILEVERSION_STR_DEF "1.1.0.0"
#define PRODUCTVERSION_STR_DEF "1.1.0.0"
#define LEGALCOPYRIGHT_DEF "The Doomseeker Team 2009 - 2015"

#endif

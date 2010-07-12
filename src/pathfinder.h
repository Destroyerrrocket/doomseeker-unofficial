//------------------------------------------------------------------------------
// pathfinder.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------

#ifndef __PATHFINDER_H_
#define __PATHFINDER_H_

#include "sdeapi/config.hpp"
#include "global.h"
#include <QStringList>

struct MAIN_EXPORT PathFinderResult
{
	QStringList		foundFiles;
	QStringList		missingFiles;
};

class MAIN_EXPORT PathFinder
{
	public:
		enum MachineType
		{
			x86,
			x64,
			Preferred
		};
	
		/**
		 *	@b Retrieves correct path to "Program Files" directory. 
		 *	Windows only.
		 *
		 *	This exploits environmental variables such as %PROGRAMFILES%,
		 *	%PROGRAMFILES(X86)% and ProgramW6432. This method is used to 
		 *	determine the correct path to the "Program Files" directory on 
		 *	Windows (XP and above). Since on *nix systems there is no equivalent
		 *	it will return an empty string.
		 */
		static QString		programFilesDirectory(MachineType machineType);
	
		/**
		 *	@b Retrieves path to the directory where user data is stored.
		 *
		 *	On Windows this is the same as the current workingDirectory.
		 *
		 *	On Unix this is ~/.doomseeker/.
		 *	If such directory doesn't exist yet it is created when this method
		 *	is called.
		 *
		 *	On MacOS this will behave the same as on Unix.
		 *
		 *	@return An absolute path to the directory on success or an empty 
		 *	string on error.
		 */
		static QString		userDataDirectory();
	
		PathFinder(Config*);

		QString 			findFile(const QString& fileName);
		PathFinderResult	findFiles(const QStringList& files);

	protected:
		Config* config;
};

#endif

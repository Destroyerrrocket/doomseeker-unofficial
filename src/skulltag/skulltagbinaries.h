//------------------------------------------------------------------------------
// skulltagbinaries.h
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
#ifndef __SKULLTAG_BINARIES_H_
#define __SKULLTAG_BINARIES_H_

#include "serverapi/binaries.h"

class SkulltagServer;

class SkulltagBinaries : public Binaries
{
	public:
		SkulltagBinaries(SkulltagServer* server);
		
		/**
		 *	If the parent server is a normal server simple path to executable 
		 *	file is returned. If this is a testing server, a shell script is 
		 *	created	if necessary and a path to this shell script s returned.
		 */
		QString				clientBinary(QString& error) const;
		QString				clientWorkingDirectory() const;
	
		QString				configKeyClientBinary() const { return "SkulltagBinaryPath"; }
		QString				configKeyServerBinary() const;
}

#endif

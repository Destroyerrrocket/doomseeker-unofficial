//------------------------------------------------------------------------------
// cmdargshelp.cpp
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
// Copyright (C) 2015 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "cmdargshelp.h"

QString CmdArgsHelp::argsHelp()
{
	QString help = "";
	help += tr(
		"--connect protocol://ip[:port]\n"
		"    Attempts to connect to the specified server.\n");
	help += tr(
		"--datadir [directory]\n"
		"    Sets an explicit search location for\n"
		"    IP2C data along with plugins.\n"
		"    Can be specified multiple times.\n");
	help += tr(
		"--help\n"
		"    Prints this list of command line arguments.\n");
	help += tr(
		"--rcon [plugin] [ip]\n"
		"    Launch the rcon client for the specified ip.\n");
	help += tr(
		"--portable\n"
		"    Starts application in portable mode.\n"
		"    In portable mode Doomseeker saves all configuration files\n"
		"    to the directory where its executable resides.\n"
		"    Normally, configuration is saved to user's home directory.\n");
	help += tr(
		"--version-json [file]\n"
		"    Prints version info on Doomseeker and all\n"
		"    plugins in JSON format to specified file,\n"
		"    then closes the program.\n");
	return help;
}

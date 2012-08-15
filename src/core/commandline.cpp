//------------------------------------------------------------------------------
// commandline.cpp
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
#include "commandline.h"

#include "strings.h"

void CommandLine::escapeArgs(QStringList& args)
{
	QStringList::iterator it;
	for (it = args.begin(); it != args.end(); ++it)
	{
		QString& str = *it;
		escapeArg(str);
	} 
}
		
#ifdef Q_OS_LINUX
void CommandLine::escapeArg(QString& arg)
{
	arg.replace('\'', "\\'");
	arg.prepend('\'');
	arg += '\'';
}
#elif defined Q_OS_WIN
void CommandLine::escapeArg(QString& arg)
{
	arg.replace('"', "\\\"");
	arg.prepend('"');
	arg += '"';
}
#else
#error No implementaton for CommandLine::escapeArg()
#endif


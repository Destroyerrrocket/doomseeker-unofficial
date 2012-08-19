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

#include "apprunner.h"
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

#if defined Q_OS_WIN
void CommandLine::escapeArg(QString& arg)
{
	// Note: this may be game specific (oh, dear...)
	arg.replace('"', "\\\"");
	arg.prepend('"');
	arg += '"';
}
#else
// Since most other operating systems are Unix like we might as well make this a default.
void CommandLine::escapeArg(QString& arg)
{
	arg.replace('\'', "'\\''"); // This does: ' -> '\''
	arg.prepend('\'');
	arg += '\'';
}
#endif

void CommandLine::escapeExecutable(QString& arg)
{
#ifdef Q_OS_MAC
	QFileInfo binary = arg;
	if(binary.isBundle())
		arg += AppRunner::findBundleBinary(binary);
#endif
	return escapeArg(arg);
}

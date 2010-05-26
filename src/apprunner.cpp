//------------------------------------------------------------------------------
// apprunner.cpp
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
#include "apprunner.h"
#include "log.h"
#include "strings.h"
#include <QProcess>

void AppRunner::cleanArguments(QStringList& args)
{
	#ifdef Q_OS_WIN32
	QStringList::iterator it;
	for (it = args.begin(); it != args.end(); ++it)
	{
		if (it->contains(" "))
		{
			Strings::trim(*it, "\"");
		}
	}
	#endif
}

MessageResult AppRunner::runExecutable(const CommandLineInfo& cmdInfo)
{
	gLog << tr("Starting (working dir %1): %2 %3").arg(cmdInfo.applicationDir.canonicalPath()).arg(cmdInfo.executable.canonicalFilePath()).arg(cmdInfo.args.join(" "));
	QStringList args = cmdInfo.args;
	cleanArguments(args);

	int result;

	#ifdef Q_WS_MAC
	if( cmdInfo.executable.isBundle() )
	{
		result = QProcess::startDetached("open", QStringList() << cmdInfo.executable.canonicalFilePath() << "--args" << args, cmdInfo.applicationDir.canonicalPath());
	}
	else
	#endif
	{
		result = QProcess::startDetached(cmdInfo.executable.canonicalFilePath(), args, cmdInfo.applicationDir.canonicalPath());
	}

	if(!result)
	{
		QString error = tr("File: %1\ncannot be run").arg(cmdInfo.executable.canonicalFilePath());
		gLog << error;
		return MessageResult(true, tr("runExecutable"), error);
	}

	return MessageResult(false, tr("runExecutable"));
}

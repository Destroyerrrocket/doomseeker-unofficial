//------------------------------------------------------------------------------
// apprunner.h
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
#ifndef __APP_RUNNER_H_
#define __APP_RUNNER_H_

#include "global.h"
#include <QDir>
#include <QFileInfo>
#include <QStringList>

class Message;

/**
 *	@brief Structure holding parameters for application launch.
 */
class CommandLineInfo
{
    public:
        QDir 			applicationDir; /// working directory
        QStringList 	args; /// launch parameters
        QFileInfo 		executable; /// path to the executable
};

class MAIN_EXPORT AppRunner : public QObject
{
	public:
		/**
		 *	On Windows this removes any wrapping " chars.
		 *
		 *	Explanation:
		 *	Draft from Qt documentation on QProcess::startDetached:
		 *	"On Windows, arguments that contain spaces are wrapped in quotes."
		 *	Thus, on Windows we must unwrap the arguments that are wrapped in
		 *	quotes because thing like +sv_hostname "Started from Doomseeker"
		 *	won't work properly and a server with empty name will be started.
		 */
		static void				cleanArguments(QStringList& args);

#ifdef Q_WS_MAC
		static QString			findBundleBinary(const QFileInfo &file);
#endif

		static Message		 	runExecutable(const CommandLineInfo& cmdInfo);
};

#endif

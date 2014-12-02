//------------------------------------------------------------------------------
// gamehost.cpp
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "gamehost.h"

#include "plugins/engineplugin.h"
#include "serverapi/gamecreateparams.h"
#include "serverapi/message.h"
#include "serverapi/serverstructs.h"
#include "apprunner.h"
#include "gamedemo.h"
#include <cassert>
#include <QFileInfo>
#include <QStringList>

#define BAIL_ON_ERROR(method) \
{ \
	method; \
	if (d->message.isError()) \
	{ \
		return; \
	} \
}

class GameHost::PrivData
{
	public:
		QString argIwadLoading;
		QString argPort;
		QString argPwadLoading;
		QString argDemoPlayback;
		QString argDemoRecord;
		QString argServerLaunch;

		CommandLineInfo* currentCmdLine;
		Message message;
		GameCreateParams params;
		EnginePlugin* plugin;

		void (GameHost::*addIwad)();
		void (GameHost::*addPwads)();
		void (GameHost::*addDMFlags)();
};

GameHost::GameHost(EnginePlugin* plugin)
{
	d = new PrivData();
	d->argIwadLoading = "-iwad";
	d->argPort = "-port";
	d->argPwadLoading = "-file";
	d->argDemoPlayback = "-playdemo";
	d->argDemoRecord = "-record";
	d->currentCmdLine = NULL;
	d->plugin = plugin;

	set_addIwad(&GameHost::addIwad_default);
	set_addPwads(&GameHost::addPwads_default);
	set_addDMFlags(&GameHost::addDMFlags_default);
}

GameHost::~GameHost()
{
	delete d;
}

POLYMORPHIC_DEFINE(void, GameHost, addIwad, (), ());
POLYMORPHIC_DEFINE(void, GameHost, addPwads, (), ());
POLYMORPHIC_DEFINE(void, GameHost, addDMFlags, (), ());

void GameHost::addCustomParameters()
{
	args().append(params().customParameters());
}

void GameHost::addDemoPlaybackIfApplicable()
{
	if (params().hostMode() == GameCreateParams::Demo)
	{
		args() << argForDemoPlayback();
		args() << params().demoPath();
	}
}

void GameHost::addDemoRecordIfApplicable()
{
	if (params().hostMode() == GameCreateParams::Offline
		&& params().demoRecord() != GameDemo::NoDemo)
	{
		args() << argForDemoRecord();
		args() << params().demoPath();
	}
}

void GameHost::addExtra()
{
}

void GameHost::addIwad_default()
{
	const QString& iwadPath = params().iwadPath();

	if (iwadPath.isEmpty())
	{
		setMessage(Message::customError(tr("Iwad is not set")));
		return;
	}

	QFileInfo fi(iwadPath);

	if (!fi.isFile())
	{
		QString error = tr("Iwad Path error:\n\"%1\" doesn't exist or is a directory!").arg(iwadPath);
		setMessage(Message::customError(error));
	}

	args() << argForIwadLoading() << iwadPath;
}

void GameHost::addPwads_default()
{
	foreach(const QString& pwad, params().pwadsPaths())
	{
		args() << argForPwadLoading();

		QFileInfo fi(pwad);
		if (!fi.isFile())
		{
			QString error = tr("Pwad path error:\n\"%1\" doesn't exist or is a directory!").arg(pwad);
			setMessage(Message::customError(error));
			return;
		}
		args() << pwad;
	}
}

const QString& GameHost::argForIwadLoading() const
{
	return d->argIwadLoading;
}

const QString& GameHost::argForPort() const
{
	return d->argPort;
}

const QString& GameHost::argForPwadLoading() const
{
	return d->argPwadLoading;
}

const QString& GameHost::argForDemoPlayback() const
{
	return d->argDemoPlayback;
}

const QString& GameHost::argForDemoRecord() const
{
	return d->argDemoRecord;
}

const QString& GameHost::argForServerLaunch() const
{
	return d->argServerLaunch;
}

QStringList &GameHost::args()
{
	return d->currentCmdLine->args;
}

void GameHost::createCommandLineArguments()
{
	BAIL_ON_ERROR(addIwad());
	BAIL_ON_ERROR(addPwads());

	// Port
	if (params().hostMode() == GameCreateParams::Host && params().port() > 0)
	{
		args() << argForPort() << QString::number(params().port());
	}

	// CVars
	const QList<GameCVar> &cvars = params().cvars();
	foreach (const GameCVar &c, cvars)
	{
		args() << QString(c.command()) << c.valueString();
	}

	if (params().hostMode() == GameCreateParams::Host)
	{
		// Some games may not offer such argument.
		if (!argForServerLaunch().isEmpty())
		{
			args() << argForServerLaunch();
		}
	}

	BAIL_ON_ERROR(addDMFlags());
	BAIL_ON_ERROR(addExtra());
	BAIL_ON_ERROR(addCustomParameters());

	addDemoPlaybackIfApplicable();
	addDemoRecordIfApplicable();
	saveDemoMetaData();
}

Message GameHost::createHostCommandLine(const GameCreateParams& params, CommandLineInfo& cmdLine)
{
	d->message = Message();
	d->currentCmdLine = &cmdLine;
	d->params = params;

	args().clear();

	setupGamePaths();
	if (d->message.isError())
	{
		return d->message;
	}

	createCommandLineArguments();
	return d->message;
}

Message GameHost::host(const GameCreateParams& params)
{
	CommandLineInfo cmdLine;

	Message message = createHostCommandLine(params, cmdLine);
	if (!message.isIgnore())
	{
		return message;
	}

#ifdef Q_OS_WIN32
	const bool WRAP_IN_SSS_CONSOLE = false;
#else
	const bool WRAP_IN_SSS_CONSOLE = params.hostMode() == GameCreateParams::Host;
#endif

	if (WRAP_IN_SSS_CONSOLE)
	{
		QIcon icon;
		if (plugin() != NULL)
		{
			icon = plugin()->icon();
		}
		AppRunner::runExecutableWrappedInStandardServerConsole(icon, cmdLine);
		return Message();
	}
	else
	{
		return AppRunner::runExecutable(cmdLine);
	}
}

const GameCreateParams& GameHost::params() const
{
	return d->params;
}

EnginePlugin* GameHost::plugin() const
{
	return d->plugin;
}

void GameHost::saveDemoMetaData()
{
	if (params().demoRecord() == GameDemo::Managed)
	{
		GameDemo::saveDemoMetaData(params().demoPath(), *plugin(),
			params().iwadName(), params().pwadsNames());
	}
}

void GameHost::setArgForIwadLoading(const QString& arg)
{
	d->argIwadLoading = arg;
}

void GameHost::setArgForPort(const QString& arg)
{
	d->argPort = arg;
}

void GameHost::setArgForPwadLoading(const QString& arg)
{
	d->argPwadLoading = arg;
}

void GameHost::setArgForDemoPlayback(const QString& arg)
{
	d->argDemoPlayback = arg;
}

void GameHost::setArgForDemoRecord(const QString& arg)
{
	d->argDemoRecord = arg;
}

void GameHost::setArgForServerLaunch(const QString& arg)
{
	d->argServerLaunch = arg;
}

void GameHost::setMessage(const Message& message)
{
	d->message = message;
}

void GameHost::setupGamePaths()
{
	QFileInfo fileInfo(params().executablePath());
	if (!fileInfo.isFile() && !fileInfo.isBundle())
	{
		QString error = tr("%1\n doesn't exist or is not a file.").arg(fileInfo.filePath());
		setMessage(Message::customError(error));
		return;
	}
	d->currentCmdLine->executable = params().executablePath();
	d->currentCmdLine->applicationDir = fileInfo.dir();
}

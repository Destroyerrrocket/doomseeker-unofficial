//------------------------------------------------------------------------------
// joincommandlinebuilder.h
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id3D55B51B_0F01_4FAB_8D44E1593B00E437
#define id3D55B51B_0F01_4FAB_8D44E1593B00E437

#include "serverapi/serverptr.h"
#include "gamedemo.h"
#include <QObject>

class CommandLineInfo;
class JoinError;
class Server;
class ServerConnectParams;
class QUrl;
class QWidget;

/**
 * @brief Generates command line for joining specified server.
 */
class JoinCommandLineBuilder : public QObject
{
	Q_OBJECT

	public:
		JoinCommandLineBuilder(ServerPtr server, GameDemo::Control demoControl,
			QWidget *parentWidget);
		~JoinCommandLineBuilder();

		const CommandLineInfo &builtCommandLine() const;
		static bool checkWadseekerValidity(QWidget *parent=NULL);
		bool isConfigurationError() const;
		const QString &error() const;
		/**
		 * @brief Runs asynchronously and emits
		 * commandLineBuildFinished() when done.
		 */
		void obtainJoinCommandLine();
		ServerPtr server() const;

	signals:
		void commandLineBuildFinished();

	private:
		enum MissingWadsProceed
		{
			Ignore,
			Cancel,
			Seeking
		};

		class PrivData;
		PrivData *d;

		QStringList allDownloadableWads(const JoinError &joinError);
		bool buildServerConnectParams(ServerConnectParams &params);
		bool checkServerStatus();
		int displayMissingWadsMessage(const QStringList &downloadableWads, const QString &message);
		void failBuild();
		void handleError(const JoinError &error);
		MissingWadsProceed handleMissingWads(const JoinError &error);
		QString mkDemoName();
		bool tryToInstallGame();

	private slots:
		void onWadseekerDone(int result);
};

#endif


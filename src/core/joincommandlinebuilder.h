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

#include "gui/missingwadsdialog.h"
#include "serverapi/serverptr.h"
#include "dptr.h"
#include <QObject>

class CommandLineInfo;
class GameDemo;
class JoinError;
class Server;
class ServerConnectParams;
class QAbstractButton;
class QUrl;
class QWidget;

/**
 * @brief Generates command line for joining specified server.
 */
class JoinCommandLineBuilder : public QObject
{
	Q_OBJECT

	public:
		JoinCommandLineBuilder(ServerPtr server, GameDemo demo, QWidget *parentWidget);
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
		/**
		 * @brief Sets the connect/ingame password and bypasses the prompt.
		 * Set passwords to a null string to unset.
		 */
		void setPasswords(const QString &connectPassword=QString(), const QString &inGamePassword=QString());

	signals:
		void commandLineBuildFinished();

	private:
		DPtr<JoinCommandLineBuilder> d;

		void allDownloadableWads(const JoinError &joinError, QStringList &required, QStringList &optional);
		bool buildServerConnectParams(ServerConnectParams &params);
		bool checkServerStatus();
		void failBuild();
		void handleError(const JoinError &error);
		MissingWadsDialog::MissingWadsProceed handleMissingWads(
			const JoinError &error);
		QString mkDemoName();
		bool tryToInstallGame();

	private slots:
		void missingWadsClicked(QAbstractButton *);
		void onWadseekerDone(int result);
};

#endif

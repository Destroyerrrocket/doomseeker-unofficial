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
		enum Demo
		{
			Managed,
			Unmanaged
		};

		JoinCommandLineBuilder(ServerPtr server, Demo demo, QWidget *parentWidget);
		~JoinCommandLineBuilder();

		static bool checkWadseekerValidity(QWidget *parent=NULL);
		const QString &error() const;
		CommandLineInfo obtainJoinCommandLine();

	private:
		enum MissingWadsProceed
		{
			Ignore,
			Cancel,
			Retry
		};

		class PrivData;
		PrivData *d;

		QStringList allDownloadableWads(const JoinError &joinError);
		bool buildServerConnectParams(ServerConnectParams &params);
		bool checkServerStatus();
		int displayMissingWadsMessage(const QStringList &downloadableWads, const QString &message);
		void handleError(const JoinError &error);
		MissingWadsProceed handleMissingWads(const JoinError &error);
		QString mkDemoName();
		void saveDemoMetaData(const QString& demoName);
};

#endif


//------------------------------------------------------------------------------
// connectionhandler.cpp
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
// Copyright (C) 2012 Braden Obrzut <admin@maniacsvault.net>
//                    "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "connectionhandler.h"

#include "joincommandlinebuilder.h"
#include "log.h"
#include "strings.h"
#include "configuration/doomseekerconfig.h"
#include "gui/configuration/doomseekerconfigurationdialog.h"
#include "plugins/engineplugin.h"
#include "plugins/pluginloader.h"
#include "refresher/refresher.h"
#include "serverapi/gameclientrunner.h"
#include "serverapi/message.h"
#include "serverapi/server.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

class ConnectionHandler::PrivData
{
	public:
		bool handleResponse;
		ServerPtr server;
		QWidget *parentWidget;
};

ConnectionHandler::ConnectionHandler(ServerPtr server, QWidget *parentWidget,
	bool handleResponse)
: QObject(parentWidget)
{
	d = new PrivData();
	d->handleResponse = handleResponse;
	d->server = server;
	d->parentWidget = parentWidget;
	connect(d->server.data(), SIGNAL(updated(ServerPtr, int)),
		this, SLOT(checkResponse(ServerPtr, int)));
}

ConnectionHandler::~ConnectionHandler()
{
	delete d;
}

void ConnectionHandler::checkResponse(const ServerPtr &server, int response)
{
	if(response != Server::RESPONSE_GOOD)
	{
		if(d->handleResponse)
		{
			switch(response)
			{
				case Server::RESPONSE_TIMEOUT:
					QMessageBox::critical(d->parentWidget, tr("Doomseeker - join server"), tr("Connection to server timed out."));
					break;
				default:
					QMessageBox::critical(d->parentWidget, tr("Doomseeker - join server"), tr("An error occured while trying to connect to server."));
					break;
			}
		}

		finish(response);
		return;
	}

	run();
}

ConnectionHandler *ConnectionHandler::connectByUrl(const QUrl &url)
{
	gLog << QString("Attempting to connect to server: %1").arg(url.toString());

	// Locate plugin by scheme
	const EnginePlugin *handler = NULL;
	// For compatibility with IDE's zds://.../<two character> scheme
	bool zdsScheme = url.scheme().compare("zds", Qt::CaseInsensitive) == 0;
	for(unsigned int i = 0;i < gPlugins->numPlugins();++i)
	{
		const EnginePlugin *plugin = gPlugins->plugin(i)->info();
		if(plugin->data()->scheme.compare(url.scheme(), Qt::CaseInsensitive) == 0 ||
			(zdsScheme && plugin->data()->scheme.left(2).compare(url.path().mid(1), Qt::CaseInsensitive) == 0))
		{
			handler = plugin;
			break;
		}
	}
	if(handler == NULL)
	{
		gLog << "Scheme not recognized starting normally.";
		return NULL;
	}

	unsigned short port = url.port(handler->data()->defaultServerPort);
	QString address;
	// We can get the port through QUrl so we'll just create a temporary variable here.
	unsigned short tmp;
	Strings::translateServerAddress(url.host(), address, tmp, QString("localhost:10666"));

	// Create the server object
	ServerPtr server = handler->server(QHostAddress(address), port);
	ConnectionHandler *connectionHandler = new ConnectionHandler(server, NULL, true);
	gRefresher->registerServer(server.data());

	return connectionHandler;
}

void ConnectionHandler::finish(int response)
{
	d->server->disconnect(this);
	emit finished(response);
}

bool ConnectionHandler::obtainJoinCommandLine(CommandLineInfo& cli,
	const QString& errorCaption, bool managedDemo, bool *hadMissing)
{
	JoinCommandLineBuilder::Demo demo = managedDemo ? JoinCommandLineBuilder::Managed : JoinCommandLineBuilder::Unmanaged;
	JoinCommandLineBuilder builder(d->server, demo, d->parentWidget);
	CommandLineInfo builtCli = builder.obtainJoinCommandLine();
	if (!builtCli.isValid())
	{
		if (!builder.error().isEmpty())
		{
			QMessageBox::critical(d->parentWidget, errorCaption, builder.error());
			if (builder.isConfigurationError())
			{
				DoomseekerConfigurationDialog::openConfiguration(d->server->plugin());
			}
		}
		return false;
	}
	cli = builtCli;
	return true;
}

void ConnectionHandler::refreshToJoin()
{
	// If the data we have is old we should refresh first to check if we can
	// still properly join the server.
	if(d->server->isRefreshable() && gConfig.doomseeker.bQueryBeforeLaunch)
	{
		gRefresher->registerServer(d->server.data());
	}
	else
	{
		checkResponse(d->server, Server::RESPONSE_GOOD);
	}
}

void ConnectionHandler::run()
{
	bool hadMissing = false;
	CommandLineInfo cli;
	if (obtainJoinCommandLine(cli, tr("Doomseeker - join server"), true, &hadMissing))
	{
		if(hadMissing)
		{
			// Downloading wads takes an unknown amount of time, a server may
			// have rotated wads, players could have joined, etc so lets refresh.
			refreshToJoin();
			return;
		}

		Message message = AppRunner::runExecutable(cli);
		if (message.isError())
		{
			gLog << tr("Error while launching executable for server \"%1\", game \"%2\": %3")
				.arg(d->server->name()).arg(d->server->engineName()).arg(message.contents());
			QMessageBox::critical(d->parentWidget, tr("Doomseeker - launch executable"), message.contents());
		}
	}

	finish(Server::RESPONSE_GOOD);
}

// -------------------------- URL Handler -------------------------------------

PluginUrlHandler *PluginUrlHandler::instance = NULL;

void PluginUrlHandler::registerAll()
{
	for(unsigned int i = 0;i < gPlugins->numPlugins();++i)
		registerScheme(gPlugins->plugin(i)->info()->data()->scheme);

	// IDE compatibility
	registerScheme("zds");
}

void PluginUrlHandler::registerScheme(const QString &scheme)
{
	if(!instance)
		instance = new PluginUrlHandler();

	QDesktopServices::setUrlHandler(scheme, instance, "handleUrl");
}

void PluginUrlHandler::handleUrl(const QUrl &url)
{
	if(QMessageBox::question(NULL, tr("Connect to server"),
		tr("Do you want to connect to the server at %1?").arg(url.toString()),
		QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
	{
		ConnectionHandler::connectByUrl(url);
	}
}

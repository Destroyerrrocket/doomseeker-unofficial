//------------------------------------------------------------------------------
// srb2server.cpp
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
// Copyright (C) 2016 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#include "srb2server.h"

#include <QBuffer>
#include <QDataStream>

#include "srb2engineplugin.h"
#include "srb2gameclientrunner.h"
#include <datastreamoperatorwrapper.h>
#include <plugins/engineplugin.h>
#include <serverapi/playerslist.h>

Srb2Server::Srb2Server(const QHostAddress &address, unsigned short port)
: Server(address, port)
{
	set_customDetails(&Srb2Server::customDetails);
	set_readRequest(&Srb2Server::readRequest);
	set_createSendRequest(&Srb2Server::createSendRequest);
}

QString Srb2Server::customDetails()
{
	return "";
}

GameClientRunner* Srb2Server::gameRunner()
{
	return new Srb2GameClientRunner(
		self().toStrongRef().staticCast<Srb2Server>());
}

EnginePlugin* Srb2Server::plugin() const
{
	return Srb2EnginePlugin::staticInstance();
}

Server::Response Srb2Server::readRequest(const QByteArray &data)
{
	return RESPONSE_GOOD;
}

QByteArray Srb2Server::createSendRequest()
{
	return QByteArray("a");
}

void Srb2Server::setGameVersion(const QString &version)
{
	Server::setGameVersion(version);
}

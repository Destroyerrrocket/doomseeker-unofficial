//------------------------------------------------------------------------------
// srb2server.h
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
#ifndef SRB2SERVER_H
#define SRB2SERVER_H

#include <QMap>
#include <serverapi/server.h>

class GameHost;
class GameClientRunner;
class EnginePlugin;

class Srb2Server : public Server
{
	Q_OBJECT

public:
	Srb2Server(const QHostAddress &address, unsigned short port);

	QString customDetails();

	GameClientRunner* gameRunner();

	EnginePlugin* plugin() const;

	void setGameVersion(const QString &version);

protected:
	Response readRequest(const QByteArray &data);
	QByteArray createSendRequest();
};

#endif

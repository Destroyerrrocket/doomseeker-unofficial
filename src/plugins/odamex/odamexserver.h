//------------------------------------------------------------------------------
// odamexserver.h
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
// Copyright (C) 2009 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------

#ifndef __ODAMEXSERVER_H__
#define __ODAMEXSERVER_H__

#include "serverapi/server.h"

#define NUM_ODAMEX_GAME_MODES 5

class Binaries;
class GameRunner;
class PluginInfo;

class OdamexServer : public Server
{
	Q_OBJECT

	public:
		static const QPixmap			*ICON;

		OdamexServer(const QHostAddress &address, unsigned short port);

		Binaries*				binaries() const;
		const QStringList&		dehs() const { return dehPatches; }
		GameRunner*				gameRunner() const;

		const QPixmap&			icon() const;

		const PluginInfo*		plugin() const;

	protected:
		Response	readRequest(QByteArray &data);
		bool		sendRequest(QByteArray &data);

		short			protocol;
		QStringList		dehPatches;
};

#endif /* __ODAMEXSERVER_H__ */

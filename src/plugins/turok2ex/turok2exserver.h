//------------------------------------------------------------------------------
// Turok2Exserver.h
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
// Copyright (C) 2017 "Edward Richardson" <Edward850@crantime.org>
//------------------------------------------------------------------------------

#ifndef __TUROK2EXSERVER_H__
#define __TUROK2EXSERVER_H__

#include <QMap>
#include <stdint.h>

#include "serverapi/server.h"

#define GENERICKEY			0x69696969
#define DOOMSEEKEY			0x70717273 // not actually unique or required by Doomseeker, can otherwise be anything other than GENERICKEY
#define DOOMSEEDTA			0x70,0x71,0x72,0x73
#define NETM_INFO2			13

class GameHost;
class GameClientRunner;
class EnginePlugin;

class Turok2ExServer : public Server
{
	Q_OBJECT

	public:
		Turok2ExServer(const QHostAddress &address, unsigned short port);

		const QStringList& dehs() const { return dehPatches; }
		GameClientRunner* gameRunner();

		EnginePlugin* plugin() const;

	protected:
		Response readRequest(const QByteArray &data);
		QByteArray createSendRequest();

	private:
		QByteArray encryptPacket(QByteArray & message, uint32_t key);
		bool decryptPacket(QByteArray & message, uint32_t key);

		QMap<QString, QString> cvars;
		short protocol;
		QStringList dehPatches;
};

#endif /* __Turok2ExSERVER_H__ */

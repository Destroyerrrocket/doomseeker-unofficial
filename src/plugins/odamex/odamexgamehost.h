//------------------------------------------------------------------------------
// odamexgamehost.h
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
#ifndef id0C2CBB3A_3E88_4063_9F380FB52C71E32A
#define id0C2CBB3A_3E88_4063_9F380FB52C71E32A

#include <serverapi/gamehost.h>

class OdamexServer;

class OdamexGameHost : public GameHost
{
	Q_OBJECT

	public:
		OdamexGameHost(const OdamexServer* server);

	protected:
		void hostProperties(QStringList& args) const;

	private:
		Q_DISABLE_COPY(OdamexGameHost)

		const OdamexServer* server;
};

#endif

//------------------------------------------------------------------------------
// skulltagserver.h
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

#ifndef __SKULLTAGSERVER_H__
#define __SKULLTAGSERVER_H__

#include "server.h"

class SkulltagServer : public Server
{
	Q_OBJECT

	public:
		enum SkulltagQueryFlags
		{
			SQF_NAME =				0x000001,
			SQF_URL =				0x000002,
			SQF_EMAIL =				0x000004,
			SQF_MAPNAME =			0x000008,
			SQF_MAXCLIENTS =		0x000010,
			SQF_MAXPLAYERS =		0x000020,
			SQF_PWADS =				0x000040,
			SQF_GAMETYPE =			0x000080,
			SQF_GAMENAME =			0x000100,
			SQF_IWAD =				0x000200,
			SQF_FORCEPASSWORD =		0x000400,
			SQF_FORCEJOINPASSWORD =	0x000800,
			SQF_GAMESKILL =			0x001000,
			SQF_BOTSKILL =			0x002000,
			SQF_DMFLAGS =			0x004000,
			SQF_LIMITS =			0x010000,
			SQF_TEAMDAMAGE =		0x020000,
			SQF_TEAMSCORES =		0x040000,
			SQF_NUMPLAYERS =		0x080000,
			SQF_PLAYERDATA =		0x100000,

			SQF_STANDARDQUERY =		SQF_NAME|SQF_URL|SQF_MAPNAME|SQF_MAXCLIENTS|SQF_MAXPLAYERS|SQF_PWADS|SQF_GAMETYPE|SQF_IWAD|SQF_FORCEPASSWORD|SQF_FORCEJOINPASSWORD|SQF_LIMITS|SQF_NUMPLAYERS|SQF_PLAYERDATA
		};

		SkulltagServer(const QHostAddress &address, unsigned short port);

	public slots:
		void	refresh();

	protected:
		QString	email;
		short	skill;
		short	botSkill;
};

#endif /* __SKULLTAGSERVER_H__ */

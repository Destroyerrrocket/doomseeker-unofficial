//------------------------------------------------------------------------------
// server.cpp
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

#include "server.h"

Player::Player(const QString &name, unsigned short score, unsigned short ping, PlayerTeam team, bool spectator, bool bot) :
	name(name), score(score), ping(ping), team(team), spectator(spectator), bot(bot)
{
}

////////////////////////////////////////////////////////////////////////////////

// I really don't know if the game modes will need to be translated but I 
// wrapped them in tr() just to be safe.
const GameMode GameMode::COOPERATIVE(QObject::tr("Cooperative"), false);
const GameMode GameMode::DEATHMATCH(QObject::tr("Deathmatch"), false);
const GameMode GameMode::TEAM_DEATHMATCH(QObject::tr("Team DM"), true);
const GameMode GameMode::CAPTURE_THE_FLAG(QObject::tr("CTF"), true);

GameMode::GameMode(const QString &name, bool teamgame) : name(name), teamgame(teamgame)
{
}

////////////////////////////////////////////////////////////////////////////////

Server::Server(const QHostAddress &address, unsigned short port) : QObject(), address(address), port(port), gameMode(GameMode::COOPERATIVE)
{
}

Server::Server(const Server &other) : QObject(), gameMode(GameMode::COOPERATIVE)
{
	(*this) = other;
}

Server::~Server()
{
}

void Server::operator= (const Server &other)
{
	address = other.address;
	port = other.port;
}

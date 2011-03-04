//------------------------------------------------------------------------------
// playerslist.h
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
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __PLAYERS_LIST_H_
#define __PLAYERS_LIST_H_

#include "global.h"
#include "player.h"
#include <QList>
#include <QMap>

class PlayersList;

#define PairPlayersByTeams int, PlayersList

/**
 *	Key - Team number.
 *	Value - List of players.
 */
typedef QMap<PairPlayersByTeams>	PlayersByTeams;

class MAIN_EXPORT PlayersList : public QList<Player>
{
	public:
		/**
		 *	@brief Lists all bots regardless if they are on a team or not.
		 */
		void				bots(PlayersList& botsList) const;
		
		/**
		 *	@brief Lists only those bots that are not on a team.
		 */
		void				botsWithoutTeam(PlayersList& botsList) const;

		int					numBots() const;
		int					numBotsOnTeam(int team) const;
		int					numBotsWithoutTeam() const;

		/**
		 *	@brief Overall number of people and bots on the server.
		 */
		int					numClients() const;
		int					numClientsWithoutBots() const;

		int					numHumansInGame() const;
		int					numHumansOnTeam(int team) const;
		int					numHumansWithoutTeam() const;
		int					numPlayersOnTeam(int team) const;
		int					numSpectators() const;

		/**
		 *	@brief Divides players and bots to lists ordered by teams.
		 *
		 *	Bots that are not assigned to teams will not be listed here. To get
		 *	them use botsWithoutTeam() method.
		 *
		 *	@param playersListMap [out] - New PlayersList objects will be stored
		 *		in this map.
		 */
		void				inGamePlayersByTeams(QMap<PairPlayersByTeams>& playersListMap) const;

		void				spectators(PlayersList& spectatorsList) const;

};

#endif

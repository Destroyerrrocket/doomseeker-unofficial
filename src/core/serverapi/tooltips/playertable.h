//------------------------------------------------------------------------------
// playertable.h
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
#ifndef __PLAYER_TABLE_H_
#define __PLAYER_TABLE_H_

#include "serverapi/playerslist.h"
#include "serverapi/serverptr.h"
#include "dptr.h"
#include <QObject>
#include <QString>

class PlayerTable : public QObject
{
	Q_OBJECT

	public:
		PlayerTable(const ServerCPtr &server);
		~PlayerTable();

		QString generateHTML();

	private:
		DPtr<PlayerTable> d;

		/**
		 *	@brief Sets numOfColumns based on pServer states.
		 */
		void setNumberOfColumns();

		/**
		 *	This will return absolutely nothing if the list in the first
		 *	argument is empty.
		 */
		QString spawnPartOfPlayerTable(const PlayersList& list, bool bAppendEmptyRowAtBeginning);
		QString spawnPlayersRows(const PlayersByTeams& playersByTeams);

		QString tableContent();
		QString tableHeader();
};

#endif

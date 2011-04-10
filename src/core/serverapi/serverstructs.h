//------------------------------------------------------------------------------
// serverstructs.h
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
// Copyright (C) 2010 "Blzut3" <admin@maniacsvault.net>
//------------------------------------------------------------------------------
#ifndef __SERVER_STRUCTS_H_
#define __SERVER_STRUCTS_H_

#include <QString>
#include <QList>

#include "global.h"

struct MAIN_EXPORT DMFlag
{
	DMFlag(QString name, unsigned value)
	: name(name), value(value)
	{
	}

	QString         name;
	unsigned    	value;
};

/**
 *	@brief Generic representation of DMFlags section.
 */
struct MAIN_EXPORT DMFlagsSection
{
	QString         name;
	QList<DMFlag>	flags;
};

/**
 *	List used by Server class' virtual method to return all flags sections.
 */
typedef QList<DMFlagsSection*> 							DMFlags;
typedef QList<DMFlagsSection*>::iterator				DMFlagsIt;
typedef QList<DMFlagsSection*>::const_iterator 			DMFlagsItConst;
typedef QList<const DMFlagsSection*> 					DMFlagsConst;
typedef QList<const DMFlagsSection*>::iterator			DMFlagsConstIt;
typedef QList<const DMFlagsSection*>::const_iterator 	DMFlagsConstItConst;

/**
 *	@brief Struct containing info about a game console variable (like fraglimit)
 */
struct MAIN_EXPORT GameCVar
{
	/**
	 *	Nice name to display in Create Server dialog.
	 */
	QString		name;

	/**
	 *	Console command used to set the given CVar.
	 */
	QString		consoleCommand;

	GameCVar() {}
	GameCVar(QString fname, QString fconsoleCommand):name(fname),consoleCommand(fconsoleCommand) {}

	void			setValue(bool b) { b == true ? val = "1" : val = "0"; }
	void			setValue(int i) { setValue(QString::number(i)); }
	void			setValue(const QString& str) { val = str; }

	const QString&	value() const { return val; }
	bool			valueBool() const { return (val.toInt() != 0); }
	bool			valueInt() const { return val.toInt(); }

	protected:
		QString		val;
};

/**
 * Data structure that holds information about a servers game mode.
 */
struct MAIN_EXPORT GameMode
{
	public:
		enum StandardGameModeIndexes
		{
			SGMICooperative		= 900,
			SGMIDeathmatch		= 901,
			SGMITeamDeathmatch	= 902,
			SGMICTF				= 903,
			SGMIUnknown			= 904
		};

		// Standard game mode set
		// These should be used in order to keep the names uniform.
		static const GameMode	COOPERATIVE;
		static const GameMode	DEATHMATCH;
		static const GameMode	TEAM_DEATHMATCH;
		static const GameMode	CAPTURE_THE_FLAG;
		static const GameMode	UNKNOWN;

		/**
		 * @param name Name to display for game mode, this should be fairly short about no longer than "cooperative".
		 */
		GameMode(int index, const QString &name, bool teamgame);

		int				modeIndex() const { return gameModeIndex; }
		const QString	&name() const { return modeName;}
		bool			isTeamGame() const { return teamgame; }
	protected:
		int		gameModeIndex;
		QString	modeName;
		bool	teamgame;
};

struct MAIN_EXPORT SkillLevel
{
	//const QString strName;

	static const int	 numSkillLevels;
	static const QString names[];
};

#endif
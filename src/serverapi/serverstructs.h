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

/**
 *	@brief Structure holding parameters for application launch.
 */
struct CommandLineInfo
{
	QDir 			applicationDir; /// working directory
	QStringList 	args; /// launch parameters
	QFileInfo 		executable; /// path to the executable
};

/**
 *	@brief Generic representation of DMFlags section.
 *
 *	Note: Maximum amount of flags in one section is 32.
 */
struct MAIN_EXPORT DMFlagsSection
{
	struct DMFlag
	{
		QString         name;
		unsigned char   value;
	};

	QString         name;
	unsigned char   size;
	DMFlag			flags[32];
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

	void			setValue(bool b) { b == true ? setValue("1") : setValue("0"); }
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
		static MAIN_EXPORT const GameMode	COOPERATIVE;
		static MAIN_EXPORT const GameMode	DEATHMATCH;
		static MAIN_EXPORT const GameMode	TEAM_DEATHMATCH;
		static MAIN_EXPORT const GameMode	CAPTURE_THE_FLAG;
		static MAIN_EXPORT const GameMode	UNKNOWN;

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

/**
 *	@brief Indicator of error for the server join process.
 *
 *	This structure contains information about whether an error occured and
 *	if it did - what type of error it is. Based on this GUI can make a
 *	decision on how to handle the error and whether to try again.
 */
struct JoinError
{
	enum JoinErrorType
	{
		NoError = 0,
		MissingWads = 1,
		Critical = 2
	};

	JoinErrorType		type;
	QString				error;

	/**
	 *	This is valid only if type == MissingWads.
	 */
	QString 			missingIwad;

	/**
	 *	This is valid only if type == MissingWads.
	 */
	QStringList 		missingWads;
};

struct MAIN_EXPORT SkillLevel
{
	//const QString strName;

	static const int	 numSkillLevels;
	static const QString names[];
};

/**
 *	@brief Host launch information for Server class.
 *
 *	Create Server dialog uses this to setup host information.
 *	However things that can be set through the Server class,
 *	like MOTD, max. clients, max. players, server name, etc. should
 *	be set through Server class' setters.
 */
struct MAIN_EXPORT HostInfo
{
	QString 		executablePath; /// if empty, serverBinary() will be used
	QString 		iwadPath;
	QStringList 	pwadsPaths;
	QStringList 	customParameters;
	DMFlags 		dmFlags;

	/**
	 *	Contents of this list will be passed as "+consoleCommand value"
	 *	to the command line.
	 */
	QList<GameCVar> cvars;

	~HostInfo()
	{
		foreach(DMFlagsSection* sec, dmFlags)
			delete sec;
	}
};

#endif

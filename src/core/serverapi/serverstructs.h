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
#include <QObject>
#include <QVariant>

#include "global.h"

/*! \file serverstructs.h */

class MAIN_EXPORT DMFlag
{
	public:
		DMFlag();
		DMFlag(QString name, unsigned value);
		COPYABLE_D_POINTERED_DECLARE(DMFlag);
		virtual ~DMFlag();

		bool isValid() const;

		const QString& name() const;
		unsigned value() const;

	private:
		class PrivData;
		PrivData* d;
};

/**
 * @brief Generic representation of DMFlags section.
 */
class MAIN_EXPORT DMFlagsSection
{
	public:
		DMFlagsSection();
		DMFlagsSection(const QString& name);
		COPYABLE_D_POINTERED_DECLARE(DMFlagsSection);
		virtual ~DMFlagsSection();

		void add(const DMFlag& flag);
		unsigned combineValues() const;
		int count() const;
		const QString &name() const;
		const DMFlag &operator[](int index) const;
		DMFlag &operator[](int index);

		DMFlagsSection& operator<<(const DMFlag& flag)
		{
			add(flag);
			return *this;
		}

	private:
		class PrivData;
		PrivData* d;
};

/**
 * @brief Struct containing info about a game variable (like fraglimit).
 */
class MAIN_EXPORT GameCVar
{
	public:
		GameCVar();
		GameCVar(const QString &name, const QString &command);
		COPYABLE_D_POINTERED_DECLARE(GameCVar);
		virtual ~GameCVar();

		/**
		 * Command used to set the given CVar.
		 */
		const QString &command() const;

		bool hasValue() const;
		bool isValid() const;

		/**
		 * Nice name to display in Create Server dialog.
		 */
		const QString &name() const;

		void setValue(const QVariant& value);

		const QVariant &value() const;
		QString valueString() const { return value().toString(); }
		bool valueBool() const { return value().toBool(); }
		int valueInt() const { return value().toInt(); }

	private:
		class PrivData;
		PrivData* d;
};

/**
 * Data structure that holds information about a servers game mode.
 */
class MAIN_EXPORT GameMode
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

		GameMode();
		/**
		 * @param name Name to display for game mode, this should be fairly short about no longer than "cooperative".
		 */
		GameMode(int index, const QString &name, bool teamgame);

		int				modeIndex() const { return gameModeIndex; }
		const QString	&name() const { return modeName;}
		bool			isTeamGame() const { return teamgame; }
		bool			isValid() const { return bIsValid; }
	protected:
		int		gameModeIndex;
		QString	modeName;
		bool	teamgame;
	private:
		bool	bIsValid;
};

// Some ports support optional wads.
class MAIN_EXPORT PWad
{
	public:
		PWad(const QString &name, bool optional=false);
		COPYABLE_D_POINTERED_DECLARE(PWad);
		virtual ~PWad();

		bool isOptional() const;
		const QString& name() const;

	private:
		class PrivData;
		PrivData* d;
};

#endif

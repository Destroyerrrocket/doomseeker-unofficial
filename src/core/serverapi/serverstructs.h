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
 * @brief Data structure that holds information about game mode.
 */
class MAIN_EXPORT GameMode
{
	public:
		enum StandardGameMode
		{
			SGM_Cooperative = 900,
			SGM_Deathmatch = 901,
			SGM_TeamDeathmatch = 902,
			SGM_CTF = 903,
			SGM_Unknown = 904
		};

		// Standard game mode set
		// These should be used in order to keep the names uniform.
		// These can't be static members as translations may not work.
		static GameMode mkCooperative();
		static GameMode mkDeathmatch();
		static GameMode mkTeamDeathmatch();
		static GameMode mkCaptureTheFlag();
		static GameMode mkUnknown();

		/**
		 * @brief Opposite of team game.
		 */
		static GameMode ffaGame(int index, const QString &name);
		/**
		 * @brief Game mode based on rivaling teams.
		 */
		static GameMode teamGame(int index, const QString &name);

		GameMode();
		COPYABLE_D_POINTERED_DECLARE(GameMode);
		virtual ~GameMode();

		int index() const;

		/**
		 * @brief Name to display for game mode.
		 *
		 * This should be fairly short about no longer than "cooperative".
		 */
		const QString &name() const;

		bool isTeamGame() const;
		bool isValid() const;

	private:
		class PrivData;
		PrivData *d;

		GameMode(int index, const QString &name);

		void setTeamGame(bool b);
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

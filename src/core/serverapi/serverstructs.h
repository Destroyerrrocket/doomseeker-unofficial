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

#include "dptr.h"
#include "global.h"

/**
 * \ingroup group_pluginapi
 * \file serverstructs.h
 */

/**
 * @ingroup group_pluginapi
 * @brief Game difficulty level definition.
 */
class MAIN_EXPORT Difficulty
{
public:
	Difficulty(const QString &name, const QVariant &data = QVariant());
	virtual ~Difficulty();

	/**
	 * @brief Arbitrary data to be used by the plugin.
	 *
	 * This can be a simple integer which denotes the skill level index.
	 * "I'm too young to die" is 0 and "NIGHTMARE!" is 4
	 */
	const QVariant &data() const;
	/**
	 * @brief Human-friendly name of the difficulty level.
	 *
	 * Suitable for translation.
	 */
	const QString &name() const;

private:
	DPtr<Difficulty> d;
};

/**
 * @brief Creates difficulty levels set.
 *
 * Override get() method and return Difficulty levels ordered from
 * easiest to hardest.
 *
 * Doomseeker will disable difficulty settings when empty list is returned.
 */
class MAIN_EXPORT DifficultyProvider : public QObject
{
	Q_OBJECT;

public:
	DifficultyProvider();
	virtual ~DifficultyProvider();

	/**
	 * @brief Default implementation returns Doom difficulty set.
	 */
	virtual QList<Difficulty> get();

private:
	Q_DISABLE_COPY(DifficultyProvider);

	DPtr<DifficultyProvider> d;
};

/**
 * @ingroup group_pluginapi
 * @brief A game setting that is a part of a group of settings
 *        that can be OR'ed logically as a single integer.
 *
 * DMFlag is basically a setting that is toggleable between enabled/disabled
 * state, and a bunch of DMFlags can be represented as bitflags that can be
 * OR'ed, XOR'ed, and AND'ed together.
 *
 * DMFlag objects can be put into DMFlagsSection collection.
 *
 * This structure is safe to copy.
 */
class MAIN_EXPORT DMFlag
{
	public:
		DMFlag();
		DMFlag(QString name, unsigned value);
		virtual ~DMFlag();

		/**
		 * @brief Valid objects have value() greater than zero.
		 *
		 * Invalid objects can be treated as 'Null'.
		 */
		bool isValid() const;

		/**
		 * @brief User-displayable name of the DMFlag, ex. "Jump is allowed".
		 */
		const QString& name() const;
		/**
		 * @brief Bits that represent this flag (usually just a single '1' bit).
		 */
		unsigned value() const;

	private:
		DPtr<DMFlag> d;
};

/**
 * @ingroup group_pluginapi
 * @brief A group of DMFlag objects that can be safely OR'ed
 *        together to form a meaningful value.
 *
 * This object is safe to copy.
 */
class MAIN_EXPORT DMFlagsSection
{
	public:
		DMFlagsSection();
		DMFlagsSection(const QString& name);
		virtual ~DMFlagsSection();

		/**
		 * @brief Append a new DMFlag to this section.
		 *
		 * Note that conflicting DMFlags will still be accepted here, as no
		 * check are performed. Objects added here affect results of
		 * combineValues().
		 */
		void add(const DMFlag& flag);
		/**
		 * @brief Logical OR of all DMFlag::value() results in this collection.
		 *
		 * Invalid DMFlag objects, if present in the collection, do not affect
		 * the output of this operation.
		 */
		unsigned combineValues() const;
		/**
		 * @brief Number of DMFlag objects inside the collection.
		 */
		int count() const;
		bool isEmpty() const;
		/**
		 * @brief User-displayable name of this section,
		 *        ex. "Compatibility flags".
		 */
		const QString &name() const;
		/**
		 * @brief Access DMFlag at specific index with '[]' operator.
		 */
		const DMFlag &operator[](int index) const;
		DMFlag &operator[](int index);

		/**
		 * @brief Stream input operator that appends DMFlag to the collection,
		 *        same as add().
		 */
		DMFlagsSection& operator<<(const DMFlag& flag)
		{
			add(flag);
			return *this;
		}

	private:
		DPtr<DMFlagsSection> d;
};

/**
 * @ingroup group_pluginapi
 * @brief A general game setting or variable (like fraglimit).
 *
 * This object is safe to copy.
 */
class MAIN_EXPORT GameCVar
{
	public:
		GameCVar();
		GameCVar(const QString &name, const QString &command);
		virtual ~GameCVar();

		/**
		 * @brief Command-line argument that sets this GameCVar.
		 *
		 * When launching a game, this command() is passed as one of the
		 * command line arguments and the value() is what follows directly
		 * after.
		 */
		const QString &command() const;

		/**
		 * @brief Is any value assigned to this GameCVar.
		 */
		bool hasValue() const;
		/**
		 * @brief 'Null' objects are invalid.
		 */
		bool isValid() const;

		/**
		 * @brief Nice name to display to user in Create Game dialog and
		 *        in other widgets.
		 */
		const QString &name() const;

		/**
		 * @brief Assign value() to this GameCVar.
		 */
		void setValue(const QVariant& value);

		/**
		 * @brief Passed as the second argument, following command().
		 */
		const QVariant &value() const;
		QString valueString() const { return value().toString(); }
		bool valueBool() const { return value().toBool(); }
		int valueInt() const { return value().toInt(); }

	private:
		DPtr<GameCVar> d;
};

/**
 * @ingroup group_pluginapi
 * @brief Game mode representation.
 *
 * The only available constructor will create an invalid object (returns false
 * on isValid()). The proper method of construction is to use provided static
 * builder methods. Either use one of 'mk' methods, that are mentioned
 * in StandardGameMode description, or build your own mode using ffaGame() or
 * teamGame() methods. index() in all cases must either be one of
 * StandardGameMode values or unique within your plugin for given game mode.
 *
 * This object is safe to copy.
 */
class MAIN_EXPORT GameMode
{
	public:
		/**
		 * @brief These game modes are so common that Doomseeker represents
		 *        them through internal values and static methods.
		 *
		 * If your plugin recognizes that the server uses one of these game
		 * modes it's much better to use these values instead of implementing
		 * your own representation.
		 *
		 * <b>More importantly</b>, static builder methods have been provided
		 * that already create appropriate GameMode objects. These methods are:
		 * mkCooperative(), mkDeathmatch(), mkTeamDeathmatch(),
		 * mkCaptureTheFlag(), and mkUnknown().
		 */
		enum StandardGameMode
		{
			SGM_Cooperative = 900,
			SGM_Deathmatch = 901,
			SGM_TeamDeathmatch = 902,
			SGM_CTF = 903,
			/**
			 * @brief Impossible to determine the game mode.
			 */
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
		 * @brief Construct a custom FFA game where players don't belong
		 *        to any teams.
		 *
		 * Cooperative game modes like "coop", "survival" or "invasion" are
		 * also considered as FFA.
		 *
		 * @param index
		 *     Value unique to given plugin but not within 900 - 1000 range.
		 * @param name
		 *     User-displayable name of the game mode.
		 *
		 * @see teamGame()
		 */
		static GameMode ffaGame(int index, const QString &name);
		/**
		 * @brief Game mode based on rivaling teams.
		 *
		 * @param index
		 *     Value unique to given plugin but not within 900 - 1000 range.
		 * @param name
		 *     User-displayable name of the game mode.
		 *
		 * @see ffaGame()
		 */
		static GameMode teamGame(int index, const QString &name);

		/**
		 * @brief Constructs an invalid GameMode object.
		 */
		GameMode();
		virtual ~GameMode();

		/**
		 * @brief Index, either a StandardGameMode or custom defined by plugin.
		 */
		int index() const;

		/**
		 * @brief User-friendly name to display for game mode.
		 *
		 * This should be fairly short about no longer than "cooperative".
		 */
		const QString &name() const;

		/**
		 * @brief Is this GameMode based on rivaling teams?
		 */
		bool isTeamGame() const;
		/**
		 * @brief 'Null' objects are invalid.
		 */
		bool isValid() const;

	private:
		DPtr<GameMode> d;

		GameMode(int index, const QString &name);

		void setTeamGame(bool b);
};

/**
 * @ingroup group_pluginapi
 * @brief PWAD hosted on a server.
 *
 * Some ports support optional wads.
 *
 * This object is safe to copy.
 */
class MAIN_EXPORT PWad
{
	public:
		PWad(const QString &name, bool optional=false);
		virtual ~PWad();

		/**
		 * @brief Is this WAD required to join the server?
		 */
		bool isOptional() const;
		/**
		 * @brief File name of the WAD.
		 */
		const QString& name() const;

	private:
		DPtr<PWad> d;
};

#endif

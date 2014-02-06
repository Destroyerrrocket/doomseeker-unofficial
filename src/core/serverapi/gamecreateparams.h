//------------------------------------------------------------------------------
// gamecreateparams.h
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
// Copyright (C) 2014 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef idF7CFB784_3591_4B2C_B5529AE7FBCC2568
#define idF7CFB784_3591_4B2C_B5529AE7FBCC2568

#include "global.h"
#include <QList>
#include <QString>
#include <QStringList>

class DMFlagsSection;
class GameCVar;

/**
 * @brief Game parametrization data used when creating new games.
 */
class MAIN_EXPORT GameCreateParams
{
	public:
		GameCreateParams();
		COPYABLE_D_POINTERED_DECLARE(GameCreateParams);
		~GameCreateParams();

		QStringList& customParameters();
		const QStringList& customParameters() const;
		/**
		 * @brief Contents of this list will be passed as "+consoleCommand value"
		 *        to the command line.
		 */
		QList<GameCVar>& cvars();
		const QList<GameCVar>& cvars() const;

		/**
		 * @brief Use if running in DEMO mode.
		 */
		const QString& demoPath() const;
		QList<DMFlagsSection> dmFlags();
		const QList<DMFlagsSection>& dmFlags() const;
		/**
		 * @brief Path to the game executable.
		 */
		const QString& executablePath() const;
		const QString& iwadPath() const;

		QStringList& pwadsPaths();
		const QStringList& pwadsPaths() const;

		void setCustomParameters(const QStringList& customParameters);
		void setCvars(const QList<GameCVar>& cvars);
		void setDemoPath(const QString& demoPath);
		void setExecutablePath(const QString& executablePath);
		void setIwadPath(const QString& iwadPath);
		void setPwadsPaths(const QStringList& pwadsPaths);

	private:
		class PrivData;
		PrivData* d;
};

#endif // header

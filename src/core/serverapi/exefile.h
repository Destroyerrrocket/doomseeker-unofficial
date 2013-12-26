//------------------------------------------------------------------------------
// exefile.h
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
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id97064B0C_7D3A_489E_B2241AD6943450FB
#define id97064B0C_7D3A_489E_B2241AD6943450FB

#include "global.h"
#include <QHash>
#include <QObject>
#include <QString>

class Message;

class MAIN_EXPORT ExeFile : public QObject
{
	Q_OBJECT

	public:
		ExeFile();
		virtual ~ExeFile();

		/**
		 * @brief INI config key for stored executable path.
		 */
		const QString& configKey() const;
		/**
		 * @brief Name of the type of the executable (server, client, etc.)
		 */
		const QString& exeTypeName() const;
		/**
		 * @brief Name of the program this executable belongs to.
		 */
		const QString& programName() const;

		/**
		 * @brief Returns the path to the executable file.
		 *
		 * @param [out] message - information message, if any.
		 *
		 * @return Empty if error is not empty.
		 */
		virtual QString pathToExe(Message& message);

		void setConfigKey(const QString& keyName);
		void setExeTypeName(const QString& name);
		void setProgramName(const QString& name);

		/**
		 * Default behavior returns directory of pathToExe(), but
		 * you can override this to provide different working directory for
		 * testing binaries.
		 *
		 * @param [out] error - type of error
		 */
		virtual QString workingDirectory(Message& message);

	private:
		class PrivData;
		PrivData* d;

		QString obtainBinary(Message& message) const;
};

#endif

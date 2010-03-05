//------------------------------------------------------------------------------
// binaries.h
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
#ifndef __BINARIES_H_
#define __BINARIES_H_

#include "global.h"
#include <QHash>
#include <QObject>
#include <QString>

class PluginInfo;

class MAIN_EXPORT Binaries : public QObject
{
	public:
		Binaries();

		/**
		 *	@brief Returns the path to the client binary.
		 *
		 *	@param [out] error - error message.
		 *	@return Empty if error is not empty.
		 */
		virtual QString					clientBinary(QString& error) const { return obtainBinary(configKeyClientBinary(), Client, error); }

		/**
		 *	Default behavior returns directory of clientBinary(), but
		 *	you can override this to provide different working directory for
		 *	Skulltag's testing binaries.
		 *	@param [out] error - type of error
		 */
		virtual QString					clientWorkingDirectory(QString& error) const;

		virtual QString					configKeyClientBinary() const = 0;
		virtual QString					configKeyOfflineBinary() const { return configKeyClientBinary(); }
		virtual QString					configKeyServerBinary() const { return configKeyClientBinary(); }

		/**
		 *	@brief Returns the path to the binary for offline play.
		 *
		 *	@param [out] error - error message.
		 *	@return Empty if error is not empty.
		 */
		virtual QString					offlineGameBinary(QString& error) const { return obtainBinary(configKeyOfflineBinary(), Offline, error); }

		/**
		 *	Returns the working directory of the binary for offline game.
		 *	@param [out] error - type of error
		 *	@return Default behavior returns offlineGameBinary() directory
		 */
		virtual QString					offlineGameWorkingDirectory(QString& error) const;

		/**
		 *	This is supposed to return the plugin this Binaries belongs to.
		 *	New instances of PluginInfo shouldn't be created here. Instead
		 *	each plugin should keep a global instance of PluginInfo (singleton?)
		 *	and a pointer to this instance should be returned.
		 */
		virtual const PluginInfo*		plugin() const = 0;

		/**
		 *	@brief Returns the path to the server binary.
		 *
		 *	@param [out] error - error message.
		 *	@return Empty if error is not empty.
		 */
		virtual QString					serverBinary(QString& error) const { return obtainBinary(configKeyServerBinary(), TServer, error); }

		/**
		 *	Default behaviour returns directory of serverBinary().
		 */
		virtual QString					serverWorkingDirectory(QString& error) const;

	protected:
		enum BinaryType
		{
			Client,
			Offline,
			TServer
		};

		/**
		 *	@brief A dictionary containing enum to string translations of words
		 *	that would fit into "No <dictionary_word> executable specified
		 *	[...]" sentence.
		 *
		 *	Initialized when Binaries is first constructed.
		 */
		typedef	QHash<BinaryType, QString>	BinaryNamesDictionary;

		static BinaryNamesDictionary 		binaryNames;

		QString								obtainBinary(const QString& configKey, BinaryType binaryType, QString& error) const;
};

#endif

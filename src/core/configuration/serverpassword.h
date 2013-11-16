//------------------------------------------------------------------------------
// serverpassword.h
//------------------------------------------------------------------------------
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA
//
//------------------------------------------------------------------------------
// Copyright (C) 2013 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef id279D1E56_A461_424E_93E3EF155BEFA077
#define id279D1E56_A461_424E_93E3EF155BEFA077

#include <QDateTime>
#include <QString>
#include <QVariant>

class ServerPassword
{
	public:
		static ServerPassword deserializeQVariant(const QVariant& map);

		/**
		 * @brief Password is valid if its phrase is not an empty string.
		 *
		 * Technically passwords full of whitespace are also valid.
		 */
		bool isValid() const
		{
			return !d.phrase.isEmpty();
		}

		const QString& lastGame() const
		{
			return d.lastGame;
		}

		const QString& lastServer() const
		{
			return d.lastServer;
		}

		const QDateTime& lastTime() const
		{
			return d.lastTime;
		}

		const QString& phrase() const
		{
			return d.phrase;
		}

		QVariant serializeQVariant() const;

		void setLastGame(const QString& v)
		{
			d.lastGame = v;
		}

		void setLastServer(const QString& v)
		{
			d.lastServer = v;
		}

		void setLastTime(const QDateTime& v)
		{
			d.lastTime = v;
		}

		void setPhrase(const QString& v)
		{
			d.phrase = v;
		}

	private:
		class PrivData
		{
		public:
			QString lastGame;
			QString lastServer;
			QDateTime lastTime;
			QString phrase;
		};
		PrivData d;
};

#endif

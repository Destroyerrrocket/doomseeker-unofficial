//------------------------------------------------------------------------------
// serverpassword.h
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
#ifndef id279D1E56_A461_424E_93E3EF155BEFA077
#define id279D1E56_A461_424E_93E3EF155BEFA077

#include <QDateTime>
#include <QString>
#include <QVariant>

class ServerPassword
{
	public:
		class Server
		{
			// [Zalewa] This class might be useful somewhere else.
			// Perhaps it should be renamed and moved out to its own file.
			public:
				static Server deserializeQVariant(const QVariant& var);

				Server()
				{
					d.port = 0;
				}

				const QString& address() const
				{
					return d.address;
				}

				const QString& game() const
				{
					return d.game;
				}

				bool isValid() const
				{
					return !d.address.isEmpty() && d.port != 0;
				}

				const QString& name() const
				{
					return d.name;
				}

				unsigned short port() const
				{
					return d.port;
				}

				void setAddress(const QString& v)
				{
					d.address = v;
				}

				void setGame(const QString& v)
				{
					d.game = v;
				}

				void setName(const QString& v)
				{
					d.name = v;
				}

				void setPort(unsigned short v)
				{
					d.port = v;
				}

				void setTime(const QDateTime& v)
				{
					d.time = v;
				}

				QVariant serializeQVariant() const;
				/**
				 * @brief Similarity to the 'other' server; between 0.0 and 1.0.
				 *
				 * Similarity is calculated basing on address, game, name
				 * and port. Some of these values have more importance than
				 * others, and some require other values to be equal.
				 *
				 * Both servers must be valid.
				 */
				float similarity(const Server& other) const;

				QDateTime time() const
				{
					return d.time;
				}

			private:
				class PrivData
				{
				public:
					QString address;
					QString game;
					QString name;
					unsigned short port;
					QDateTime time;
				};

				PrivData d;
		};

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

		QString lastGame() const;
		Server lastServer() const;
		QString lastServerName() const;
		QDateTime lastTime() const;

		const QList<Server>& servers() const
		{
			return d.servers;
		}

		const QString& phrase() const
		{
			return d.phrase;
		}

		QVariant serializeQVariant() const;

		void addServer(const Server& v);
		void removeServer(const QString& game, const QString& address, unsigned short port);

		void setPhrase(const QString& v)
		{
			d.phrase = v;
		}

	private:
		class PrivData
		{
		public:
			QList<Server> servers;
			QString phrase;
		};
		PrivData d;
};

#endif

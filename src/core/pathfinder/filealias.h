//------------------------------------------------------------------------------
// filealias.h
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
#ifndef id8E04D971_DF78_4DB4_97E84A8D859F3694
#define id8E04D971_DF78_4DB4_97E84A8D859F3694

#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>

class FileAlias
{
	public:
		static FileAlias freeDoom1Aliases();
		static QList<FileAlias> freeDoom2Aliases();
		static QList<FileAlias> standardWadAliases();

		FileAlias();
		FileAlias(const QString &name);

		const QStringList &aliases() const;
		void setAliases(const QStringList &val);

		static FileAlias deserializeQVariant(const QVariant &var);
		QVariant serializeQVariant() const;

		/**
		 * @brief Valid FileAlias has a name and at least one alias.
		 */
		bool isValid() const;

		const QString &name() const;
		void setName(const QString &val);

	private:
		class PrivData
		{
		public:
			QStringList aliases;
			QString name;
		};

		PrivData d;
};

#endif // header

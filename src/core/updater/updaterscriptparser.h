//------------------------------------------------------------------------------
// updaterscriptparser.h
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef DOOMSEEKER_UPDATER_UPDATERSCRIPTPARSER_H
#define DOOMSEEKER_UPDATER_UPDATERSCRIPTPARSER_H

#include <QDomDocument>
#include <QString>

/**
 * @brief Interface to Mendeley updater .xml script files.
 *
 * When UpdaterScriptParser is constructed it operates on a pre-existing
 * QDomDocument. The QDomDocument object must remain valid or any method that
 * operates on its contents will crash the program.
 *
 * It is safe to destroy the parser after the document is destroyed.
 */
class UpdaterScriptParser
{
	public:
		UpdaterScriptParser(QDomDocument& document);
		~UpdaterScriptParser();

		/**
		 * @brief Sets value of package name tags in the script file.
		 *
		 * More than one package in script is considered an error,
		 * and the method won't make any modifications and will return
		 * error message.
		 *
		 * Modifies following paths:
		 * - /packages/package[0]/name/#text
		 * - /install/file[*]/package/#text - "file[*]" means every "file"
		 *   element.
		 *
		 * @return A null QString if successful. If failed then QString 
		 *         contains error message.
		 */
		QString setPackageName(const QString& name);

	private:
		class PrivData;

		static QString msgMainElementMissingError();

		PrivData* d;

		QDomElement mainElement();
		QString setPackageNameInFiles(const QString& name);
		QString setPackageNameInPackages(const QString& name);
};

#endif

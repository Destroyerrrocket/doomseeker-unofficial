//------------------------------------------------------------------------------
// localization.h
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
// Copyright (C) 2012 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef DOOMSEEKER_LOCALIZATION_H
#define DOOMSEEKER_LOCALIZATION_H

#include "localizationinfo.h"

#include <QIODevice>
#include <QList>
#include <QString>
#include <QTranslator>

class Localization
{
	public:
		static bool loadTranslation(const QString& localeName);

		static QList<LocalizationInfo> loadLocalizationsList(const QStringList& definitionsFileSearchDirs);

	private:
		class LocalizationLoader;

		static QList<QTranslator*> currentlyLoadedTranslations;
		static void installQtTranslations(const QString &localeName, QStringList searchPaths);
		static bool installTranslation(const QString &translationName, const QStringList &searchPaths);
		static QTranslator* loadTranslationFile(const QString& translationName, const QStringList& searchPaths);
};

#endif

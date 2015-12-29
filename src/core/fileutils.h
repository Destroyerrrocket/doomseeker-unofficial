//------------------------------------------------------------------------------
// fileutils.h
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
#ifndef DOOMSEEKER_FILEUTILS_H
#define DOOMSEEKER_FILEUTILS_H

#include <QByteArray>
#include <QDir>
#include <QString>
#include <QStringList>

class FileUtils
{
	public:
		static QByteArray md5(const QString &path);
		/**
		 * @brief Uses QFileInfo::operator== to see if 'path' is on
		 * 'candidates' list.
		 */
		static bool containsPath(const QStringList &candidates,
			const QString &path);

		static Qt::CaseSensitivity comparisonSensitivity();

		/**
		 * @brief Deletes all files in specified directory.
		 *
		 * Attempts to delete all files it can. If one file cannot be deleted
		 * then this method will proceed to the next one until all
		 * files are iterated over. Failure to delete even one file will
		 * result in 'false' being returned.
		 *
		 * @param dirPath
		 *     Path to the directory.
		 * @param nameFilters
		 *     Filters as in QDir::setNameFilters().
		 * @param filters
		 *     QDir::Filter
		 * @return true if all files were successfully deleted.
		 */
		static bool rmAllFiles(const QString& dirPath,
			const QStringList & nameFilters = QStringList());
};

#endif

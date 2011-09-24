//------------------------------------------------------------------------------
// ini.h
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
// Copyright (C) 2009 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __INI_H_
#define __INI_H_

#include "ini/inisection.h"
#include "ini/inivariable.h"
#include "global.h"
#include <QHash>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVector>

/**
 *	@brief INI configuration files handler.
 *
 *	This can read INI files from a disk or from memory.
 *
 *	Names of sections and keys are not case sensitive but the letter case is
 *	being preserved when the file is read and saved. This way the programmer
 *	doesn't need to bother with keeping each character 100% accurate and at the
 *	same time is able to format the INI file content in a nice and clean way.
 *
 *	This class implements an extension to the original INI file format:
 *	- ability to create a QString list, by applying names under section with no
 *	'=' character. For example:
 *	@code
 *	[ IniSection ]
 *	name1
 *	name2
 *	name3
 *	@endcode
 *
 *	The currently supported file format is:
 *	@code
 *	# File's top comment line 1
 *	# File's top comment line 2
 *	#
 *	# File's top comment line 4
 *
 *
 *	# Section1's top comment line 1
 *	# Section1's top comment line 2
 *	[ section1 ]	# section1's side comment
 *	# var1's top comment line 1
 *	# var1's top comment line 2
 *	var1=1	# var1's side comment
 * 	var2="Exit game"
 *
 *	[ section2 ]
 *	var1=10
 *	var2="New Game"
 *
 *	[ namelist ]
 *	name1
 *	name2
 *	name3
 *	@endcode
 *
 *	IMPORTANT: If file's top comment should be recognised as such it needs to be
 *	separated by two empty lines from the rest of the file.
 */
class MAIN_EXPORT Ini : public QObject
{
	public:
    	/**
		 *	Constructor that will load the file from a drive.
		 */
		Ini(const QString& filename);
		~Ini();

		/**
		 *	No change to the data will be made if section already exists.
		 *	@return Newly created or existing section.
		 */
		IniSection			createSection(const QString& name);

		/**
		 *	Value of data parameter will be ignored and no changes will be
		 *	performed if setting already exists.
		 *	@return Newly created or existing setting.
		 */
		IniVariable 		createSetting(const QString& sectionname, const QString& name, const QVariant& data);

		/**
		 *	Completely removes a given section.
		 */
		void				deleteSection(const QString& sectionname);

		/**
		 *	Completely removes a given setting. This will not erase the section
		 *	even if it's completely empty.
		 */
		void				deleteSetting(const QString& sectionname, const QString& settingname);

		/**
		 *	@brief Loads .ini file from drive.
		 *
		 *	@param filePath
		 *		Full path to the INI file.
		 *
		 *	@return True if load was successful.
		 */
		bool				loadIniFile(const QString &filePath);

		void				removeKey(const QString& key);

		/**
		 *	This method will not create a new section if it doesn't exist yet.
		 *	@return NULL if section doesn't exist or a pointer to
		 *	internally stored IniSection object. Do not delete this object.
		 */
		IniSection  		retrieveSection(const QString& name);

		/**
		 *	This won't create a variable if it doesn't exist and return NULL
		 *	in such case.
		 *	@return A reference to the internally stored IniVariable if it
		 *	does. Be sure to check if it isNull.
		 */
		IniVariable 		retrieveSetting(const QString& sectionname, const QString& variablename);

		/**
		 *	This will only work if dataSource() == Drive. If the data source
		 *	is from memory there's really nowhere to save to and this will
		 *	return false.
		 */
		bool				save();

		/**
		 *	This method will attempt to retrieve an existing section.
		 *	If this section does not exist a new section will be created.
		 *
		 *	This is in fact an alias to createSection() and it has been
		 *	introduced to fit the IniVariable create/retrieve/setting set
		 *	of methods and to avoid further confusion
		 *	(ie. "why does this crash").
		 *
		 *	@return NULL if section doesn't exist or a pointer to
		 *	internally stored IniSection object. Do not delete this object.
		 */
		IniSection			section(const QString& name);

		/**
		 *	Retrieves references to all sections whose names fit a certain
		 *	pattern. Please remember that internally all sections are stored
		 *	as lower-case strings. The regex pattern will be instructed to
		 *	ignore the case size.
		 */
		QVector<IniSection>	sectionsArray(const QString& regexPattern);

		/**
		 *	This method will create a variable if it doesn't exist. To avoid
		 *	this behavior see: @see retrieveSetting().
		 *	@return Returns a pointer to a IniVariable object. Do not delete
		 *	this object.
		 */
		IniVariable 		setting(const QString& sectionname, const QString& variablename);

		void				setValue(const QString& key, const QVariant& value);
		QVariant			value(const QString& key) const;

	private:
        QSettings*          pIni;
};

#endif

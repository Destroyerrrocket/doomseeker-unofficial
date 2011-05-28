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

#include "inisection.h"

#include "global.h"
#include <QHash>
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
		static IniSection nullSection;

		/**
		 *	DataSource determines where the data came from when the object
		 *	was constructed.
		 */
		enum DataSource
		{
			Drive 	= 0,
			Memory	= 1
		};

		Ini()
		{
			dataSourc = Memory;
		}

		/**
		 *	Constructor that will load the file from a drive.
		 */
		Ini(const QString& filename);

		/**
		 *	Constructor that will load the file from memory.
		 */
		Ini(const QString& displayName, const QByteArray& memorydata);

		Ini(const Ini& other)
		{
			copy(other);
		}

		/**
		 *	No change to the data will be made if section already exists.
		 *	@return Newly created or existing section.
		 */
		IniSection&			createSection(const QString& name);

		/**
		 *	Value of data parameter will be ignored and no changes will be
		 *	performed if setting already exists.
		 *	@return Newly created or existing setting.
		 */
		IniVariable&		createSetting(const QString& sectionname, const QString& name, const IniVariable& data);

		/**
		 *	@return Source type of the INI data, either a physical file or
		 *	a block of memory.
		 */
		DataSource			dataSource() const { return dataSourc; }

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
		 *	@return Errors that occured in this instance of that class.
		 */
		const QStringList&	errors() const { return errorsList; }

		/**
		 *	Converts internally stored structures into a piece of data
		 *	that can be saved into a file.
		 */
		QByteArray			iniData();

		bool				isFirstRun() const { return firstRun; }

		/**
		 *	This is set by the constructor depening on whether it was able
		 *	to read the input data or not.
		 */
		bool				isValid() const { return valid; }

		/**
		 *	Appends additional INI files. Identical settings will be overwrtten
		 *	with data from the new INI. If the object was invalid and this
		 *	method succeeds the object will become valid, however in this case
		 *	the data source will become "memory".
		 *	@return true on successful data appendage
		 */
		bool				loadAdditionalSettings(const QString& filename);
		bool				loadAdditionalSettings(const QByteArray& data);

		/**
		 *	@brief Loads .ini file from drive.
		 *
		 *	@param filePath
		 *		Full path to the INI file.
		 *
		 *	@return True if load was successful.
		 */
		bool				loadIniFile(const QString &filePath);

		Ini&				operator=(const Ini& other);

		/**
		 *	For debug purposes. Prints internally stored data to stdout.
		 */
		void				print() const;

		/**
		 *	This method will not create a new section if it doesn't exist yet.
		 *	@return NULL if section doesn't exist or a pointer to
		 *	internally stored IniSection object. Do not delete this object.
		 */
		IniSection&			retrieveSection(const QString& name);

		/**
		 *	This won't create a variable if it doesn't exist and return NULL
		 *	in such case.
		 *	@return A reference to the internally stored IniVariable if it
		 *	does. Be sure to check if it isNull.
		 */
		IniVariable&		retrieveSetting(const QString& sectionname, const QString& variablename);

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
		IniSection&			section(const QString& name);

		/**
		 *	Retrieves references to all sections whose names fit a certain
		 *	pattern. Please remember that internally all sections are stored
		 *	as lower-case strings. The regex pattern will be instructed to
		 *	ignore the case size.
		 */
		QVector<IniSection*>	sectionsArray(const QString& regexPattern);

		/**
		 *	Sets top comment of the INI file.
		 */
		void				setIniTopComment(const QString& comment) { iniTopComment = comment; }

		/**
		 *	This method will create a variable if it doesn't exist. To avoid
		 *	this behavior see: @see retrieveSetting().
		 *	@return Returns a pointer to a IniVariable object. Do not delete
		 *	this object.
		 */
		IniVariable&		setting(const QString& sectionname, const QString& variablename);

	protected:
		/**
		 *	List where all errors will be passed into.
		 */
		QStringList			errorsList;

		/**
		 *	Source of the initial data, set by constructor.
		 */
		DataSource 			dataSourc;

		/**
		 *	If dataSourc == Drive then this is the name of the file where
		 *	the data was read from, otherwise it is set by the constructor.
		 */
		QString				filename;

		/**
		 *	Top comment for the INI file is stored here.
		 */
		QString				iniTopComment;

		/**
		 *	Variables are stored within this.
		 */
		IniSections			sections;

		/**
		 *	True if the file didn't ini file didn't exist.
		 */
		bool				firstRun;

		/**
		 *	False if it was impossible to process the input data while
		 *	constructing the new object.
		 */
		bool				valid;

		void				copy(const Ini& other);

		IniSection&			parseSectionName(QString& line, bool& ok, const QString& topComment, unsigned lineNum);

		/**
		 *	Converts internally stored structures into an array of bytes that
		 *	construct a INI file.
		 */
		void				structuresIntoQByteArray(QByteArray& output) const;

		/**
		 *	Reads an array of bytes that construct a INI file and converts
		 *	the data into internal structures.
		 */
		void				readQByteArrayIntoStructures(const QByteArray& array);
};

#endif
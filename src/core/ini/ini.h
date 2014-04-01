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
#include <QString>
#include <QStringList>
#include <QVector>

class SettingsProvider;

/**
 * @brief Configuration handler.
 *
 * This class is an extension to simple interface provided by SettingsProvider.
 *
 * This is wrapper provides a hierarchical access
 * to the configuration. The hierarchy is resolved through the use of IniSection
 * and IniVariable classes. The Ini class provides methods necessary to obtain
 * valid instances of these objects.
 *
 * Objects of IniSection and IniVariable classes can be copied over as they
 * hold no actual data. They are used to generate valid paths and internally
 * use methods provided by the Ini class objects that are associated with them.
 * However, Ini object @b must remain valid when associated IniSection and
 * IniVariable objects are in use or segmentation fault will occur.
 *
 * Names of sections and variables are case sensitive.
 *
 * The currently supported file format is:
 * @code
 *
 * [ section1 ]
 * var1=1
 * var2="Exit game"
 *
 * [ section2 ]
 * var1=10
 * var2="New Game"
 *
 * @endcode
 *
 */
class MAIN_EXPORT Ini : public QObject
{
	Q_OBJECT

	public:
		Ini(SettingsProvider* provider);
		virtual ~Ini();

		/**
		 *	No change to the data will be made if section already exists.
		 *	@return Newly created or existing section.
		 */
		IniSection createSection(const QString& name);

		/**
		 *	Value of data parameter will be ignored and no changes will be
		 *	performed if setting already exists.
		 *	@return Newly created or existing setting.
		 */
		IniVariable createSetting(const QString& sectionname, const QString& name, const QVariant& data);

		/**
		 *	Completely removes a given section.
		 */
		void deleteSection(const QString& sectionname);

		/**
		 *	Completely removes a given setting. This will not erase the section
		 *	even if it's completely empty.
		 */
		void deleteSetting(const QString& sectionname, const QString& settingname);

		void removeKey(const QString& key);

		/**
		 *	This method will not create a new section if it doesn't exist yet.
		 *	@return NULL if section doesn't exist or a pointer to
		 *	internally stored IniSection object. Do not delete this object.
		 */
		IniSection retrieveSection(const QString& name);

		/**
		 *	This won't create a variable if it doesn't exist and return NULL
		 *	in such case.
		 *	@return A reference to the internally stored IniVariable if it
		 *	does. Be sure to check if it isNull.
		 */
		IniVariable retrieveSetting(const QString& sectionname, const QString& variablename);

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
		IniSection section(const QString& name);

		/**
		 *	Retrieves references to all sections whose names fit a certain
		 *	pattern. Please remember that internally all sections are stored
		 *	as lower-case strings. The regex pattern will be instructed to
		 *	ignore the case size.
		 */
		QVector<IniSection> sectionsArray(const QString& regexPattern);

		/**
		 *	This method will create a variable if it doesn't exist. To avoid
		 *	this behavior see: @see retrieveSetting().
		 *	@return Returns a pointer to a IniVariable object. Do not delete
		 *	this object.
		 */
		IniVariable setting(const QString& sectionname, const QString& variablename);

		void setValue(const QString& key, const QVariant& value);
		QVariant value(const QString& key) const;

	private:
		class PrivData;
		PrivData *d;
};

#endif

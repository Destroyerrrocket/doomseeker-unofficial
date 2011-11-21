//------------------------------------------------------------------------------
// inivariable.h
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
#ifndef __INIVARIABLE_H__
#define __INIVARIABLE_H__

#include "global.h"

#include <QHash>
#include <QString>
#include <QVariant>

class IniSection;

/**
 *	@brief INI variable representation.
 *
 *	Structure containing variable's value and comments. The name of the
 *	variable is not contained in the structure itself to prevent redundancy.
 */
class MAIN_EXPORT IniVariable
{
	public:
		/**
 		* @brief Creates an invalid IniVariable object. Such object should not
 		*        be used for read/write operations.
 		*/
		IniVariable();

		/**
		 * @brief Creates a valid IniVariable object. Read/write operations are
		 *        permitted.
		 */
		IniVariable(IniSection* pSection, const QString& key);

		/**
	 	* @brief Creates a valid IniVariable, read-only object.
	 	*
		 * Only read operations are permitted. Performing write operations
		 * on such object will fail.
		 */
		IniVariable(const IniSection* pSection, const QString& key);

		/**
		 * @brief If true, IniSection object is not valid and should not be
		 *        used to perform any actions on the Ini file.
		 */
		bool			isNull() const { return pConstSection == NULL; }

		/**
		 * @brief Returns the underlying value as a QString.
		 *
		 * Internally the value is always accessed as QVariant.
		 */
		QString			valueString() const { return this->value().toString(); }

		const IniVariable &operator=(const QString &str);
		const IniVariable &operator=(const char* str) { return *this = QString(str); }
		const IniVariable &operator=(int i);
		const IniVariable &operator=(unsigned int i);
		const IniVariable &operator=(short i);
		const IniVariable &operator=(unsigned short i);

		/**
		 * @brief Sets the value to bool.
		 *
		 * The value is set by converting the bool to a integer, 0 for false
		 * and 1 for true.
		 */
		const IniVariable &operator=(bool b);
		const IniVariable &operator=(float f);
		const IniVariable &operator=(const IniVariable &other);

		/**
		 * @brief Attempts to convert the value to QString.
		 */
		QString operator*() const { return value().toString(); }

		/**
		 * @brief Attempts to convert the value to QString.
		 */
		operator QString () const { return value().toString(); }

		/**
		 * @brief Attempts to convert the value to a float.
		 */
		operator float() const;
		/**
		 * @brief Attempts to convert the value to a integer.
		 */
		operator int() const;
		operator unsigned int() const;

		operator short() const;
		operator unsigned short() const;

		/**
		* @brief Convert value to boolean, if possible. It's done by converting
		*        to numValue() first, then to bool.
		*/
		operator bool() const;
		operator IniVariable&() { return *this; }
		operator const IniVariable&() const { return *this; }

		/**
		 * @brief Explicitly sets the value from QVariant.
		 */
		void				setValue(const QVariant& value);

		/**
		 * @brief Extracts the value as QVariant.
		 */
		QVariant			value() const;

	private:
		friend class TestReadINIVariable;
		friend class TestReadINIList;

		/**
		 * @brief For non-const operations. Might be NULL even if pConstSection
		 *        is not NULL.
		 */
		IniSection*			pSection;

		/**
 		* @brief For const operations. If NULL then IniVariable object is
 		*        invalid.
 		*/
		const IniSection*	pConstSection;

		/**
		 * @brief The key name of this variable.
		 */
		QString				key;


};

typedef QHash<QString, IniVariable>					IniVariables;	// the first QString is the name
typedef QHash<QString, IniVariable>::iterator		IniVariablesIt;
typedef QHash<QString, IniVariable>::const_iterator	IniVariablesConstIt;

#endif
